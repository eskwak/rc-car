// TB6612FNG Motor Driver
#ifndef MBED_TB6612FNG_H
#define MBED_TB6612FNG_H

#include "mbed.h"

/** TB6612FNG control class.
 *
 * Example:
 * @code
 * //Perform readings on two i2c devices with the same address
 * #include "TB6612FNG.h"
 * #include "mbed.h"
 *
 *
 * int main() {
 *       
 *   while(1) {
 *   }
 * }
 * @endcode
 */

class TB6612FNG {
    private:
        bool on;
        
        DigitalOut *dir1;
        DigitalOut *dir2;
        PwmOut *pwm;
        DigitalOut *stby;
        
    public:
        /** Initialize object with default pins */
        TB6612FNG();
        
        /** Initialize object with specific motor pins
        * 
        * @param idir1 Direction pin (IN1)
        * @param idir2 Second direction pin (IN2)
        * @param ipwm Motor PWM pin (PWM)
        * @param istby SCL pin
        */
        TB6612FNG(PinName idir1, PinName idir2, PinName ipwm, PinName istby);
        
        /** Change the pins the motor was initialized with
        * 
        * @param idir1 Direction pin (IN1)
        * @param idir2 Second direction pin (IN2)
        * @param ipwm Motor PWM pin (PWM)
        * @param istby SCL pin
        */
        void config(PinName idir1, PinName idir2, PinName ipwm, PinName istby);
        
        /** Enables the motor
        */
        void start();
        
        /** Stops the motor from moving
        */
        void stop();
        
        /** Disables the motor, puts it into standby
        */
        void standby();
        
        /** Sets the motor to move forward at a given speed (as a percent [0,1])
        * @param speed Speed of the motor as a percent [0,1]
        */
        void fwd(float speed);
        
        /** Sets the motor to move backward at a given speed (as a percent [0,1])
        * @param speed Speed of the motor as a percent [0,1]
        */
        void rev(float speed);
        
};

#endif