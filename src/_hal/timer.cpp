#include "timer.h"
#include "esp32-hal-timer.h"



// Can one timer handles two types of ISR service?  seems LPC can do that. How about Esp32?
// Should we use hardware TMR3 for unstep?
#define STEP_TICKER_TIMER 1

hw_timer_t* hw_timer_stepTicker;  
// Setup where frequency is in Hz, delay is in microseconds
int stepTicker_setup(uint32_t frequency, uint32_t delay, void *mr0handler, void *mr1handler){
    uint32_t xx = frequency;
    hw_timer_stepTicker = timerBegin(STEP_TICKER_TIMER, 80, true);    // interrupto to be generated is edge(true)  or level(false).
    timerAttachInterrupt(hw_timer_stepTicker, (void(*)(void)) mr0handler, true);
    timerAlarmWrite(hw_timer_stepTicker, 1000000/xx, true);
    //timerAlarmWrite(hw_timer1, 100000, true);   
    timerAlarmEnable(hw_timer_stepTicker);
    return 0; 
}

// For unstep.  
void tmr1_mr1_start(){

}

void stepTicker_stop(){
    timerAlarmDisable(hw_timer_stepTicker);
}

void tmr2_stop(){
    
}
// frequency in HZ
int tmr2_setup(uint32_t frequency, void *timer_handler){
    return 0;
}
int tmr2_set_frequency(uint32_t frequency){
    return 0;
}