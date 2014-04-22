#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Pro or Pro Mini (5V, 16 MHz) w/ ATmega328
#define __AVR_ATmega328P__
#define 
#define ARDUINO 101
#define ARDUINO_MAIN
#define __AVR__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

void setup_timer_interrupt();
void ramp_up(uint16_t rpc_from_us, uint16_t rpc_to_us, uint32_t ramp_up_time_us);
void ramp_down(uint16_t rpc_from_us, uint16_t rpc_to_us, uint32_t ramp_down_time_us);
void oscilliate(uint16_t amplitude, float frequency, uint32_t time_ms);
void sawtooth(uint16_t amplitude, float frequency, uint32_t time_ms);
void squareTorture(float frequency, uint32_t time_ms);
void toggle_led();
void run_test();
//
//

#include "C:\Development\ArduPilot-Arduino-1.0.3-windows\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Development\ArduPilot-Arduino-1.0.3-windows\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Development\RC_PWM_Generator\RC_PWM_Generator.ino"
#endif
