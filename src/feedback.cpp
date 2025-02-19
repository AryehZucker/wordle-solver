#include "feedback.hpp"

#include "tables.hpp"
#include "utils.hpp"

#include <bits/stdc++.h>

Feedback::Feedback() : Feedback(10) {}

Feedback::Feedback(int size)
{
    letter_data = new DataL[size];
}

Feedback::~Feedback()
{
    delete[] letter_data;
}

bool Feedback::operator==(const Feedback &other) const
{
    if (letters != other.letters ||
            bad_letters != other.bad_letters ||
            weight(letters) != weight(other.letters))
        return false;

    for (int i = 0; i < weight(letters); i++)
        if (letter_data[i].amount != other.letter_data[i].amount ||
                letter_data[i].known_pos != other.letter_data[i].known_pos ||
                letter_data[i].bad_pos != other.letter_data[i].bad_pos)
            return false;

    return true;
}

Feedback Feedback::operator+(const Feedback &other) const
{
    Feedback combined_feedback;
    unsigned int d1_letters, d2_letters, c_letters;
    // pointers to letter_data that we're up to
    struct DataL *combo_ldp;
    const struct DataL *d1_ldp, *d2_ldp;

    // combine good and bad letters
    combined_feedback.letters = letters | other.letters;
    combined_feedback.bad_letters = bad_letters | other.bad_letters;

    // keep track of which good letters are in which data
    d1_letters = letters & ~other.letters; // letters only found in this
    d2_letters = other.letters & ~letters; // letters only found in other
    c_letters = letters & other.letters;   // letters found in both

    d1_ldp = letter_data;
    d2_ldp = other.letter_data;
    combo_ldp = combined_feedback.letter_data;

    // combine letter_data
    while (d1_letters || d2_letters || c_letters)
    {
        if (c_letters & 1)
        {
            // this automatically acounts for CAPPED
            combo_ldp->amount = MAX(d1_ldp->amount, d2_ldp->amount);
            combo_ldp->known_pos = d1_ldp->known_pos | d2_ldp->known_pos;
            combo_ldp->bad_pos = d1_ldp->bad_pos & d2_ldp->bad_pos;
            d1_ldp++, d2_ldp++, combo_ldp++;
        }
        else if (d1_letters & 1)
            *combo_ldp++ = *d1_ldp++;
        else if (d2_letters & 1)
            *combo_ldp++ = *d2_ldp++;

        d1_letters >>= 1;
        d2_letters >>= 1;
        c_letters >>= 1;
    }

    simplify(combined_feedback.letter_data, weight(combined_feedback.letters));

    return combined_feedback;
}

std::size_t FeedbackHasher::operator()(const Feedback &f) const
{
    std::hash<int> int_hasher;
    std::hash<unsigned char> char_hasher;

    std::size_t hash = 17;
    hash = hash * 31 + int_hasher(f.letters);
    hash = hash * 31 + int_hasher(f.bad_letters);
    for (int i = 0; i < weight(f.letters); i++)
    {
        hash = hash * 31 + char_hasher(f.letter_data[i].amount);
        hash = hash * 31 + char_hasher(f.letter_data[i].known_pos);
        hash = hash * 31 + char_hasher(f.letter_data[i].bad_pos);
    }
    return hash;
}
