#ifndef ICM20948_H
#define ICM20948_H

#include "mbed.h"

#ifndef ICM20948_I2C_ADDR
#define ICM20948_I2C_ADDR 0x69  // 7-bit address when AD0 is low
#endif

class ICM20948 {
public:
    // Constructor: takes a pointer to an I2C object and a 7-bit I2C address.
    ICM20948(I2C* i2c, int address)
        : _i2c(i2c), _address(address << 1) { }

    // initialize() resets and wakes the sensor.
    bool initialize() {
        // Reset the device by writing 0x80 to register 0x06 (PWR_MGMT_1)
        writeRegister(0x06, 0x80);
        wait_us(100000);  // Wait 100 ms for reset
        // Wake up device: set clock source (0x01)
        writeRegister(0x06, 0x01);
        wait_us(10000);   // Wait 10 ms
        return true;
    }

    // getAccel() reads 6 bytes starting at register 0x2D and converts them to g's.
    void getAccel(float* ax, float* ay, float* az) {
        uint8_t data[6];
        readRegisters(0x2D, data, 6);
        int16_t rawX = (int16_t)((data[0] << 8) | data[1]);
        int16_t rawY = (int16_t)((data[2] << 8) | data[3]);
        int16_t rawZ = (int16_t)((data[4] << 8) | data[5]);
        // Convert raw values to g's (assuming ±2g full-scale: 16384 LSB/g).
        *ax = rawX / 16384.0f;
        *ay = rawY / 16384.0f;
        *az = rawZ / 16384.0f;
    }

private:
    I2C* _i2c;
    int _address;  // 8-bit address (7-bit shifted left by 1)

    // Helper to read one register.
    uint8_t readRegister(uint8_t reg) {
        uint8_t value;
        _i2c->write(_address, (char*)&reg, 1, true);
        _i2c->read(_address, (char*)&value, 1);
        return value;
    }

    // Helper to read multiple registers.
    void readRegisters(uint8_t reg, uint8_t* data, int len) {
        _i2c->write(_address, (char*)&reg, 1, true);
        _i2c->read(_address, (char*)data, len);
    }

    // Helper to write one register.
    void writeRegister(uint8_t reg, uint8_t value) {
        char data[2];
        data[0] = reg;
        data[1] = value;
        _i2c->write(_address, data, 2);
    }
};

#endif // ICM20948_H