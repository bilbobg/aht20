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
    get status register -> 0x18, 00011000
    start measurment
    measurment done! bytes read: 7
    byte [0] -> 0x1C, 00011100
    byte [1] -> 0x3B, 00111011
    byte [2] -> 0x6B, 01101011
    byte [3] -> 0xB6, 10110110
    byte [4] -> 0x88, 10001000
    byte [5] -> 0x5A, 01011010
    byte [6] -> 0xAA, 10101010
    CRC check -> true
    humidity: 23.211193
    temp: 31.657791
