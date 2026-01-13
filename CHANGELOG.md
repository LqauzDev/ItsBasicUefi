# Changelog

All notable changes to ItsBasicUefi will be documented in this file.

## [1.0.0] - 2026-01-05

### Added
- Initial release of ItsBasicUefi framework
- Core UEFI abstraction layer (Uefi.Core)
- Standard library implementation (Uefi.StdLib)
- Custom build system (Uefi.Build)
- Graphics rendering library (Uefi.Graphics)
- Input device management (Uefi.Input)
- Cryptographic functions (Uefi.Crypto)
- ACPI table management (Uefi.Acpi)
- OVMF virtualization detection (Uefi.Ovmf)
- Timer and datetime services (Uefi.Timer)
- File system operations framework (Uefi.FileSystem)
- Network protocol support framework (Uefi.Network)

### Features
- Modular architecture with clean separation of concerns
- Configuration-driven build system with project.ubuild files
- SHA-1, SHA-256, MD5, and AES cryptographic support
- 2D graphics rendering with shapes and colors
- Keyboard, mouse, and touch input abstraction
- High-resolution timing and datetime functions
- ACPI table parsing and system information
- OVMF/QEMU virtualization environment detection
- Complete example applications demonstrating all features

### Examples
- HelloWorld - Basic "Hello World" UEFI application
- OvmfDetect - Virtualization environment detection demo
- GraphicsInput - Interactive graphics and input demo
- CryptoDemo - Cryptographic functions demonstration

### Documentation
- Complete README with getting started guide
- API documentation for all libraries
- Build system documentation
- Example usage guides
