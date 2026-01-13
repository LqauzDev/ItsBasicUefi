# Contributing to ItsBasicUefi

Thank you for your interest in contributing to ItsBasicUefi! This document provides guidelines for contributors.

## Getting Started

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes
4. Test your changes thoroughly
5. Submit a pull request

## Code Style

### C Code Style
- Use 4 spaces for indentation (no tabs)
- Maximum line length: 80 characters
- Function names in snake_case: `function_name()`
- Variable names in snake_case: `variable_name`
- Constants in UPPER_CASE: `CONSTANT_NAME`
- Struct names in PascalCase: `StructName`
- Header guards in format: `FILENAME_H`

### Example
```c
#ifndef FILENAME_H
#define FILENAME_H

#include <stdint.h>

typedef struct {
    uint32_t field_one;
    uint32_t field_two;
} ExampleStruct;

int example_function(ExampleStruct* param);

#endif // FILENAME_H
```

## Library Structure

When adding a new library:

1. Create directory `Uefi.LibraryName/`
2. Add `include/library.h` with public API
3. Add `src/library.c` with implementation
4. Create `project.ubuild` configuration file
5. Add example in `Examples/LibraryNameDemo/`
6. Update documentation

### project.ubuild Format
```
name=Uefi.LibraryName
type=lib
sources=src/library.c
includes=include ../Uefi.Core/include ../Uefi.StdLib/include
output=libuefi_library.a
depends=Uefi.Core Uefi.StdLib
```

## Testing

- All new features must include examples
- Test on real hardware and in OVMF/QEMU
- Ensure compatibility with UEFI 2.4+
- Test memory management thoroughly

## Documentation

- Update README.md for new libraries
- Add API documentation in header files
- Include usage examples
- Update CHANGELOG.md

## Submitting Changes

1. Ensure your code follows the style guidelines
2. Test all functionality
3. Update documentation
4. Create a descriptive pull request
5. Link to any relevant issues

## Bug Reports

When reporting bugs:
- Include system information (UEFI version, hardware)
- Provide minimal reproduction case
- Include error messages and logs
- Describe expected vs actual behavior

## Feature Requests

- Open an issue with "Feature Request" label
- Describe the use case
- Suggest API design
- Consider implementation complexity

## Community

- Be respectful and constructive
- Help others with their issues
- Share knowledge and experience
- Follow the code of conduct

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
