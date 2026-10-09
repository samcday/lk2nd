# AGENTS.md

## Project Overview

lk2nd is a custom bootloader for Qualcomm SoCs, based on a fork of Qualcomm's Little Kernel (LK). It provides a unified fastboot interface, automatic device detection, and workarounds for device-specific issues. There are two configurations:

- **lk2nd** (secondary bootloader): Packed into an Android boot image, loaded by the stock bootloader. Does not replace the stock bootloader.
- **lk1st** (primary bootloader): Experimental. Replaces the stock bootloader entirely (intended for SBCs).

## Build Commands

```bash
# Build lk2nd for a specific SoC (replace msm8916 with your target)
make TOOLCHAIN_PREFIX=arm-none-eabi- lk2nd-msm8916

# Build lk1st
make TOOLCHAIN_PREFIX=arm-none-eabi- lk1st-msm8916

# Clean all build directories
make spotless
```

Output: `build-<project>/lk2nd.img` (or `lk.bin` for lk1st).

### Build Requirements

- ARM 32-bit GCC cross-toolchain (`arm-none-eabi-gcc`)
- Device Tree Compiler (`dtc`)
- libfdt development headers
- Python 3 (for signing)
- GNU tar

### Build Flags

- `DEBUG=0|1|2` — Log level (default: 1)
- `LK2ND_FORCE_FASTBOOT=1` — Always enter fastboot menu on boot
- `LK2ND_ADTBS=<dtb>` — Build only listed DTBs
- `SIGN_BOOTIMG=1` — Sign image for AVB1
- `TOOLCHAIN_PREFIX=<prefix>` — Cross-compiler prefix (default: `arm-none-eabi-`)

## Architecture

- `lk2nd/` — lk2nd-specific modules (fastboot, device detection, display, boot, etc.)
- `lk2nd/project/` — Build system configuration for lk2nd/lk1st
- `project/` — Per-SoC target definitions (e.g., `msm8916.mk`)
- `target/` — Target-specific code and rules
- `platform/` — Platform-specific drivers and initialization
- `arch/` — Architecture-specific code (arm, x86)
- `make/` — Build system (LK-style makefiles)
- `app/` — Applications (aboot)
- `dev/` — Device drivers
- `kernel/` — Kernel modules
- `lib/` — Libraries
- `include/` — Common headers

## Key Conventions

- **Indentation**: Tabs for C/makefiles, 4 spaces for Python (see `.editorconfig`)
- **No tests**: This is a bootloader; there is no test suite. Verification is done by building and flashing to hardware.
- **CI**: Drone CI (`.drone.yml`) builds for all supported SoCs with `-Werror` (via `LKLE_CFLAGS`). CI uses Alpine Linux.
- **local.mk**: Gitignored file for local build configuration (e.g., `PROJECT := lk2nd-msm8916`).
- **Device trees**: DTBs are compiled from `.dts` files in `lk2nd/device/dts/<target>/`. Each supported device has a `.dts` file.
- **512 KiB offset**: lk2nd reserves the first 512 KiB of the boot partition for itself. The real OS image is placed at this offset.

## Adding Device Support

1. Create a `.dts` file in `lk2nd/device/dts/<target>/` (e.g., `msm8916/my-device.dts`)
2. Add the device to `Documentation/devices.md`
3. Build and test on hardware

## Flashing

```bash
# Fastboot
fastboot flash boot lk2nd.img

# Samsung (Heimdall)
heimdall flash --BOOT lk2nd.img

# EDL mode
edl w boot lk2nd.img
```

## Documentation

- `Documentation/building.md` — Build instructions and requirements
- `Documentation/targets.md` — Supported SoCs and targets
- `Documentation/devices.md` — List of supported devices
- `Documentation/boot.md` — Boot process details
- `Documentation/fastboot.md` — Fastboot commands
