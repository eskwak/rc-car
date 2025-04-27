#include "mbed.h"
#include <string>
using namespace std;

class Ultrasonic {
private:
    DigitalOut _trigger;
    DigitalIn _echo;
    string _UnidMeasure;
    int _sampling_time;
    int _tiempo_transcurrido_us;

    Timer pulse_width;
    int _distance_cm;
    Thread _sonar_thread;
    Mutex _mutex;

    void echo_loop();

public:
    Ultrasonic(PinName trigger, PinName echo, string UnidMeasure, int time);
    void Echolocation();
    int get_measurement();
    void print_measurement();
    void set_SamplingTime(int);

    void start_threaded_echo();
    int get_latest_distance();
};