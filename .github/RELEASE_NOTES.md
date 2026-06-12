## SysMon v0.7.0

A Windows kernel driver (KMDF/WDM) that monitors system activity in real time
and streams events to a user-mode client.

### What's new in v0.7.0 — Remote Thread Detection

- **Remote thread detection**: SysMon now detects when a thread is created in a process by a *different* process — a common technique used by code injection attacks.
- New `RemoteThread` event type carrying creator process ID, creator thread ID, target process ID, and target thread ID.
- New `ExecutiveResource` synchronization wrapper (`ERESOURCE`) used to protect the new-process tracking list with shared/exclusive locking.
- New `LookasideList<T>` helper for pool-efficient item allocation.
- New `Locker<T>` RAII wrapper for automatic lock/unlock of any synchronization primitive.
- Client updated to display remote thread events.

### Events captured
- Process creation (PID, parent PID, command line)
- Process exit (exit code)
- Thread creation / exit (thread ID, exit code)
- Image load (file path, load address, size)
- **Remote thread creation** (creator PID/TID, target PID/TID)

### Requirements
- Windows 10/11 (test signing must be enabled)
- Visual C++ Redistributable 2022

### Installation

**1. Enable test signing** (run as Administrator, then reboot):
```bat
bcdedit /set testsigning on
```

**2. Install the certificate** (run as Administrator):
```bat
certutil -addstore Root SysMon.cer
certutil -addstore TrustedPublisher SysMon.cer
```

**3. Install and start the driver**:
```bat
sc create SysMon type= kernel binPath= C:\path\to\SysMon.sys
sc start SysMon
```

**4. Run the client**:
```bat
SysMonClient.exe
```

### Packages

| Package | Framework | Config |
|---------|-----------|--------|
| `SysMon-kmdf-rel.zip` | KMDF | Release |
| `SysMon-wdm-rel.zip`  | WDM  | Release |
| `SysMon-kmdf-dbg.zip` | KMDF | Debug   |
| `SysMon-wdm-dbg.zip`  | WDM  | Debug   |
