# aht20 communication, no wiringPi

basic i2c communication with aht20 temp/humidity sensor

testing on Orange Pi Zero with Armbian 23.8.1 bullseye, kernel 5.15.93-sunxi

compile:
```
g++ -o aht20 main.c -Wall -O
```