
#ifndef __GPIO_LEDS_H_
#define __GPIO_LEDS_H_	1
int   gpio_port_configure_output(int port);
int   gpio_port_drive_high(int port);
int   gpio_port_drive_low(int port);
#endif
