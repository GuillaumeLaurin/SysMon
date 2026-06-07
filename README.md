# SysMon

A Windows kernel driver (WDM/KMDF) that captures system events — process creation/exit, thread creation/exit, and image loads — and streams them to a user-mode client application in real time.

## Architecture

```
┌──────────────────────────────────────────┐      ┌─────────────────────────┐
│               Kernel space               │      │       User space        │
│                                          │      │                         │
│  PsSetCreateProcessNotifyRoutineEx ──►  OnProcessNotify()                 │
│  PsSetCreateThreadNotifyRoutine    ──►  OnThreadNotify()                  │
│  PsSetLoadImageNotifyRoutine       ──►  OnImageNotify()                   │
│                      │                   │      │   SysMonClient.exe      │
│                      ▼                   │      │        │                │
│              FullItem<T> allocated       │      │   ReadFile loop         │
│              pushed to g_State           │      │   (every 200 ms)        │
│              (FastMutex-protected)       │      │        │                │
│                      │                   │ read │        ▼                │
│              Device: \\Device\sysmon ────┼──────┼──► DisplayInfo()        │
│              SymLink: \\.\sysmon         │      │    prints events        │
└──────────────────────────────────────────┘      └─────────────────────────┘
```

Each event callback allocates a `FullItem<T>` from the paged pool, fills the typed `Data` field, and pushes it into a thread-safe linked list (`g_State`). The client drains the list on every `IRP_MJ_READ` via `memcpy` into its buffer.

---

## Events captured

| Event | Struct | Key fields |
|-------|--------|------------|
| Process created | `ProcessCreateInfo` | PID, parent PID, creating thread/process, command line |
| Process exited | `ProcessExitInfo` | PID, exit code |
| Thread created | `ThreadCreateInfo` | thread ID, owning process ID |
| Thread exited | `ThreadExitInfo` | thread ID, owning process ID, exit code |
| Image loaded | `ImageLoadInfo` | PID, image size, load address, image file path |

All structs inherit `ItemHeader` which carries the event type tag, size, and a `LARGE_INTEGER` timestamp.

### Internal event layout

```cpp
// Kernel-internal wrapper — LIST_ENTRY + typed data, one allocation
template<typename T>
struct FullItem {
    LIST_ENTRY Entry;   // intrusive list node
    T Data;             // ProcessCreateInfo / ThreadExitInfo / …
};

// Queue operations use the common base for size-agnostic reads
auto info = CONTAINING_RECORD(entry, FullItem<ItemHeader>, Entry);
auto size = info->Data.Size;
memcpy(buffer, &info->Data, size);
```

---

## Project structure

```
SysMon/
├── .github/
│   └── workflows/
│       └── build.yml          # CI — build, sign, release
├── cmake/
│   ├── FindWdk.cmake          # WDK auto-detection and target helper
│   └── SignDriver.cmake       # Post-build signing script (cmake -P)
├── include/
│   ├── Common.h               # Kernel includes, logging macros, pool helpers
│   ├── Device.h               # Device creation / notify callback declarations
│   ├── FastMutex.h            # FAST_MUTEX RAII wrapper
│   ├── Globals.h              # Global driver state declaration
│   ├── Locker.h               # Scoped lock guard
│   ├── Public.h               # Shared event structs (kernel + client)
│   └── Queue.h                # I/O queue declarations and IOCTL codes
├── src/
│   ├── Driver.cpp             # DriverEntry / DriverUnload (KMDF + WDM)
│   ├── Device.cpp             # Device creation + all notify callbacks
│   ├── Queue.cpp              # IRP dispatch — Read, Write, DeviceControl
│   ├── FastMutex.cpp          # FastMutex implementation
│   └── Globals.cpp            # Thread-safe event queue implementation
├── client/
│   └── src/
│       └── Client.cpp         # User-mode polling client
├── inf/
│   └── SysMon.inf.in          # INF template (expanded by CMake)
├── scripts/
│   └── Export-SigningCert.ps1 # One-shot helper: export cert for CI secrets
├── CMakeLists.txt
├── CMakePresets.json
└── .gitignore
```

---

## Requirements

| Tool | Minimum version |
|------|----------------|
| Windows 10/11 (dev machine) | 22H2+ |
| Visual Studio | 2022 (17.x) with *Desktop development with C++* workload |
| CMake | 3.20+ |
| Windows Driver Kit (WDK) | 10.0.22621.0+ — [Download](https://learn.microsoft.com/windows-hardware/drivers/download-the-wdk) |

> The WDK version must match the Windows SDK version installed alongside Visual Studio.

---

## Build

### Quick start (CMake Presets)

```bat
REM KMDF Debug
cmake --preset debug
cmake --build --preset debug

REM KMDF Release
cmake --preset release
cmake --build --preset release

REM WDM Debug
cmake --preset wdm-debug
cmake --build --preset wdm-debug

REM WDM Release
cmake --preset wdm-release
cmake --build --preset wdm-release
```

Each preset produces:
- `build/<preset>/<Config>/SysMon.sys` — the kernel driver
- `build/<preset>/SysMonClient.exe` — the user-mode client
- `build/<preset>/SysMon.inf` — the driver INF

### Manual configure (non-standard WDK path)

```bat
cmake -G "Visual Studio 17 2022" -A x64 ^
      -DWDK_ROOT="D:/WDK/10" ^
      -DWDK_VERSION="10.0.26100.0" ^
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

## Driver signing

Signing is required for the driver to load on a target machine in test mode. A self-signed certificate is sufficient for development and testing.

### 1. Create the self-signed certificate (once per machine)

```bat
cmake --build build/debug --target create_test_cert
```

This creates `CN=SysMon` in `Cert:\CurrentUser\My` using `New-SelfSignedCertificate` (valid 10 years).

### 2. Build with signing enabled

```bat
cmake --preset release -DDRIVER_SIGN=ON
cmake --build --preset release
```

The `release-signed` preset has `DRIVER_SIGN=ON` pre-configured:

```bat
cmake --preset release-signed
cmake --build --preset release-signed
```

Signing is done via `cmake/SignDriver.cmake` which calls `signtool.exe` using the `CN=SysMon` certificate from `Cert:\CurrentUser\My`.

---

## Deployment on a test machine

### 1. Enable Test Signing Mode

On the target machine (as Administrator), then reboot:

```bat
bcdedit /set testsigning on
```

> Never enable test signing on a production machine.

### 2. Install the certificate

Copy the `.cer` file to the target machine, then run as Administrator:

```bat
certutil -addstore Root SysMon.cer
certutil -addstore TrustedPublisher SysMon.cer
```

### 3. Install the driver

```bat
sc create SysMon type= kernel binPath= C:\path\to\SysMon.sys
sc start SysMon
```

Or via the CMake target (requires `devcon` in the WDK tools):

```bat
cmake --build build/release --target install_driver
```

### 4. Run the client

```bat
build\release\SysMonClient.exe
```

Expected output:

```
[14:32:01.042] Process 7812 Created  | "C:\Windows\System32\notepad.exe"
[14:32:01.055] Thread 8104 Created   | Process 7812
[14:32:04.317] Image Loaded          | Process 7812 | notepad.exe  @ 0x7FF6A3B00000
[14:32:09.001] Thread 8104 Exited    | Process 7812 | Code: 0
[14:32:09.002] Process 7812 Exited   | Code: 0
```

### 5. Uninstall

```bat
sc stop SysMon
sc delete SysMon
REM or
cmake --build build/release --target uninstall_driver
```

---

## CI/CD

The GitHub Actions pipeline (`.github/workflows/build.yml`) builds all four configurations (KMDF debug/release + WDM debug/release) on every push and pull request.

### What the pipeline does

| Step | Detail |
|------|--------|
| Install WDK | Silent install, cached between runs |
| Import certificate | From `DRIVER_CERT_PFX_B64` secret — skipped on PRs from forks |
| Configure | `DRIVER_SIGN=ON` for release builds when cert is present, `OFF` otherwise |
| Build | All four CMake presets |
| Export `.cer` | Public certificate included in the release package |
| Upload artifacts | One `.zip` per configuration, retained 30 days |
| GitHub Release | Created automatically on `v*` tags, includes all zips + install instructions |

### Setting up signing secrets (one-time)

**1.** Create the certificate locally (if not done):
```bat
cmake --build build/debug --target create_test_cert
```

**2.** Export it for GitHub Secrets:
```powershell
.\scripts\Export-SigningCert.ps1
```

This prints the base64-encoded PFX directly to the console (nothing is written to disk).

**3.** Add the two secrets in **GitHub > Settings > Secrets and variables > Actions**:

| Secret name | Value |
|-------------|-------|
| `DRIVER_CERT_PFX_B64` | Base64 string printed by the script |
| `CERT_PASSWORD` | Password entered during export (can be empty) |

**4.** Trigger a release by pushing a version tag:
```bash
git tag v1.0.0
git push origin v1.0.0
```

---

## Debugging

### WinDbg kernel debugging

On the target machine (as Administrator):

```bat
bcdedit /debug on
bcdedit /dbgsettings net hostip:<DEV_MACHINE_IP> port:50000 key:1.2.3.4
REM Reboot
```

In WinDbg on the dev machine:

```
File > Attach to Kernel > Net
Port: 50000  |  Key: 1.2.3.4
```

Enable all DbgPrint output:

```
ed nt!Kd_DEFAULT_MASK 0xFFFFFFFF
```

### Debug messages (DbgPrint)

`LOG_INFO`, `LOG_WARN`, `LOG_ERROR`, `LOG_TRACE` in `Common.h` emit messages visible in:
- **DebugView** (Sysinternals) — without a debugger attached (`Capture > Capture Kernel`)
- **WinDbg** — with a kernel debugger attached

Messages are compiled out in Release builds (`#if DBG`).

---

## Resources

- [WDK Documentation](https://learn.microsoft.com/windows-hardware/drivers/)
- [Windows driver samples](https://github.com/microsoft/Windows-driver-samples)
- [WDF API reference](https://learn.microsoft.com/windows-hardware/drivers/ddi/_wdf/)
- [PsSetCreateProcessNotifyRoutineEx](https://learn.microsoft.com/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex)
- [PsSetCreateThreadNotifyRoutine](https://learn.microsoft.com/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreatethreadnotifyroutine)
- [PsSetLoadImageNotifyRoutine](https://learn.microsoft.com/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetloadimagenotifyroutine)
- [Driver Verifier](https://learn.microsoft.com/windows-hardware/drivers/devtest/driver-verifier)
- [Static Driver Verifier](https://learn.microsoft.com/windows-hardware/drivers/devtest/static-driver-verifier)
