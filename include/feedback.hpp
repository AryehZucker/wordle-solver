#ifndef FEEDBACK_H
#define FEEDBACK_H

//letters + bad_letters + ceil(5/2) letters
#define HASH_LEN 5

//data on a letter
struct DataL {
	//amount includes flag for CAPPED
	unsigned char amount, known_pos, bad_pos;
};

//data on a single word
struct DataS {
	int letters, bad_letters;
	DataL letter_data[5];
};

//data on a 2 word combo
struct DataC {
	int letters, bad_letters;
	DataL letter_data[10];
};

class Feedback
{
public:
    unsigned int data_hash[HASH_LEN];
    DataC data;
    DataS dataS;
    static int compare(const Feedback &f1, const Feedback &f2);
    Feedback(void);
    Feedback(const DataS &data1, const DataS &data2);
    void combine(const DataS &data1, const DataS &data2);
    void generateHash(void);
};

#endif
