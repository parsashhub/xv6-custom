#include "types.h"
#include "riscv.h"
#include "custom_logger.h"
#include "defs.h"

void log_message(int level, const char *message) {
    // Check log level and print appropriate prefix
    if (level == INFO) {
        printf("INFO - %s\n", message);
    } else if (level == WARN) {
        printf("WARNING - %s\n", message);
    } else if (level == ERROR) {
        printf("ERROR - %s\n", message);
    } else {
        // Invalid log level
        printf("UNKNOWN - %s\n", message);
    }
}