#include "basic.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

//In a real driver you would implement these functions

void init_device(u32 ignored) {

}

void exit_device(u32 ignored) {

}

char * read_device(u32 ignored, u32 ignored2, u32 ignored3) {
    return 0x0;
}

void write_device(u32 ignored, u32 ignored2, u32 ignored3, char * ignored4) {
}

char * do_cool_things() {
    return "cool\n";
}

char * do_dumb_things() {
    return "hi mom\n";
}