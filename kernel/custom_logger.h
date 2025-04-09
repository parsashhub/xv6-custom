#ifndef CUSTOM_LOGGER_H
#define CUSTOM_LOGGER_H

// Log levels
typedef enum {
  INFO = 0,
  WARN = 1,
  ERROR = 2
} LogLevel;

// Log message function
void log_message(int level, const char *message);

#endif // CUSTOM_LOGGER_H