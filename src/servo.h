#ifndef _SERVO_H_
#define _SERVO_H_

#define PORT_1	PORTB
#define PIN_1	PB3
#define DDR_1	DDRB

#define PORT_2	PORTD
#define PIN_2	PD6
#define DDR_2	DDRD

void init_servo(void);
void servo1_set_percentage(signed char percentage);
void servo2_set_percentage(signed char percentage);
 
#endif /* _SERVO_H_ */
