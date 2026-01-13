<div align="center">

#  ItsBasicUefi

**Modern UEFI Development Made Simple**

*A lightweight, modular framework that brings elegance to low-level programming*

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![UEFI](https://img.shields.io/badge/UEFI-2.4+-blue.svg)](https://uefi.org/)
[![C](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)

</div>

---

## Overview

ItsBasicUefi transforms complex UEFI development into a clean, intuitive experience. Built for developers who demand both performance and simplicity, our framework provides powerful abstractions while maintaining direct hardware access when you need it.

## Features

- **Modular Architecture**: Clean separation of concerns with independent libraries
- **Custom Build System**: Simple configuration-driven building
- **Standard Library**: C standard library implementation for UEFI
- **Graphics Support**: 2D graphics rendering with shapes and colors
- **Input Handling**: Keyboard, mouse, and touch input abstraction
- **Cryptographic Functions**: SHA-1, SHA-256, MD5, and AES support
- **ACPI Support**: ACPI table parsing and system information
- **OVMF Integration**: Virtualization environment detection
- **Timer Services**: High-resolution timing and datetime functions
- **File System**: File system operations (extensible)
- **Network**: Network protocol support (extensible)

## Library Structure

```
Uefi.Core/          - Core UEFI functionality and system table abstraction
Uefi.StdLib/        - Standard library implementation (printf, stdlib, string)
Uefi.Graphics/      - 2D graphics rendering
Uefi.Input/         - Input device management (keyboard, mouse, touch)
Uefi.Crypto/        - Cryptographic functions (SHA, MD5, AES)
Uefi.Acpi/          - ACPI table management
Uefi.Ovmf/          - OVMF virtualization detection
Uefi.Timer/         - Timer and datetime services
Uefi.FileSystem/    - File system operations
Uefi.Network/       - Network protocol support
Uefi.Build/         - Custom build system
Examples/            - Example applications
```

<div align="center">

## Quick Start

### Prerequisites

- GCC with UEFI support
- UEFI development environment
- OVMF for testing (recommended)

</div>

---

## Building

### Linux
```bash
# Build everything
make all

# Or build step by step
make build-system
make build-libs
make build-examples
```

### Windows
```cmd
# Using MSYS2/MinGW
mingw32-make all

# Or build manually
cd Uefi.Build
gcc -o ubuild main.c
./ubuild build Uefi.Core
```

### Cross-Platform
```bash
# Using CMake (recommended for complex projects)
mkdir build && cd build
cmake ..
make
```

### Creating a New Application

1. Create a directory for your application
2. Create a `project.ubuild` file:
   ```
   name=MyApp
   type=app
   sources=main.c
   includes=include ../../Uefi.Core/include ../../Uefi.StdLib/include
   output=myapp.efi
   depends=Uefi.Core Uefi.StdLib
   ```
3. Write your application code
4. Build with: `./ubuild build MyApp`

## Example Usage

```c
#include "../../Uefi.Core/include/uefi_lib.h"
#include "../../Uefi.StdLib/include/stdio.h"

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    uefi_init(SystemTable);
    
    printf("Hello, UEFI World!\r\n");
    
    return EFI_SUCCESS;
}
```

## Configuration

Each library and application has a `project.ubuild` configuration file:

- `name`: Project name
- `type`: Project type (`lib` or `app`)
- `sources`: Source files (space-separated)
- `includes`: Include directories (space-separated)
- `output`: Output file name
- `depends`: Dependencies (space-separated library names)

<div align="center">

## 📚 API Documentation

### Core Functions
- `uefi_init()` - Initialize UEFI system
- `uefi_print()` - Print string to console
- `uefi_println()` - Print string with newline
- `uefi_wait_for_key()` - Wait for key press

### Graphics
- `graphics_init()` - Initialize graphics context
- `graphics_clear()` - Clear screen with color
- `graphics_draw_rect()` - Draw rectangle
- `graphics_fill_circle()` - Fill circle

### Input
- `input_init()` - Initialize input system
- `input_wait_for_key()` - Wait for key press
- `input_get_mouse_position()` - Get mouse coordinates

### Crypto
- `sha256_hash_string()` - Calculate SHA-256 hash
- `crypto_generate_random_bytes()` - Generate random bytes

## 🧪 Testing

The framework includes several example applications in the `Examples/` directory:

- `HelloWorld` - Basic "Hello World" application
- `OvmfDetect` - OVMF environment detection
- `GraphicsInput` - Interactive graphics demo
- `CryptoDemo` - Cryptographic functions demonstration

## 📄 License

This project is provided as-is for educational and development purposes under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style
- New libraries follow the established structure
- Include examples for new features
- Update documentation

## 🆘 Support

For issues and questions, please refer to the example applications and API documentation.

---

<div align="center">

**Built with ❤️ for the UEFI community**

*Transforming low-level development, one line at a time*

</div>
