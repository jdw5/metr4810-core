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
| PB4 | GPIO Input | IR Receiver | - |
| PA1 | PWM Output | Motor Right Forward | TIM2_CHAN2 |
| PA3 | PWM Output | Motor Right Reverse | TIM2_CHAN4 |
| PA5 | PWM Output | Motor Left Forward | TIM2_CHAN1 |
| PB10 | PWM Output | Motor Left Reverse | TIM2_CHAN3 |

## Config
- 32kHZ clock
- 1 timer, 4 channels
- 1 GPIO OUT
- 1 GPIO IN
- 3V logic

## Command Values
| Button |  Decimal |  Hex   |
| ------ | -------- | ------ | 
|    A   | 16720605 | FF22DD |
|    B   | 16712445 | FF02FD |
|    C   | 16761405 | FFC23D |
|  FWD   | 16750695 | FF9867 |
|  BCK   | 16726215 | FF38C7 |
|  LFT   | 16724175 | FF30CF |
|  RGT   | 16743045 | FF7A85 |
|  PWR   | 16736925 | FF629D |
|  BTN   | 16718055 | FF18E7 |


