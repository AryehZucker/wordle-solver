#include "feedback.hpp"

#include "dictionary.hpp"
#include "tables.hpp"
#include "utils.hpp"

int Feedback::compare(const Feedback &f1, const Feedback &f2)
{
    int i;
    for (i = 0; f1.data_hash[i] == f2.data_hash[i]; i++)
    {
        if (i == HASH_LEN - 1) // the entire hash has been compared and matches
            return 0;
    }
    return (f1.data_hash[i] < f2.data_hash[i]) ? -1 : 1;
}

Feedback::Feedback() {}

Feedback::Feedback(const DataS &data1, const DataS &data2)
{
    combine(data1, data2);
    generateHash();
}

void Feedback::combine(const DataS &data1, const DataS &data2)
{
    unsigned int d1_letters, d2_letters, c_letters;
    // pointers to letter_data that we're up to
    struct DataL *combo_ldp;
    const struct DataL *d1_ldp, *d2_ldp;

    // combine good and bad letters
    data.letters = data1.letters | data2.letters;
    data.bad_letters = data1.bad_letters | data2.bad_letters;

    // keep track of which good letters are in which data
    d1_letters = data1.letters & ~data2.letters; // letters only found in data1
    d2_letters = data2.letters & ~data1.letters; // letters only found in data2
    c_letters = data1.letters & data2.letters;   // letters found in both

    d1_ldp = data1.letter_data;
    d2_ldp = data2.letter_data;
    combo_ldp = data.letter_data;

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

    simplify(data.letter_data, weight(data.letters));
}

void Feedback::generateHash(void)
{
    int len = weight(data.letters);
    unsigned int tmp;

    // inefficient!!
    // initialize the hash to null data (0 is null because amount can't be 0)
    for (int i = 0; i < HASH_LEN; i++)
        data_hash[i] = 0;

    // which is better to go first? which is more commonly diff?
    int j = 0;
    data_hash[j++] = data.bad_letters;
    data_hash[j++] = data.letters;

    // multiple data can be combined into one int
    for (int i = 0; i < len; i++)
    {
        tmp = data.letter_data[i].amount << (WORDLEN * 2);
        tmp |= data.letter_data[i].known_pos << WORDLEN;
        tmp |= data.letter_data[i].bad_pos;

        if (i % 2 == 0)
        {
            data_hash[j] = tmp << 16;
        }
        else
        {
            data_hash[j++] |= tmp;
        }
    }
}
