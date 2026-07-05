## SysMon v1.0.0

A Windows kernel driver (KMDF/WDM) that monitors system activity in real time,
now paired with a full ImGui/DirectX 11 desktop client with SQLite-backed
persistence, structured logging, and a dedicated error-reporting pipeline.

### What's new in v1.0.0 — GUI client rewrite

The user-mode client has been completely rewritten from a console polling
loop into a layered, dependency-injected desktop application:

- **Dependency injection**: `Application` + `DIContainer` wire every service
  (driver connector, event processor, storage, logging, reporter) as
  singletons resolved by type.
- **Driver integration**: `DriverService` starts/stops the `SysMon` service
  through the SCM; `DriverConnector` opens `\\.\sysmon`; `EventProcessor`
  polls the driver on a background thread and decodes raw buffers back into
  typed events.
- **Storage module**: events and settings are now persisted to a local
  SQLite database (`Database`, `EventRepository`, `ConfigRepository`)
  instead of being printed and discarded.
- **GUI**: a windowed ImGui/DirectX 11 interface (`Win32Window`,
  `DX11Renderer`, `UIRenderer`) replaces the console output, with page
  navigation (`Router`, `PageManager`) across five screens:
  - **Dashboard** — live stat cards and recent activity
  - **Processes** — process creation/exit history
  - **Threads** — thread creation/exit and remote thread (injection) alerts
  - **Images** — loaded image/module history
  - **Settings** — user-configurable refresh interval and max visible rows
  - Shared components: `Sidebar`, `EventTable`, `FilterBar`, `StatCard`, `AlertBanner`
- **Logging**: `Logger` (spdlog-backed, console + rotating file) replaces
  ad-hoc console prints.
- **Error reporting pipeline**: every `SysMonException` and Error/Fatal log
  entry is queued (`ErrorQueue`), filtered (`SeverityFilter`,
  `CategoryFilter`, `RateLimitFilter`), formatted as JSON (`JsonFormatter`)
  and dispatched (`ErrorDispatcher`) to `ConsoleSink`/`FileSink`.
  `DumpProvider` writes a fingerprinted `.dmp` crash dump for unhandled
  exceptions caught by `ExceptionHandler`.
- **Typed exceptions**: `SysMonException` hierarchy (`ConfigException`,
  `DeviceException`, `StorageException`) thrown via `THROW_*` macros and
  caught centrally.
- New vendored dependencies: [Dear ImGui](https://github.com/ocornut/imgui),
  [spdlog](https://github.com/gabime/spdlog),
  [nlohmann/json](https://github.com/nlohmann/json) (submodules), and a
  vendored SQLite3 amalgamation.
- Full Javadoc/Doxygen documentation pass across the entire codebase
  (driver + client, headers and sources).

### Events captured
- Process creation (PID, parent PID, command line)
- Process exit (exit code)
- Thread creation / exit (thread ID, exit code)
- Image load (file path, load address, size)
- Remote thread creation (creator PID/TID, target PID/TID) — carried over from v0.7.0

### Requirements
- Windows 10/11 (test signing must be enabled)
- Visual C++ Redistributable 2022
- DirectX 11 capable GPU (required to run the client)

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

**3. Install the driver**:
```bat
sc create SysMon type= kernel binPath= C:\path\to\SysMon.sys
```

**4. Run the client** (as Administrator — it starts the driver service for you):
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
