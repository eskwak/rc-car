#include "mbed.h"
#include "ultrasonic.h"
#include <chrono>
#include <ratio>

Ultrasonic::Ultrasonic(PinName trigger,PinName echo,string UnidMeasure,int time): _echo(echo),_trigger(trigger){
    _UnidMeasure = UnidMeasure;
    _sampling_time = time;
}

void Ultrasonic::Echolocation (void){
    
    pulse_width.reset();
    
    _trigger = 1;
    wait_us(10);
    _trigger = 0;

    while (_echo == 0);

    pulse_width.start();
    while (_echo == 1);
    pulse_width.stop();

    _tiempo_transcurrido_us = pulse_width.elapsed_time().count();
    _tiempo_transcurrido_us /= 58;

    ThisThread::sleep_for(std::chrono::milliseconds(_sampling_time));
}

int Ultrasonic::get_measurement(void){
    Ultrasonic::Echolocation ();
    return _tiempo_transcurrido_us;
}

void Ultrasonic::set_SamplingTime(int time){
    _sampling_time = time;
}

void Ultrasonic::print_measurement (void){

    Ultrasonic::Echolocation ();

    if (_UnidMeasure == "cm") {
        printf("la distancia es = %d cm\n",_tiempo_transcurrido_us);
    }
    if (_UnidMeasure == "m") {
        if (_tiempo_transcurrido_us < 100) {
            printf("la distancia es = %d cm\n",_tiempo_transcurrido_us);
        }
        if (_tiempo_transcurrido_us >= 100) {
            printf("la distancia es = %d m\n",_tiempo_transcurrido_us/100);
        }
    }
}

void Ultrasonic::start_threaded_echo() {
    _sonar_thread.start(callback(this, &Ultrasonic::echo_loop));
}

void Ultrasonic::echo_loop() {
    while (true) {
        Echolocation();
        _mutex.lock();
        _distance_cm = _tiempo_transcurrido_us;
        _mutex.unlock();
    }
}

int Ultrasonic::get_latest_distance() {
    _mutex.lock();
    int val = _distance_cm;
    _mutex.unlock();
    return val;
}