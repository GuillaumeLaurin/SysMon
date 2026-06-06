# SysMon

A Windows kernel driver (WDM/KMDF) that captures system events — process creation, process exit, and more — and streams them to a user-mode client application in real time.

## Architecture

```
┌─────────────────────────────┐        ┌─────────────────────────┐
│        Kernel space         │        │      User space         │
│                             │        │                         │
│  PsSetCreateProcessNotify   │        │    SysMonClient.exe     │
│           │                 │        │         │               │
│           ▼                 │        │    ReadFile loop        │
│    OnProcessNotify()        │        │    (every 400 ms)       │
│           │                 │        │         │               │
│           ▼                 │  read  │         ▼               │
│    g_State (event queue) ───┼────────┼──► DisplayInfo()        │
│    (FastMutex-protected)    │        │    prints PID, cmdline, │
│                             │        │    timestamps           │
│    Device: \\Device\SysMon  │        │                         │
│    SymLink: \\.\SysMon      │        │ opens \\.\SysMon        │
└─────────────────────────────┘        └─────────────────────────┘
```

The kernel driver registers a process-notification callback via `PsSetCreateProcessNotifyRoutineEx`. Each event is serialized into a tagged struct, pushed into a thread-safe linked list, and flushed to the client on every `IRP_MJ_READ`.

## Events captured

| Event | Struct | Fields |
|-------|--------|--------|
| Process created | `ProcessCreateInfo` | PID, parent PID, creating thread/process ID, command line |
| Process exited | `ProcessExitInfo` | PID, exit code |

All events share a common `ItemHeader` with type tag, size, and timestamp (`LARGE_INTEGER`).

## Project structure

```
SysMon/
├── cmake/
│   └── FindWDK.cmake          # Automatic WDK detection module
├── include/
│   ├── Common.h               # Kernel includes and logging macros
│   ├── Device.h               # Device object interface
│   ├── FastMutex.h            # Kernel FAST_MUTEX RAII wrapper
│   ├── Globals.h              # Global driver state (event queue)
│   ├── Locker.h               # Scoped lock guard
│   ├── Public.h               # Shared event structs (kernel + client)
│   └── Queue.h                # I/O queue and IOCTL interface
├── src/
│   ├── Driver.cpp             # DriverEntry / DriverUnload
│   ├── Device.cpp             # Device creation + OnProcessNotify callback
│   ├── Queue.cpp              # IRP dispatch (Read, Write, DeviceControl)
│   ├── FastMutex.cpp          # FastMutex implementation
│   └── Globals.cpp            # Thread-safe event queue implementation
├── client/
│   └── src/
│       └── Client.cpp         # User-mode polling client
├── inf/
│   └── SysMon.inf.in          # INF template (expanded by CMake)
├── .github/
│   └── workflows/
│       └── build.yml          # GitHub Actions CI pipeline
├── CMakeLists.txt
├── CMakePresets.json
└── .gitignore
```

---

## Requirements

| Tool | Minimum version | Link |
|------|----------------|------|
| Windows 11 / 10 (dev machine) | 22H2+ | — |
| Visual Studio | 2022 (17.x) | [Download](https://visualstudio.microsoft.com/) |
| VS workload | Desktop development with C++ | Via VS Installer |
| CMake | 3.20+ | [Download](https://cmake.org/download/) |
| Windows Driver Kit (WDK) | 10.0.22621.0+ | [Download](https://learn.microsoft.com/windows-hardware/drivers/download-the-wdk) |

> The WDK version must match the Windows SDK version installed alongside Visual Studio.

---

## Build

### Quick start (CMake Presets)

```bat
REM Debug
cmake --preset debug
cmake --build --preset debug

REM Release
cmake --preset release
cmake --build --preset release
```

This produces two outputs:
- `build/debug/Debug/SysMon.sys` — the kernel driver
- `build/debug/SysMonClient.exe` — the user-mode client

### Manual (non-standard WDK path)

```bat
cmake -G "Visual Studio 17 2022" -A x64 ^
      -DWDK_ROOT="D:/WDK/10" ^
      -DWDK_VERSION="10.0.22621.0" ^
      -B build/custom
cmake --build build/custom --config Debug
```

### Available CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_KMDF` | `ON` | Use KMDF (recommended) |
| `USE_WDM` | `OFF` | Use low-level WDM |
| `BUILD_CLIENT` | `ON` | Build the user-mode client |
| `DRIVER_SIGN` | `OFF` | Sign the `.sys` after build |
| `WDK_ROOT` | Auto | WDK root path |
| `WDK_VERSION` | Auto | WDK version string (`10.0.XXXXX.0`) |
| `KMDF_VERSION` | Auto | KMDF version string (`1.XX`) |

---

## Deployment on a test machine

### 1. Enable Test Signing Mode

On the test machine (as administrator):

```bat
bcdedit /set testsigning on
REM Reboot the machine
```

> Never enable test signing on a production machine.

### 2. Create a self-signed test certificate

```bat
cmake --build build/debug --target create_test_cert
```

### 3. Install the driver

```bat
cmake --build build/debug --target install_driver
```

Or manually with `devcon`:

```bat
devcon install build\debug\SysMon.inf Root\SysMon
```

### 4. Run the client

```bat
build\debug\SysMonClient.exe
```

Expected output:

```
14:32:01.042: Process 7812 Created. Command line: "C:\Windows\System32\notepad.exe"
14:32:04.317: Process 7812 Exited (Code: 0)
```

### 5. Uninstall

```bat
cmake --build build/debug --target uninstall_driver
REM or
devcon remove Root\SysMon
```

---

## Debugging

### WinDbg (kernel debugging)

On the target machine, enable kernel debugging:

```bat
bcdedit /debug on
bcdedit /dbgsettings net hostip:<DEV_MACHINE_IP> port:50000 key:1.2.3.4
```

Then in WinDbg on the development machine:

```
File > Attach to Kernel > Net
Port: 50000 | Key: 1.2.3.4
```

### Debug messages (DbgPrint)

The `LOG_*` macros in `Common.h` emit messages visible in:
- **DebugView** (Sysinternals) — without a debugger attached
- **WinDbg** — with a kernel debugger attached

To enable kernel messages in DebugView: `Capture > Capture Kernel`

---

## CI/CD

The GitHub Actions pipeline (`.github/workflows/build.yml`):
- Automatically installs the WDK (with caching)
- Builds Debug and Release configurations
- Publishes `.sys`, `.pdb`, `.inf`, and `SysMonClient.exe` as build artifacts

---

## Resources

- [WDK Documentation](https://learn.microsoft.com/windows-hardware/drivers/)
- [Windows driver samples](https://github.com/microsoft/Windows-driver-samples)
- [WDF API reference](https://learn.microsoft.com/windows-hardware/drivers/ddi/_wdf/)
- [PsSetCreateProcessNotifyRoutineEx](https://learn.microsoft.com/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex)
- [Static Driver Verifier](https://learn.microsoft.com/windows-hardware/drivers/devtest/static-driver-verifier)
- [Driver Verifier](https://learn.microsoft.com/windows-hardware/drivers/devtest/driver-verifier)
