#ifndef LOGGING_H
#define LOGGING_H

#include <ctime>

class Logger {
private:
    long long completed_iterations = 0, total_iterations;
    time_t last_print_time = 0;
public:
    Logger(struct Dict &words);
    void logCompletedIteration(void);
    void displayProgress(void);
};

#endif
