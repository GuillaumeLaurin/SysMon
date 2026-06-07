## SysMon v0.5.0

A Windows kernel driver (KMDF/WDM) that monitors system activity in real time
and streams events to a user-mode client.

### Events captured
- Process creation (PID, parent PID, command line)
- Process exit (exit code)
- Thread creation / exit (thread ID, exit code)
- Image load (file path, load address, size)

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
