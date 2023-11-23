# aht20 communication, no wiringPi

basic i2c communication with aht20 temp/humidity sensor

testing on Orange Pi Zero with Armbian 23.8.1 bullseye, kernel 5.15.93-sunxi

## how to compile:
```
g++ -o aht20 main.c -Wall -O
```

## sample output:

open chip at 0x38 succeeded, fd 3
sent reset command
sent control command
get status register: 0x18, 00011000
start measurment
measurment done! bytes read: 7
CRC check: true
byte [0] -> 0x1C, 00011100
byte [1] -> 0x3A, 00111010
byte [2] -> 0xD3, 11010011
byte [3] -> 0x36, 00110110
byte [4] -> 0x8C, 10001100
byte [5] -> 0x94, 10010100
byte [6] -> 0xD2, 11010010
humidity: 22.978497
temp: 31.864166
