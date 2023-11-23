#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <bitset>
#include <unistd.h>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

/*
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
*/

#define AHT2X_INIT_REG                    0xBE  //initialization register, for AHT2x only
#define AHTXX_STATUS_REG                  0x71  //read status byte register
#define AHTXX_START_MEASUREMENT_REG       0xAC  //start measurement register
#define AHTXX_SOFT_RESET_REG              0xBA  //soft reset register

/* calibration register controls */
#define AHT1X_INIT_CTRL_NORMAL_MODE       0x00  //normal mode on/off       bit[6:5], for AHT1x only
#define AHT1X_INIT_CTRL_CYCLE_MODE        0x20  //cycle mode on/off        bit[6:5], for AHT1x only
#define AHT1X_INIT_CTRL_CMD_MODE          0x40  //command mode  on/off     bit[6:5], for AHT1x only
#define AHTXX_INIT_CTRL_CAL_ON            0x08  //calibration coeff on/off bit[3]
#define AHTXX_INIT_CTRL_NOP               0x00  //NOP control, send after any "AHT1X_INIT_CTRL..."

/* status byte register controls */
#define AHTXX_STATUS_CTRL_BUSY            0x80  //busy                      bit[7]
#define AHT1X_STATUS_CTRL_NORMAL_MODE     0x00  //normal mode status        bit[6:5], for AHT1x only
#define AHT1X_STATUS_CTRL_CYCLE_MODE      0x20  //cycle mode status         bit[6:5], for AHT1x only
#define AHT1X_STATUS_CTRL_CMD_MODE        0x40  //command mode status       bit[6:5], for AHT1x only
#define AHTXX_STATUS_CTRL_CRC             0x10  //CRC8 status               bit[4], no info in datasheet
#define AHTXX_STATUS_CTRL_CAL_ON          0x08  //calibration coeff status  bit[3]
#define AHTXX_STATUS_CTRL_FIFO_ON         0x04  //FIFO on status            bit[2], no info in datasheet
#define AHTXX_STATUS_CTRL_FIFO_FULL       0x02  //FIFO full status          bit[1], no info in datasheet
#define AHTXX_STATUS_CTRL_FIFO_EMPTY      0x02  //FIFO empty status         bit[1], no info in datasheet

/* measurement register controls */
#define AHTXX_START_MEASUREMENT_CTRL      0x33  //measurement controls, suspect this is temperature & humidity DAC resolution
#define AHTXX_START_MEASUREMENT_CTRL_NOP  0x00  //NOP control, send after any "AHTXX_START_MEASUREMENT_CTRL..."

#define AHTXX_NO_ERROR           0x00    //success, no errors
#define AHTXX_BUSY_ERROR         0x01    //sensor is busy
#define AHTXX_ACK_ERROR          0x02    //sensor didn't return ACK (not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit))
#define AHTXX_DATA_ERROR         0x03    //received data smaller than expected
#define AHTXX_CRC8_ERROR         0x04    //computed CRC8 not match received CRC8, for AHT2x only
#define AHTXX_ERROR              0xFF    //other errors

using namespace std;

int aht20_reset(int fd) {

    uint8_t command[] = {AHTXX_SOFT_RESET_REG};

    cout << "sent reset command" << endl;
    
    return write(fd, command, 1);
}

int aht20_normalmode(int fd) {
    uint8_t command[] = {AHT2X_INIT_REG, AHTXX_INIT_CTRL_CAL_ON | AHT1X_INIT_CTRL_NORMAL_MODE, AHTXX_INIT_CTRL_NOP};

    cout << "sent control command" << endl;

    return write(fd, command, 3);
}

uint8_t aht20_getstatus (int fd) {
    
    uint8_t data[32];

    uint8_t command[] = {AHTXX_STATUS_REG};

    write(fd, command, 1);

    read(fd, data, 1);

    bitset<8> bits(data[0]);

    cout << "get status register: " << "0x" << hex << uppercase << (int)data[0] << nouppercase << dec << ", " << bits << endl;

    return data[0];
}

int main(void) {
    
    int fd = wiringPiI2CSetup(0x38);

    aht20_reset(fd);

    aht20_normalmode(fd);

    aht20_getstatus(fd);

    uint8_t command[] = {AHTXX_START_MEASUREMENT_REG, AHTXX_START_MEASUREMENT_CTRL, AHTXX_START_MEASUREMENT_CTRL_NOP};

    uint8_t buf_in[32];

    write(fd, command, 3);

    cout << "start measurment" << endl;

    if (1) {

        do {

            usleep(80 * 1000);

            read(fd, buf_in, 7);

        } while ((int)buf_in[0] != 0x1c);

        cout << "measurment complete" << endl;

        int result = read(fd, buf_in, 7);

        cout << "bytes read: " << result << endl;

        for (int i = 0; i < result; i++) {

            bitset<8> bits(buf_in[i]);

            cout << "data: " << "0x" << hex << uppercase << (int)buf_in[i] << nouppercase << dec << ", " << bits << endl;
        }

        unsigned long humidity   = buf_in[1];                          //20-bit raw humidity data
            humidity <<= 8;
            humidity  |= buf_in[2];
            humidity <<= 4;
            humidity  |= buf_in[3] >> 4;

        if (humidity > 0x100000) {humidity = 0x100000;}             //check if RH>100, no need to check for RH<0 since "humidity" is "uint"

        printf("humidity: %f\n", ((float)humidity / 0x100000) * 100);

            unsigned long temperature   = buf_in[3] & 0x0F;                //20-bit raw temperature data
                    temperature <<= 8;
                    temperature  |= buf_in[4];
                    temperature <<= 8;
                    temperature  |= buf_in[5];

        printf("temp: %f\n", ((float)temperature / 0x100000) * 200 - 50);
    } else {
        printf("reading not complete\n");
    }
}