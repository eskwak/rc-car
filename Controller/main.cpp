#include "mbed.h"
#include "icm20948.h"
#include <cstdio>
#define ICM20948_I2C_ADDR 0x69

// Sends turn angle, speed, and direction over UART.
void send_data(BufferedSerial&, float, float);


/*
    Variables required for arm functioning.
*/
I2C i2c(p9, p10);
ICM20948 imu(&i2c, ICM20948_I2C_ADDR);
BufferedSerial bluetooth_arm(p28, p27);
DigitalIn reverse_button(p15);
DigitalIn accel_button(p16);
AnalogIn speed_selector(p20);

// On board LEDs used for testing.
PwmOut led1(LED1);
PwmOut led2(LED2);
PwmOut led3(LED3);


int main() {
    imu.initialize();
    bluetooth_arm.set_baud(9600);
    bluetooth_arm.set_format(8, BufferedSerial::None, 1);
    reverse_button.mode(PullDown);
    accel_button.mode(PullDown);
    
    led1 = 0.0f;
    led2 = 0.0f;
    led3 = 0.0f;

    // Direction of car movement (1.0 -> forward, -1.0 -> backwards).
    float direction = 1.0f;

    // Car speed.
    float speed = 0.0f;

    // Start timer for debouncing for reverse button.
    Timer reverse_button_timer;
    reverse_button_timer.start();

    while (true) {
        // Triggering reverse button if debouncing timer has reached threshold.
        if (reverse_button.read() && reverse_button_timer.elapsed_time() > 300ms) {
            if (direction == 1.0f) direction = -1.0f;
            else direction = 1.0f;
            
            // Reset the timer to allow debouncing.
            reverse_button_timer.reset();
        }

        // Read raw read speed value from pot.
        speed = speed_selector.read();

        float ax, ay, az;
        // Get imu readings.
        imu.getAccel(&ax, &ay, &az);

        // y reading determines turning direction.
        // flip polarity to align with car orientation.
        float accel_y = -ay;

        // When the accel button is not pressed, the car should not move.
        if (!accel_button.read()) {
            speed = 0.0f;
        }

        // Send data to car side bluetooth module.
        send_data(bluetooth_arm, accel_y, direction * speed);
        wait_us(100000);
    }
}

void send_data(BufferedSerial &serial, float accel, float direction_speed) {
    uint8_t packet[1 + 2 * sizeof(float)];
    packet[0] = 0xAA; // Start byte

    memcpy(&packet[1], &accel, sizeof(float));
    memcpy(&packet[1 + sizeof(float)], &direction_speed, sizeof(float));

    serial.write(packet, sizeof(packet));
}
/*
void send_data(BufferedSerial &serial, float accel, float direction_speed) {
    // Set up buffers.
    uint8_t buffer_accel[sizeof(float)];
    uint8_t buffer_direction_speed[sizeof(float)];

    // Copy data from inputs to buffers.
    memcpy(&buffer_accel, &accel, sizeof(float));
    memcpy(&buffer_direction_speed, &direction_speed, sizeof(float));

    // Write to car side bluetooth module.
    serial.write(buffer_accel, sizeof(buffer_accel));
    serial.write(buffer_direction_speed, sizeof(buffer_direction_speed));
}
*/