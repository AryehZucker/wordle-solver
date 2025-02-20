#include "feedback.hpp"

#include "tables.hpp"
#include "utils.hpp"

#include <bits/stdc++.h>

Feedback::Feedback(const char *guess, const char *ans)
{
    char letter;
    static struct DataL all_letter_data[26];
    int guess_count, ans_count;

    // initialize data to none
    for (int i = 0; i < 26; i++)
    {
        all_letter_data[i].amount = all_letter_data[i].known_pos = 0;
        all_letter_data[i].bad_pos = 0x1F;
    }

    for (int i = 0; i < WORDLEN; i++)
    {
        // find all "known_pos"
        if ((letter = guess[i]) == ans[i])
            all_letter_data[letter - 'a'].known_pos |= 1 << i; // turn on the i-th bit

        // find all "bad_pos"
        else
            all_letter_data[letter - 'a'].bad_pos &= ~(1 << i); // turn off the i-th bit
    }

    for (int i = 0; i < 26; i++)
    {
        // count how many letter "i"s are in guess and answer
        guess_count = ans_count = 0;
        for (int j = 0; j < WORDLEN; j++)
        {
            if (guess[j] == ('a' + i))
                guess_count++;
            if (ans[j] == ('a' + i))
                ans_count++;
        }

        // find "amount"
        all_letter_data[i].amount = MIN(guess_count, ans_count);
        // find "capped"
        if (guess_count > all_letter_data[i].amount)
        {
            if (ans_count == 0) // the letter is not in the ans
                bad_letters |= 1 << i;
            else
                all_letter_data[i].amount |= CAPPED;
        }
    }

    // load all non-empty data into "data"
    for (int i = 0; i < 26; i++)
        if (all_letter_data[i].amount != 0)
            letters |= 1 << i;

    letter_data = new DataL[size()];
    struct DataL *letter_data_ptr = letter_data;
    for (int i = 0; i < 26; i++)
        if (all_letter_data[i].amount != 0)
            *letter_data_ptr++ = all_letter_data[i];

    simplify();
}

Feedback::Feedback(const Feedback &f1, const Feedback &f2)
{
    unsigned int d1_letters, d2_letters, c_letters;
    // pointers to letter_data that we're up to
    struct DataL *combo_ldp;
    const struct DataL *d1_ldp, *d2_ldp;
    
    // combine good and bad letters
    letters = f1.letters | f2.letters;
    bad_letters = f1.bad_letters | f2.bad_letters;
    
    letter_data = new DataL[size()];

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

    simplify();
}

Feedback::~Feedback()
{
    delete[] letter_data;
}

int Feedback::size() const
{
    return weight(letters);
}

bool Feedback::fits(const DataA &ans_data) const
{
    // test that the letters don't contradict
    if (letters & ~ans_data.letters || bad_letters & ans_data.letters)
        return false;

    else
    {
        const struct DataL *gldp = letter_data; // pointer to the guess letter_data that we're upto
        const struct DataLA *aldp = ans_data.letter_data; // pointer to the answer letter_data that we're upto

        int guess_letters = letters,
            ans_letters = ans_data.letters;

        // loop through all the letters in guess data
        while (guess_letters)
        {
            if (guess_letters & 1)
            { // the bit for this letter is on, meaning there is data for this letter
                // check amount
                if (aldp->amount < (gldp->amount & ~CAPPED))
                    return false;

                // check capped
                if (gldp->amount & CAPPED && aldp->amount != (gldp->amount & ~CAPPED))
                    return false;

                // check known_pos
                if (~aldp->pos & gldp->known_pos)
                    return false;

                // check bad_pos
                if (aldp->pos & ~gldp->bad_pos)
                    return false;

                gldp++, aldp++;
            }
            else if (ans_letters & 1) // the bit for this letter is on
                aldp++;

            guess_letters >>= 1;
            ans_letters >>= 1;
        }
    }

    return true;
}

bool Feedback::operator==(const Feedback &other) const
{
    if (letters != other.letters ||
            bad_letters != other.bad_letters ||
            size() != other.size())
        return false;

    for (int i = 0; i < size(); i++)
        if (letter_data[i].amount != other.letter_data[i].amount ||
                letter_data[i].known_pos != other.letter_data[i].known_pos ||
                letter_data[i].bad_pos != other.letter_data[i].bad_pos)
            return false;

    return true;
}

Feedback Feedback::operator+(const Feedback &other) const
{
    return Feedback(*this, other);
}

void Feedback::simplify(void)
{
    const int data_len = size();
    int i, changed, total_amount, taken = 0;

    do
    {
        changed = 0;

        // count total known letters
        total_amount = 0;
        for (i = 0; i < data_len; i++)
            total_amount += letter_data[i].amount & ~CAPPED;

        for (i = 0; i < data_len; i++)
        {
            // capped:3 & known_pos:2
            if ((letter_data[i].amount & ~CAPPED) == (unsigned int)weight(letter_data[i].bad_pos))
            {
                if (!(letter_data[i].amount & CAPPED))
                {
                    letter_data[i].amount |= CAPPED;
                    changed = 1;
                }
                if (letter_data[i].known_pos != letter_data[i].bad_pos)
                {
                    letter_data[i].known_pos = letter_data[i].bad_pos;
                    changed = 1;
                }
            }
            // capped:2
            else if (total_amount == WORDLEN)
            {
                if (!(letter_data[i].amount & CAPPED))
                {
                    letter_data[i].amount |= CAPPED;
                    changed = 1;
                }
            }

            // bad_pos:2
            if (letter_data[i].amount == (weight(letter_data[i].known_pos) | CAPPED))
                if (letter_data[i].bad_pos != letter_data[i].known_pos)
                {
                    letter_data[i].bad_pos = letter_data[i].known_pos;
                    changed = 1;
                }

            // mark all taken positions
            taken |= letter_data[i].known_pos;
        }

        // bad_pos:3
        for (int i = 0; i < data_len; i++)
            if (letter_data[i].bad_pos & taken & ~letter_data[i].known_pos)
            {
                letter_data[i].bad_pos &= ~taken | letter_data[i].known_pos;
                changed = 1;
            }
    } while (changed);

    /*i choose not to draw inferences regarding the second instance of bad_pos:3
     *this will not affect whether data fits a word (and so is of no concern),
     *it will only affect considering data as equal
     */
}

std::size_t FeedbackHasher::operator()(const Feedback &f) const
{
    std::hash<int> int_hasher;
    std::hash<unsigned char> char_hasher;

    std::size_t hash = 17;
    hash = hash * 31 + int_hasher(f.letters);
    hash = hash * 31 + int_hasher(f.bad_letters);
    for (int i = 0; i < f.size(); i++)
    {
        hash = hash * 31 + char_hasher(f.letter_data[i].amount);
        hash = hash * 31 + char_hasher(f.letter_data[i].known_pos);
        hash = hash * 31 + char_hasher(f.letter_data[i].bad_pos);
    }
    return hash;
}
