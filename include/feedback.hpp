#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <cstddef>

// data on a letter
struct DataL
{
    // amount includes flag for CAPPED
    unsigned char amount, known_pos, bad_pos;
};

struct Feedback
{
    int letters = 0, bad_letters = 0;
    DataL letter_data[10];
    bool operator==(const Feedback &other) const;
    Feedback operator+(const Feedback &other) const;
};

struct FeedbackHasher
{
    std::size_t operator()(const Feedback &f) const;
};

#endif
