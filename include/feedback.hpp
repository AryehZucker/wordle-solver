#ifndef FEEDBACK_H
#define FEEDBACK_H

// letters + bad_letters + ceil(5/2) letters
#define HASH_LEN 5

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
    unsigned int data_hash[HASH_LEN];
    static int compare(const Feedback &f1, const Feedback &f2);
    Feedback(void);
    Feedback(const Feedback &f1, const Feedback &f2);

private:
    void combine(const Feedback &f1, const Feedback &f2);
    void generateHash(void);
};

#endif
