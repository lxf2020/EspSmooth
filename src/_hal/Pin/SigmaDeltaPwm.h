#pragma once

#include "_hal/Pin/Pin.h"

class SigmaDeltaPwm : public Pin {
public:
    SigmaDeltaPwm();
    void on_tick(void);


    void     max_pwm(int);
    int      max_pwm(void);

    void     pwm(int);
    int      get_pwm() const { return _pwm; }
    void     set(bool);
    void    as_output(){}
    

private:
    int  _max;
    int  _pwm;
    int  _sd_accumulator;
    bool _sd_direction;
};