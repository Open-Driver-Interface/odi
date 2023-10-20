#ifndef _ODI_DEBUG_H
#define _ODI_DEBUG_H

#include "../config.h"

#define ODI_DTAG_ERROR "error"
#define ODI_DTAG_INFO "info"
#define ODI_DTAG_WARN "warn"
#define ODI_DTAG_DEBUG "debug"
#define ODI_DTAG_NONE "none"


#define ODI_DEBUG_MAX_TAGS 16

struct odi_debug_entry {
    char * message;
    struct odi_debug_entry * next;
};

struct odi_debug_queue {
    char * tag;
    struct odi_debug_entry * entries;
};

void odi_debug_append(const char* tag, char * message, ...);
void odi_debug_flusha();
void odi_debug_cleara();
void odi_debug_flush(const char * tag);
void odi_debug_clear(const char * tag);

#endif