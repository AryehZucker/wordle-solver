#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <cstddef>

struct DataA;

// data on a letter
struct DataL
{
    // amount includes flag for CAPPED
    unsigned char amount, known_pos, bad_pos;
};

struct Feedback
{
    int letters = 0, bad_letters = 0;
    DataL *letter_data;
    Feedback(const char *guess, const char *ans);
    Feedback(const Feedback &f1, const Feedback &f2);
    ~Feedback();
    int size(void) const;
    bool fits(const DataA &ans_data) const;
    bool operator==(const Feedback &other) const;
    Feedback operator+(const Feedback &other) const;
private:
    void simplify(void);
};

struct FeedbackHasher
{
    std::size_t operator()(const Feedback &f) const;
};

#endif
