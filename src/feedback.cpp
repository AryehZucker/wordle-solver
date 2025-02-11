#include "feedback.hpp"

#include "tables.hpp"
#include "utils.hpp"

#include <bits/stdc++.h>

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

Feedback::Feedback() {}

Feedback::Feedback(const Feedback &f1, const Feedback &f2)
{
    combine(f1, f2);
}

void Feedback::combine(const Feedback &f1, const Feedback &f2)
{
    unsigned int d1_letters, d2_letters, c_letters;
    // pointers to letter_data that we're up to
    struct DataL *combo_ldp;
    const struct DataL *d1_ldp, *d2_ldp;

    // combine good and bad letters
    letters = f1.letters | f2.letters;
    bad_letters = f1.bad_letters | f2.bad_letters;

    // keep track of which good letters are in which data
    d1_letters = f1.letters & ~f2.letters; // letters only found in f1
    d2_letters = f2.letters & ~f1.letters; // letters only found in f2
    c_letters = f1.letters & f2.letters;   // letters found in both

    d1_ldp = f1.letter_data;
    d2_ldp = f2.letter_data;
    combo_ldp = letter_data;

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

    simplify(letter_data, weight(letters));
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
