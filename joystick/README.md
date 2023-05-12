# Joystick Code

## Generating STM Cube IDE code
- Download the `.ioc` file from the `ioc` directory
- Create a new project from ioc in the IDE
- Select the ioc file, create
- Save the ioc file and autogenerate code
- Copy the code in `main.c` into `Core/Src/main.c` in the IDE


## NEC Codes
| Button | Decimal | Hex |
| ------ | ------- | --- | 
|    A   | 2155843950 |     |


## Custom NEC Codes
| Button | Decimal | Hex |
| ------ | ------- | --- | 
|  P+   | 146 | 0x000092 0x248000 |
|  Pp   | 530 | 0x000212 0x40000|
|  S/P   |  |     |
|  Music   |  |     |

### Joystick
| Button | Decimal | Hex | Binary |
| ------ | ------- | --- | ------ |
|  Full Forward | 491544 | 0x078018 | 0000 0111 1000 0000 0001 1000 |
|  Full Backward | 493600 | 0x078820| 0000 0111 1000 1000 0010 0000 |
|  Full Left | 7868440 | 0x781018 | 0111 1000 0001 0000 0001 1000 |
|  Full Right | 7864328 | 0x780008 | 011110000000000000001000 |
| Full TL | 8359968 | 0x7F9020 | 011111111001000000100000 |
| Full TR? | 3113024 | 0x2F8040 | 001011111000000001000000 |
| Full TR?? | 8257550 | 0x7E000E | 011111100000000000001110 |
| Full TR??? | 8355856 | 0x7F8010 | 011111111000000000010000 |

### Forward
| Decimal | Hex | Binary |
| ------- | --- | ------ |
| 491544 | 0x078018 | 000001111000000000011000 |
264220 | 0x04081C | 000001000000100000011100 |
67160 | 0x010658 | 000000010000011001011000 |
360470 | 0x058016 | 000001011000000000010110 |
100378 |  0x01881A | 000000011000100000011010 |
196628 | 0x030014 | 000000110000000000010100 |
 
## Trial 2
| Hex | Binary | Dir |
| --- | ------ | --- |
| 1E0064 | 0001 1110 0000 0000 0110 0100 | FWD |
| 1E2084 | 0001 1110 0010 0000 1000 0100 | BCK |
| 1E00026| 0001 1110 0000 0000 0000 0010 0110 | RGT |
| 1E04066| 0001 1110 0000 0100 0000 0110 0110 | LFT |

## Reversed MSB Code
| Decimal | Hex | Binary | Dir |
| ------- | --- | ------ | --- |
| 786672 | 0x0C00F0 | 0000 1100 0000 0000 1111 0000 | FWD |
| 133360 | 0x0208F0 | 0000 0010 0000 1000 1111 0000 | BCK |
| 787471 | 0x0C040F | 0000 1100 0000 0100 0000 1111 | LFT |
| 524303 | 0x08000F | 0000 1000 0000 0000 0000 1111 | RGT |





