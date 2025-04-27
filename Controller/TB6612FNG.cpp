#include "TB6612FNG.h"

TB6612FNG::TB6612FNG(){
}

TB6612FNG::TB6612FNG(PinName idir1, PinName idir2, PinName ipwm, PinName istby){
    config(idir1,idir2,ipwm,istby);
}
      
void TB6612FNG::config(PinName idir1, PinName idir2, PinName ipwm, PinName istby){
    dir1 = new DigitalOut(idir1);
    dir2 = new DigitalOut(idir2);
    pwm = new PwmOut(ipwm);
    stby = new DigitalOut(istby);
    stop();
}

void TB6612FNG::start(){
    on = true;
    *dir1 = 1;
    *dir2 = 1;
    *pwm = 0;
    *stby = 1;
}

void TB6612FNG::stop(){
    *dir1 = 0;
    *dir2 = 0;
    *pwm = 1;
}

void TB6612FNG::standby(){
    on = false;
    *dir1 = 0;
    *dir2 = 0;
    *pwm = 1;
    *stby = 0;
}

void TB6612FNG::fwd(float speed){
    if(on && speed <= 1){
        *dir1 = 1;
        *dir2 = 0;
        *pwm = speed;
    }
}

void TB6612FNG::rev(float speed){
    if(on && speed <= 1){
        *dir1 = 0;
        *dir2 = 1;
        *pwm = speed;
    }
}