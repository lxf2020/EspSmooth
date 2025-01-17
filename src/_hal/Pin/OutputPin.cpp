#include "OutputPin.h"
#include "HardwareSerial.h"

static const uint8_t __GPIO_RANGE[20]= {0,2,4,5,13,14,15,16,17,18,19,21,22,23,25,26,27,32,33};
// static const uint8_t __GPIO_RANGE[20];
OutputPin::OutputPin(const char* pin_description){
	from_string(pin_description);
	__is_started = false;   // to be a procted variable?
	return;
	// TODO:
	//check if the input pin_number is in the avaliable range.
	// for(int i=0; i<20; i++){
	// 	if(__GPIO_RANGE[i] == get_gpio_id()){
	// 		return;
	// 	}
	// }
	printf("[E][OutputPin] GPIO_ i% is NOT suitable for output.\n");
}

bool OutputPin::start()
{
	if(__is_started) return true;

    if(this->open_drain){                                   //need to check?
        pinMode(this->gpio_id_, OUTPUT_OPEN_DRAIN);   
    }else{
        pinMode(this->gpio_id_, OUTPUT);
    }

	__is_started = true;
	Serial.print("[D][OutputPin] start Open_drain= ");
	Serial.print(this->open_drain);
	Serial.print( ",  output_pin= GPIO_");
	Serial.println(this->get_gpio_id());
	return true;
}

//Set as inpput?
bool OutputPin::stop(){
	__is_started =  false;   //rename to "__is_working" ?
	Serial.print("[D][OutputPin] stop() output_pin= GPIO_");
	Serial.println(this->get_gpio_id());
	return true;
}

