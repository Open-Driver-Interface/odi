#include "debug.h"
#include "../dependencies.h"
#include "types.h"

#define ODI_DEBUG_MSG_LEN 4096

struct odi_debug_queue dqueue[ODI_DEBUG_MAX_TAGS] = {
    {
        .tag = ODI_DTAG_ERROR,
        .entries = 0x0
    },
    {
        .tag = ODI_DTAG_INFO,
        .entries = 0x0
    },
    {
        .tag = ODI_DTAG_WARN,
        .entries = 0x0
    },
    {
        .tag = ODI_DTAG_DEBUG,
        .entries = 0x0
    },
    [4 ... ODI_DEBUG_MAX_TAGS - 1] = {
        .tag = ODI_DTAG_NONE,
        .entries = 0x0
    }
};

struct odi_debug_queue* get_queue(const char* tag) {
    for (int i = 0; i < ODI_DEBUG_MAX_TAGS; i++) {
        if (!odi_dep_strcmp(dqueue[i].tag, tag)) {
            return &dqueue[i];
        }
    }
    return 0x0;
}

void odi_debug_append(const char* tag, char * message, ...) {
    //TODO: CHANGE THIS
    char * msg = odi_dep_malloc (ODI_DEBUG_MSG_LEN);
    va_list args;
    va_start(args, message);
    odi_dep_vsnprintf(msg, ODI_DEBUG_MSG_LEN, message, args);
    odi_dep_printf("[%s] %s\n", tag, msg);
    va_end(args);
    odi_dep_free(msg);
}


void odi_debug_flusha() {
    for (int i = 0; i < ODI_DEBUG_MAX_TAGS; i++) {
        odi_debug_flush(dqueue[i].tag);
    }
}
void odi_debug_cleara() {
    for (int i = 0; i < ODI_DEBUG_MAX_TAGS; i++) {
        odi_debug_clear(dqueue[i].tag);
    }
}

void odi_debug_flush(const char * tag) {
    (void)tag;
    //TODO: CHANGE THIS
}

//Delete all entries in the queue
void odi_debug_clear(const char * tag) {
    (void)tag;
    //TODO: CHANGE THIS
}