#include "mbed.h"
#include "TB6612FNG.h"
#include <chrono>
#include <iostream>
#include "ultrasonic.h"

/*
    Variables required for car functioning.

    Motor A -> Right
    Motor B -> Left
*/
TB6612FNG motor_A(p5, p6, p21, p20);
TB6612FNG motor_B(p8, p7, p22, p20);
BufferedSerial bluetooth_car(p28, p27);
PwmOut led1(LED1);
PwmOut rightTurnSignal(p25);
PwmOut leftTurnSignal(p24);
Ultrasonic sonar(p11, p12, "cm", 10);

// Receives UART packets from arm side bluetooth module.
// Stores via reference.
bool receive_data(BufferedSerial&, float&, float&);

int main() {
    bluetooth_car.set_baud(9600);
    bluetooth_car.set_format(8, BufferedSerial::None, 1);
    motor_A.start();
    motor_B.start();
    led1 = 0.0f;
    sonar.start_threaded_echo();

    // Initialize crash distance to something for valid movement.
    // This will be changed almost immediately upon startup.
    int crash_distance = 15;

    // Variables to hold speeds.
    float left_speed = 0.0f;
    float right_speed = 0.0f;

    // Turn angle and speed variable changed by reference.
    float turn_angle;                                                                                                                                                                                                         
    float speed;

    while (1) {
        // Get the current crash distance.
        crash_distance = sonar.get_latest_distance();

        if (bluetooth_car.readable()) {
            if (receive_data(bluetooth_car, turn_angle, speed)) {
                if (speed > 0.0f && crash_distance <= 20) {
                    speed = 0.0f;
                }
               // Clamp turn angle.
                if (turn_angle < -2.0f) turn_angle = -2.0f;
                if (turn_angle > 2.0f) turn_angle = 2.0f;

                // Scale down turn angle.
                turn_angle /= 2.0f;

                // Right turn -> Turn angle < 0.
                // Left turn -> Turn angle > 0.
                if (turn_angle < 0.0f) {
                    left_speed = 1.0f + turn_angle;
                    right_speed = 1.0f;
                } 
                else if (turn_angle > 0.0f) {
                    left_speed = 1.0f;
                    right_speed = 1.0f - turn_angle;
                }
                else {
                    left_speed = 1.0f;
                    right_speed = 1.0f;
                }

                // Determine forward/backwards movement.
                // Negative speed indicates backwards movement.
                if (speed > 0.0f) {
                    if (crash_distance > 20) {
                        motor_A.fwd(right_speed * speed);
                        motor_B.fwd(left_speed * speed);
                    }
                } 
                else if (speed < 0.0f) {
                    motor_A.rev(right_speed * -speed);
                    motor_B.rev(left_speed * -speed);
                }
                else {
                    motor_A.fwd(0.0f);
                    motor_B.fwd(0.0f);
                }

                // Turn signal
                if (turn_angle > 0.3){
                    rightTurnSignal = 0.0f;
                    leftTurnSignal = turn_angle;
                }
                else if (turn_angle < -0.3){
                    rightTurnSignal = turn_angle * -1;
                    leftTurnSignal = 0.0f;
                }
                else {
                    rightTurnSignal = 0.0f;
                    leftTurnSignal = 0.0f;
                }
                
            }
        }

        wait_us(100000);
    }
}

bool receive_data(BufferedSerial& serial, float& turn_angle, float& speed) {
    uint8_t start;
    // Wait for start byte
    while (true) {
        if (serial.read(&start, 1) && start == 0xAA) {
            break;
        }
    }

    uint8_t turn_angle_buffer[sizeof(float)];
    uint8_t speed_buffer[sizeof(float)];
    size_t received = 0;

    while (received < sizeof(turn_angle_buffer)) {
        if (serial.read(&turn_angle_buffer[received], 1)) {
            received++;
        }
    }

    received = 0;
    while (received < sizeof(speed_buffer)) {
        if (serial.read(&speed_buffer[received], 1)) {
            received++;
        }
    }

    memcpy(&turn_angle, &turn_angle_buffer, sizeof(float));
    memcpy(&speed, &speed_buffer, sizeof(float));
    return true;
}

/*
bool receive_data(BufferedSerial& serial, float& turn_angle, float& speed) {
    uint8_t turn_angle_buffer[sizeof(float)];
    uint8_t speed_buffer[sizeof(float)];
    size_t received = 0;

    // Receieve turn angle data.
    while (received < sizeof(turn_angle_buffer)) {
        if (serial.read(&turn_angle_buffer[received], 1)) {
            received++;
        }
    }

    // Receive speed & direction data.
    received = 0;
    while (received < sizeof(speed_buffer)) {
        if (serial.read(&speed_buffer[received], 1)) {
            received++;
        }
    }

    memcpy(&turn_angle, &turn_angle_buffer, sizeof(float));
    memcpy(&speed, &speed_buffer, sizeof(float));
    
    return true;
}
*/