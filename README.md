# Velix

Velix is an STM32G4-based embedded hardware and firmware project. The current
firmware is generated from STM32CubeMX and built with CMake/Ninja for an
STM32G431RBT6 target.

The repository is organized as a combined hardware and software workspace. The
hardware design is stored under `hardware/`, while the STM32 firmware project is
stored under `software/`.

## Project Status

The project currently contains a CubeMX-generated firmware skeleton. Peripheral
initialization is in place, but the application layer is still mostly empty:
`main()` initializes the configured peripherals and then enters an empty main
loop.

Current firmware build output has been generated successfully in Debug mode:

```text
text    data   bss    dec    hex
35148   12     3084   38244  9564
```

## Directory Structure

```text
Velix/
├── hardware/
│   └── Velix_2026-06-28.epro2
├── software/
│   ├── Core/
│   │   ├── Inc/
│   │   └── Src/
│   ├── Drivers/
│   ├── Middlewares/
│   ├── cmake/
│   ├── CMakeLists.txt
│   ├── CMakePresets.json
│   ├── STM32G431XX_FLASH.ld
│   ├── Velix.ioc
│   └── startup_stm32g431xx.s
└── README.md
```

## Hardware

- MCU: STM32G431RBT6
- Package: LQFP64
- Clock source: external HSE, configured as 25 MHz in CubeMX
- System clock: 170 MHz PLL configuration
- Hardware design file: `hardware/Velix_2026-06-28.epro2`

The pinout suggests a motor-control-oriented board with display, encoder,
serial communication, CAN, and external peripheral interfaces.

## Firmware

The firmware is based on:

- STM32CubeMX 6.15.0
- STM32Cube FW_G4 V1.6.1
- STM32 HAL drivers
- CMake 3.22 or newer
- Ninja
- GNU Arm Embedded Toolchain, using the `arm-none-eabi-` prefix

### Configured Peripherals

- `TIM1`
  - Center-aligned three-phase complementary PWM
  - Channels 1, 2, 3 with complementary outputs
  - Channel 4 configured with no output, used as ADC injected trigger
  - Period: 4249
  - Dead time: 50

- `ADC1`
  - Regular channels: PA0/ADC1_IN1, PA1/ADC1_IN2, PA2/ADC1_IN3
  - Injected channels: ADC1_IN1, ADC1_IN2, ADC1_IN3
  - Injected trigger: `TIM1_CC4`

- `TIM2`
  - Basic timer
  - Prescaler: `170 - 1`
  - Period: `1000 - 1`

- `TIM3`
  - PWM output on channel 2

- `TIM4`
  - Encoder interface on PB6/PB7

- `SPI1`
  - Master mode
  - 16-bit data size
  - Approx. 21.25 Mbit/s
  - Pins: PA5/SCK, PA6/MISO, PB5/MOSI
  - Chip select label: `Encoder_CS`

- `SPI2`
  - Master mode
  - Approx. 21.25 Mbit/s
  - Pins: PB13/SCK, PB14/MISO, PB15/MOSI

- `USART1`
  - 3 Mbit/s
  - TX/RX with DMA
  - Pins: PA9/TX, PA10/RX

- `USART3`
  - 2 Mbit/s
  - TX/RX with DMA
  - Pins: PC10/TX, PC11/RX

- `FDCAN1`
  - Classic CAN frame format
  - Pins: PA11/RX, PA12/TX

- `I2C3`
  - Fast mode
  - Pins: PA8/SCL, PC9/SDA

- `CORDIC`
  - Enabled for math acceleration

### Named GPIO Signals

- `Encoder_CS`: PA4
- `ST7789_DC`: PC4
- `ST7789_RST`: PC5
- `ST7789_CS`: PC6
- `SP3485`: PB12
- `FRAM_WP`: PC8
- `LED_R`: PC12
- `LED_G`: PD2
- `LED_B`: PB3
- `key1`: PC13
- `key2`: PC14
- `encoder`: PB4

## Build

Make sure the GNU Arm toolchain is available in `PATH`:

```sh
arm-none-eabi-gcc --version
cmake --version
ninja --version
```

Configure and build the Debug target:

```sh
cd software
cmake --preset Debug
cmake --build --preset Debug
```

Configure and build the Release target:

```sh
cd software
cmake --preset Release
cmake --build --preset Release
```

Build artifacts are generated under:

```text
software/build/Debug/
software/build/Release/
```

The main firmware output is:

```text
Velix.elf
```

## Flashing

The repository does not currently define a flashing target. Typical options are:

```sh
STM32_Programmer_CLI -c port=SWD -w software/build/Debug/Velix.elf -v -rst
```

or using OpenOCD, STM32CubeProgrammer GUI, or an IDE/debugger configured for the
STM32G431RBT6 target.

## Development Notes

- Keep user code inside STM32CubeMX `USER CODE BEGIN` / `USER CODE END` blocks
  when editing generated files.
- Prefer adding project-specific drivers and application modules outside the
  generated CubeMX sections, then include them from `software/CMakeLists.txt`.
- Regenerate code from `software/Velix.ioc` when changing pinout, clock tree,
  DMA, interrupt, or peripheral configuration.
- `software/build/` and IDE metadata are ignored by git.
- The current root `.gitignore` ignores IDE folders, but `.DS_Store` files are
  still untracked if created by macOS.

## Suggested Next Steps

- Add application modules for motor control, encoder access, display output,
  serial protocol handling, and board bring-up tests.
- Add a post-build step to generate `.bin` and `.hex` files from `Velix.elf`.
- Add a flash/debug preset or helper script for the selected probe.
- Add a board bring-up checklist documenting power rails, SWD, clocks, LEDs,
  display, encoder, CAN/RS485, and PWM/ADC safety checks.
