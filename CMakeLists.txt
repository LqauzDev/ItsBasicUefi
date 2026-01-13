cmake_minimum_required(VERSION 3.10)
project(ItsBasicUefi VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for UEFI
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ffreestanding -fno-builtin -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -mno-red-zone -mcmodel=large")

# Linker flags for UEFI applications
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -Wl,--dll")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,efi_application")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--entry=UefiMain")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Uefi.Core/include)
include_directories(${CMAKE_SOURCE_DIR}/Uefi.StdLib/include)

# Core library
add_library(Uefi.Core STATIC
    Uefi.Core/src/uefi_lib.c
)
target_include_directories(Uefi.Core PUBLIC Uefi.Core/include)

# Standard library
add_library(Uefi.StdLib STATIC
    Uefi.StdLib/src/stdio.c
    Uefi.StdLib/src/stdlib.c
    Uefi.StdLib/src/string.c
)
target_include_directories(Uefi.StdLib PUBLIC 
    Uefi.StdLib/include 
    Uefi.Core/include
)
target_link_libraries(Uefi.StdLib Uefi.Core)

# Graphics library
add_library(Uefi.Graphics STATIC
    Uefi.Graphics/src/graphics.c
)
target_include_directories(Uefi.Graphics PUBLIC 
    Uefi.Graphics/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Graphics Uefi.Core Uefi.StdLib)

# Input library
add_library(Uefi.Input STATIC
    Uefi.Input/src/input.c
)
target_include_directories(Uefi.Input PUBLIC 
    Uefi.Input/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Input Uefi.Core Uefi.StdLib)

# Crypto library
add_library(Uefi.Crypto STATIC
    Uefi.Crypto/src/crypto.c
)
target_include_directories(Uefi.Crypto PUBLIC 
    Uefi.Crypto/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Crypto Uefi.Core Uefi.StdLib)

# ACPI library
add_library(Uefi.Acpi STATIC
    Uefi.Acpi/src/acpi.c
)
target_include_directories(Uefi.Acpi PUBLIC 
    Uefi.Acpi/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Acpi Uefi.Core Uefi.StdLib)

# OVMF library
add_library(Uefi.Ovmf STATIC
    Uefi.Ovmf/src/ovmf.c
)
target_include_directories(Uefi.Ovmf PUBLIC 
    Uefi.Ovmf/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Ovmf Uefi.Core Uefi.StdLib)

# Timer library
add_library(Uefi.Timer STATIC
    Uefi.Timer/src/timer.c
)
target_include_directories(Uefi.Timer PUBLIC 
    Uefi.Timer/include 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(Uefi.Timer Uefi.Core Uefi.StdLib)

# Examples
add_executable(HelloWorld.efi Examples/HelloWorld/main.c)
target_include_directories(HelloWorld.efi PRIVATE 
    Uefi.Core/include 
    Uefi.StdLib/include
)
target_link_libraries(HelloWorld.efi Uefi.Core Uefi.StdLib)

add_executable(OvmfDetect.efi Examples/OvmfDetect/main.c)
target_include_directories(OvmfDetect.efi PRIVATE 
    Uefi.Core/include 
    Uefi.StdLib/include
    Uefi.Ovmf/include
    Uefi.Timer/include
)
target_link_libraries(OvmfDetect.efi Uefi.Core Uefi.StdLib Uefi.Ovmf Uefi.Timer)

add_executable(GraphicsInput.efi Examples/GraphicsInput/main.c)
target_include_directories(GraphicsInput.efi PRIVATE 
    Uefi.Core/include 
    Uefi.StdLib/include
    Uefi.Graphics/include
    Uefi.Input/include
    Uefi.Timer/include
)
target_link_libraries(GraphicsInput.efi Uefi.Core Uefi.StdLib Uefi.Graphics Uefi.Input Uefi.Timer)

add_executable(CryptoDemo.efi Examples/CryptoDemo/main.c)
target_include_directories(CryptoDemo.efi PRIVATE 
    Uefi.Core/include 
    Uefi.StdLib/include
    Uefi.Crypto/include
)
target_link_libraries(CryptoDemo.efi Uefi.Core Uefi.StdLib Uefi.Crypto)

# Custom target for building all libraries
add_custom_target(libs
    DEPENDS Uefi.Core Uefi.StdLib Uefi.Graphics Uefi.Input Uefi.Crypto Uefi.Acpi Uefi.Ovmf Uefi.Timer
)

# Custom target for building all examples
add_custom_target(examples
    DEPENDS HelloWorld.efi OvmfDetect.efi GraphicsInput.efi CryptoDemo.efi
)

# Custom target for building everything
add_custom_target(all
    DEPENDS libs examples
)

# Install targets
install(TARGETS Uefi.Core Uefi.StdLib Uefi.Graphics Uefi.Input Uefi.Crypto Uefi.Acpi Uefi.Ovmf Uefi.Timer
    ARCHIVE DESTINATION lib/uefi
)

install(DIRECTORY Uefi.Core/include/ Uefi.StdLib/include/ Uefi.Graphics/include/ 
                Uefi.Input/include/ Uefi.Crypto/include/ Uefi.Acpi/include/ 
                Uefi.Ovmf/include/ Uefi.Timer/include/
    DESTINATION include/uefi
)
