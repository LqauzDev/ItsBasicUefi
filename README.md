<div align="center">

# ItsBasicUefi

**A modular C framework for building UEFI applications and libraries.**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![UEFI](https://img.shields.io/badge/UEFI-2.4%2B-blue.svg)](https://uefi.org/)
[![C](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)

</div>

---

## Overview

ItsBasicUefi provides a practical set of reusable subsystems for UEFI development. The project is organized as independent libraries so you can include only the capabilities your application needs.

### Core goals

- Keep UEFI development approachable and maintainable.
- Provide clean subsystem boundaries with consistent APIs.
- Support both custom (`ubuild`) and CMake-based workflows.

## Subsystems

- **Uefi.Core** — Core system table/bootstrap helpers.
- **Uefi.StdLib** — Minimal standard-library-like utilities for UEFI.
- **Uefi.Graphics** — Basic 2D drawing primitives.
- **Uefi.Input** — Keyboard and pointing-device input helpers.
- **Uefi.Crypto** — Hashing and symmetric cryptography helpers.
- **Uefi.Acpi** — ACPI table discovery and parsing helpers.
- **Uefi.Ovmf** — OVMF/virtualization environment detection.
- **Uefi.Timer** — Timing and datetime services.
- **Uefi.FileSystem** — File-system abstraction layer.
- **Uefi.Network** — Network protocol abstractions.
- **Uefi.Memory** — **New memory subsystem** with pool wrappers, arena allocation, memory utilities, and runtime allocation statistics.
- **Uefi.Build** — Lightweight custom project builder.

## Repository Layout

```text
Uefi.Core/
Uefi.StdLib/
Uefi.Graphics/
Uefi.Input/
Uefi.Crypto/
Uefi.Acpi/
Uefi.Ovmf/
Uefi.Timer/
Uefi.FileSystem/
Uefi.Network/
Uefi.Memory/
Uefi.Build/
Examples/
```

## Building

### Option 1: Make

```bash
make all
```

Common targets:

```bash
make build-system
make build-libs
make build-examples
make clean
```

### Option 2: CMake

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Using the New Memory Subsystem

`Uefi.Memory` provides pool-backed arena allocation and memory primitives designed for constrained UEFI contexts.

### Example

```c
#include "memory.h"

MemoryArena arena;

if (memory_init() && memory_arena_create(&arena, 4096, EfiBootServicesData)) {
    void* packet = memory_arena_allocate(&arena, 512, 16);
    memory_set(packet, 0, 512);

    // ... use packet ...

    memory_arena_destroy(&arena);
}
```

### API highlights

- `memory_allocate_pool` / `memory_free_pool`
- `memory_arena_create` / `memory_arena_allocate` / `memory_arena_reset` / `memory_arena_destroy`
- `memory_get_stats` / `memory_reset_stats`
- `memory_set` / `memory_copy` / `memory_compare`

## Creating a Project with `ubuild`

Each library/application has a `project.ubuild` file:

- `name`: Project name
- `type`: `lib` or `app`
- `sources`: Space-separated source files
- `includes`: Space-separated include directories
- `output`: Artifact name
- `depends`: Space-separated dependencies

## Examples

The `Examples/` directory includes sample applications for:

- basic boot/application startup
- graphics + input interaction
- OVMF detection
- cryptographic operations

## Contributing

Please see [CONTRIBUTING.md](CONTRIBUTING.md). Contributions are welcome, especially:

- additional subsystems
- documentation improvements
- new platform validation and examples

## License

Distributed under the MIT License. See [LICENSE](LICENSE) for details.
