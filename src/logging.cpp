#include "logging.hpp"

#include <iomanip>
#include <iostream>

#include "dictionary.hpp"

Logger::Logger(struct Dict &words)
{
    total_iterations = (long long)words.answers.len * words.guesses.len * (words.guesses.len - 1) / 2;

    std::cout << words.answers.len << " answers\t" << words.guesses.len << " guesses" << std::endl;
    std::cout << "Total iterations: " << total_iterations << std::endl;
}

void Logger::logCompletedIteration()
{
    completed_iterations++;
}

void Logger::displayProgress()
{
    if (difftime(time(NULL), last_print_time) < 1)
        return;

    last_print_time = time(NULL);

    std::cout << std::fixed << "\r"
              << completed_iterations << "/" << total_iterations << "\t"
              << std::setprecision(2) << "[" << 100.0 * completed_iterations / total_iterations << "% completed]\t"
              << std::flush;
}
