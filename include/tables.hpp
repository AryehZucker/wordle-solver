#ifndef TABLES_H
#define TABLES_H

#include "dictionary.hpp"
#include "feedback.hpp"

#include <vector>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define CAPPED 010U
#define EMPTY (-1)

struct WordList;
struct Dict;

//data on a letter for DataA
struct DataLA {
	//amount includes flag for CAPPED
	unsigned char amount, pos;
};

//data of an answer
struct DataA {
	int letters;
	struct DataLA letter_data[WORDLEN];
};

void wordToData(const char *word, struct DataA *data);

void genDataTable(const char *ans, struct WordList guesses, std::vector<Feedback> &table);

int getElims(const Feedback &feedback);

void initAnsToDataTable(struct Dict words);
void delAnsToDataTable(void);
int countElims(const Feedback &feedback);
int fits(const Feedback &feedback, const struct DataA *ans_data);

void simplify(struct DataL *letter_data, const int data_len);

#endif
