# Board/Vehicle

## Generating STM Cube IDE code
- Download the `.ioc` file from the `ioc` directory
- Create a new project from ioc in the IDE
- Select the ioc file, create in the IDE
- Save the ioc file and autogenerate code
- Copy the code in `src/main.c` into `Core/Src/main.c` in the IDE
- Copy the `.h` and `.c` files from `src` directory into `Core/Src/main.c`

## Pins
| Processor Pin | Type | Use | Alternate Functions |
| ------------- | ---- |---- | ------------------- |
| PB4 | PWM Input | Read IR values from transmitter | TIM2_CHAN2 |
| PA1 | PWM Output | Motor R | TIM2_CHAN2 |
| PA3 | PWM Output | Motor R | TIM2_CHAN4 |
| PA5 | PWM Output | Motor R | TIM2_CHAN1 |
| PB10 | PWM Output | Motor R | TIM2_CHAN3 |

## Functionality (High Level)
- Read in IR signal using timer alternate function