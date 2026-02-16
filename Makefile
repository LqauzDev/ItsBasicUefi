# ItsBasicUefi Build System
# Supports Linux, Windows (MSYS2/MinGW), and macOS

# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(UNAME_S)
endif

# Compiler and tools configuration
ifeq ($(DETECTED_OS),Windows)
    CC := gcc
    AR := ar
    RM := del /Q
    MKDIR := mkdir
    CP := copy
    SEP := \
else
    CC := gcc
    AR := ar
    RM := rm -f
    MKDIR := mkdir -p
    CP := cp
    SEP := /
endif

# Compiler settings
CFLAGS := -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -nostdlib
LDFLAGS := -nostdlib -Wl,--dll -Wl,--subsystem,efi_application -Wl,--entry=UefiMain
INCLUDES := -I$(ROOT_DIR)$(SEP)Uefi.Core$(SEP)include -I$(ROOT_DIR)$(SEP)Uefi.StdLib$(SEP)include

# Platform-specific flags
ifeq ($(DETECTED_OS),Darwin)
    # macOS specific flags
    CFLAGS += -target x86_64-unknown-windows
    LDFLAGS += -target x86_64-unknown-windows
endif

# Directories
ROOT_DIR := .
BUILD_DIR := build
LIB_DIR := $(BUILD_DIR)$(SEP)lib
BIN_DIR := $(BUILD_DIR)$(SEP)bin

# Default target
.PHONY: all
all: build-system build-libs build-examples

# Build the ubuild tool
.PHONY: build-system
build-system:
	@echo "Building build system for $(DETECTED_OS)..."
	@$(MKDIR) $(BUILD_DIR)
	@cd Uefi.Build && $(CC) -o $(ROOT_DIR)$(SEP)$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) main.c

# Build all libraries
.PHONY: build-libs
build-libs: build-system
	@echo "Building libraries..."
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Core
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.StdLib
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Graphics
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Input
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Crypto
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Acpi
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Ovmf
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Timer
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.FileSystem
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Network
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Uefi.Memory

# Build all examples
.PHONY: build-examples
build-examples: build-libs
	@echo "Building examples..."
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Examples$(SEP)HelloWorld
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Examples$(SEP)OvmfDetect
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Examples$(SEP)GraphicsInput
	@$(BUILD_DIR)$(SEP)ubuild$(if $(filter Windows,$(DETECTED_OS)),.exe,) build Examples$(SEP)CryptoDemo

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
ifeq ($(DETECTED_OS),Windows)
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@for /r . %%f in (*.o *.a *.efi) do @if exist "%%f" del /q "%%f"
else
	@$(RM) -r $(BUILD_DIR)
	@find . -name "*.o" -delete 2>/dev/null || true
	@find . -name "*.a" -delete 2>/dev/null || true
	@find . -name "*.efi" -delete 2>/dev/null || true
endif

# Install (copy to system location)
.PHONY: install
install: all
	@echo "Installing to system..."
ifeq ($(DETECTED_OS),Windows)
	@echo "Windows installation not implemented yet"
else
	@sudo $(MKDIR) /usr/local/lib/uefi
	@sudo $(MKDIR) /usr/local/include/uefi
	@sudo $(CP) $(LIB_DIR)/*.a /usr/local/lib/uefi/
	@sudo $(CP) -r Uefi.*$(SEP)include* /usr/local/include/uefi/
endif

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "Uninstalling..."
ifeq ($(DETECTED_OS),Windows)
	@echo "Windows uninstallation not implemented yet"
else
	@sudo $(RM) -r /usr/local/lib/uefi
	@sudo $(RM) -r /usr/local/include/uefi
endif

# Help target
.PHONY: help
help:
	@echo "ItsBasicUefi Build System"
	@echo "Detected OS: $(DETECTED_OS)"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build everything (default)"
	@echo "  build-system - Build the ubuild tool"
	@echo "  build-libs   - Build all libraries"
	@echo "  build-examples - Build all examples"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install to system (Linux/macOS only)"
	@echo "  uninstall    - Remove from system (Linux/macOS only)"
	@echo "  help         - Show this help"

# Platform-specific help
.PHONY: info
info:
	@echo "Build Information:"
	@echo "  OS: $(DETECTED_OS)"
	@echo "  CC: $(CC)"
	@echo "  AR: $(AR)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  LDFLAGS: $(LDFLAGS)"
