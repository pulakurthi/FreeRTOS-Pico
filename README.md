# FreeRTOS on Raspberry Pi Pico 2 (RP2350)

Bare-minimum FreeRTOS project for the **Pico 2 (RP2350, Cortex-M33)** that blinks the onboard LED using the **Raspberry Pi fork** of the FreeRTOS Kernel.

## Prerequisites

- Raspberry Pi Pico SDK **2.2.0**
- [Raspberry Pi FreeRTOS Kernel fork](https://github.com/raspberrypi/FreeRTOS-Kernel)
- CMake ≥ 3.13, GCC ARM toolchain (`arm-none-eabi-gcc`)
- A Raspberry Pi Pico 2 board

---

## 1 — Clone the Pico SDK

```bash
cd ~
git clone -b 2.2.0 https://github.com/raspberrypi/pico-sdk.git pico-sdk-2.2.0
cd pico-sdk-2.2.0
git submodule update --init
export PICO_SDK_PATH=$HOME/pico-sdk-2.2.0
```

Make the variable permanent:

```bash
echo 'export PICO_SDK_PATH=$HOME/pico-sdk-2.2.0' >> ~/.bashrc
source ~/.bashrc
```

## 2 — Clone the FreeRTOS Kernel

> **Note:** This project uses the **Raspberry Pi fork** of the FreeRTOS Kernel
> (`raspberrypi/FreeRTOS-Kernel`) instead of the upstream
> `FreeRTOS/FreeRTOS-Kernel`. The fork includes first-party RP2040/RP2350
> portable layers under `portable/ThirdParty/GCC/` and adds Pico SDK interop
> support (`configSUPPORT_PICO_SYNC_INTEROP`, `configSUPPORT_PICO_TIME_INTEROP`).

```bash
cd ~
git clone --recurse-submodules https://github.com/raspberrypi/FreeRTOS-Kernel.git
export FREERTOS_KERNEL_PATH=$HOME/FreeRTOS-Kernel
```

Make the variable permanent:

```bash
echo 'export FREERTOS_KERNEL_PATH=$HOME/FreeRTOS-Kernel' >> ~/.bashrc
source ~/.bashrc
```

## 3 — Create Your Own Project (this repo)

Copy two helper CMake files from the FreeRTOS Kernel portable directory into your project root:

```
FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2350_ARM_NTZ/
├── pico_sdk_import.cmake        → <your-project>/pico_sdk_import.cmake
└── FreeRTOS_Kernel_import.cmake → <your-project>/FreeRTOS_Kernel_import.cmake
```

These scripts locate the SDK and FreeRTOS Kernel via the environment variables set above (`PICO_SDK_PATH`, `FREERTOS_KERNEL_PATH`). They must be `include()`-ed **before** the `project()` call in CMake.

## 4 — Build

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

The resulting `FreeRTOS-Pico.uf2` file will be in `build/`. Hold the BOOTSEL button on the Pico 2, plug it in via USB, then drag-and-drop the `.uf2` file onto the mounted drive.

---

## Project Structure

```
FreeRTOS-Pico/
├── CMakeLists.txt                  # Build configuration
├── pico_sdk_import.cmake           # Locates the Pico SDK (copied from SDK)
├── FreeRTOS_Kernel_import.cmake    # Locates the FreeRTOS Kernel (copied from Kernel)
├── lib/
│   ├── include/
│   │   └── FreeRTOSConfig.h        # FreeRTOS tuning & feature flags
│   └── src/
│       └── memory.c                # Static memory callbacks & stack overflow hook
└── src/
    └── main.c                      # Application entry point
```

---

## File Explanations

### `CMakeLists.txt`

The CMake build script. Key sections:

| Lines | Purpose |
|-------|---------|
| `cmake_minimum_required(VERSION 3.13)` | Minimum CMake version required by the Pico SDK. |
| `set(PICO_BOARD pico2 ...)` | Targets the **Pico 2** (RP2350). Change to `pico` for the original RP2040. |
| `include(pico_sdk_import.cmake)` | Pulls in the Pico SDK using `PICO_SDK_PATH`. Must come **before** `project()`. |
| `include(FreeRTOS_Kernel_import.cmake)` | Pulls in the FreeRTOS Kernel using `FREERTOS_KERNEL_PATH`. Must also come **before** `project()`. |
| `project(FreeRTOS-Pico C CXX ASM)` | Declares the project and the languages used. |
| `pico_sdk_init()` | Initializes the Pico SDK build system. |
| `add_executable(...)` | Declares the firmware binary and its source files (`lib/src/memory.c`, `src/main.c`). |
| `target_include_directories(... lib/include)` | Tells the compiler where to find `FreeRTOSConfig.h`. |
| `target_link_libraries(...)` | Links **pico_stdlib** (GPIO, UART, clocks), **FreeRTOS-Kernel** (scheduler, tasks, queues), and **FreeRTOS-Kernel-Heap4** (dynamic memory allocator using `heap_4.c`). |
| `pico_enable_stdio_usb(... 1)` | Enables USB-based standard I/O (`printf` output over USB serial). |
| `pico_enable_stdio_uart(... 0)` | Disables UART-based standard I/O. |
| `pico_add_extra_outputs(...)` | Generates additional output formats including the `.uf2` file used for drag-and-drop flashing. |

### `FreeRTOSConfig.h` (`lib/include/`)

This header configures every aspect of the FreeRTOS kernel. It is **required** — the kernel will not compile without it. Below are the key groups of settings:

#### Clock & Tick

| Macro | Value | Meaning |
|-------|-------|---------|
| `configCPU_CLOCK_HZ` | 150 000 000 | RP2350 default clock speed (150 MHz). Set only when `PICO_RP2350` is defined. |
| `configTICK_RATE_HZ` | 1000 | Kernel tick every 1 ms — sets the resolution for `vTaskDelay()` and time slicing. |
| `configUSE_TICKLESS_IDLE` | 1 | Enables tickless idle mode — the tick interrupt is suppressed while idle to save power. |

#### Scheduling

| Macro | Value | Meaning |
|-------|-------|---------|
| `configUSE_PREEMPTION` | 1 | Higher-priority tasks preempt lower-priority ones immediately. |
| `configUSE_TIME_SLICING` | 1 | Equal-priority tasks share CPU time in round-robin fashion. |
| `configMAX_PRIORITIES` | 32 | Priority levels 0 (idle) through 31. |
| `configMINIMAL_STACK_SIZE` | 256 | Minimum stack size (in words) allocated to each task. 256 × 4 = 1024 bytes on a 32-bit MCU. |

#### Synchronization

| Macro | Value | Meaning |
|-------|-------|---------|
| `configUSE_MUTEXES` | 1 | Enables mutex support. |
| `configUSE_RECURSIVE_MUTEXES` | 1 | Enables recursive mutex support. |
| `configUSE_COUNTING_SEMAPHORES` | 1 | Enables counting semaphores. |
| `configUSE_QUEUE_SETS` | 1 | Enables queue sets (block on multiple queues). |

#### Memory

| Macro | Value | Meaning |
|-------|-------|---------|
| `configSUPPORT_STATIC_ALLOCATION` | 1 | Allows creating tasks/queues with statically allocated memory. |
| `configSUPPORT_DYNAMIC_ALLOCATION` | 1 | Allows `pvPortMalloc()` / `vPortFree()` (uses Heap4). |
| `configTOTAL_HEAP_SIZE` | 128 KB | Total RAM available to the FreeRTOS heap allocator. |

#### Software Timers

Enabled (`configUSE_TIMERS = 1`). The timer service task runs at the highest priority (`configMAX_PRIORITIES - 1`) with a stack depth of 1024 words and a queue depth of 10.

#### Hook Functions

| Macro | Value | Meaning |
|-------|-------|---------|
| `configCHECK_FOR_STACK_OVERFLOW` | 1 | Enables method-1 (high water mark) stack overflow detection. Calls `vApplicationStackOverflowHook()` on overflow. |
| `configUSE_IDLE_HOOK` / `configUSE_TICK_HOOK` | 0 | No idle or tick hooks — avoids the need to define empty callback stubs. |

#### Debug Assert

`configASSERT(x)` uses the standard C `assert()` macro (via `#include <assert.h>`) to trap configuration errors during development.

#### Pico SDK Interop

| Macro | Value | Meaning |
|-------|-------|---------|
| `configSUPPORT_PICO_SYNC_INTEROP` | 1 | Enables interop between FreeRTOS and the Pico SDK's synchronization primitives (`mutex`, `critical_section`, etc.). |
| `configSUPPORT_PICO_TIME_INTEROP` | 1 | Enables interop between FreeRTOS and the Pico SDK's `sleep_until()` / `sleep_ms()` time functions. |

#### Cortex-M33 / RP2350 Specifics (conditional on `PICO_RP2350`)

| Macro | Value | Meaning |
|-------|-------|---------|
| `configMAX_SYSCALL_INTERRUPT_PRIORITY` | 16 (0x10) | ISRs with priority ≥ 0x10 may call `...FromISR()` FreeRTOS APIs. |
| `configENABLE_FPU` | 1 | Enables FPU context save/restore on task switches (Cortex-M33 has an FPU). |
| `configENABLE_TRUSTZONE` | 0 | TrustZone not used. |
| `configRUN_FREERTOS_SECURE_ONLY` | 1 | Runs FreeRTOS in secure mode. Must be set to 1 on RP2350, otherwise a silent hard fault occurs. |
| `configENABLE_MPU` | 0 | Memory Protection Unit not used. |

#### SMP (Symmetric Multiprocessing)

Configured for **single-core** operation (`configNUMBER_OF_CORES = 1`). The RP2350 has two cores — to use both, change it to 2.

### `lib/src/memory.c`

Provides the static memory allocation callbacks and the stack overflow hook required by FreeRTOS:

| Function | Purpose |
|----------|---------|
| `vApplicationGetIdleTaskMemory()` | Supplies a statically allocated TCB and stack for the idle task. Required when `configSUPPORT_STATIC_ALLOCATION = 1`. |
| `vApplicationGetTimerTaskMemory()` | Supplies a statically allocated TCB and stack for the timer service task. Required when both `configSUPPORT_STATIC_ALLOCATION` and `configUSE_TIMERS` are 1. |
| `vApplicationStackOverflowHook()` | Called on stack overflow — prints the offending task name, then halts the CPU. |

### `main.c`

The application firmware. Blinks the onboard LED using a FreeRTOS task:

| Section | What it does |
|---------|--------------|
| `pico_led_init()` | Initializes `PICO_DEFAULT_LED_PIN` as a GPIO output. |
| `pico_set_led(bool)` | Drives the LED pin high or low. |
| `vBlinkTask()` | A FreeRTOS task that toggles the LED on and off with `vTaskDelay()` and prints the state over USB serial via `printf`. |
| `main()` | Initializes USB stdio and the LED, creates the blink task with `xTaskCreate()`, and starts the FreeRTOS scheduler with `vTaskStartScheduler()`. |


