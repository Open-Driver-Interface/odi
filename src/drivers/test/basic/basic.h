#ifndef _BASIC_H
#define _BASIC_H

//Real device driver, you can define it however you want
//A really really really important thing: You should cast
//The control buffer to the correct type and use it to interact
//With the device. 

//Just for portability reasons we include this header
#include "../../../core/types.h"


//Even this functios are optional.
void init_device(u32);
void exit_device(u32);

char * read_device(u32, u32, u32);
void write_device(u32, u32, u32, char *);

char * do_cool_things();
char * do_dumb_things();

#endif