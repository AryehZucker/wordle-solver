#ifndef TABLES_H
#define TABLES_H

#include "dictionary.hpp"

#include <vector>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define CAPPED 010U
#define EMPTY (-1)

struct Feedback;

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

void genDataTable(const char *ans, const Dict &guesses, std::vector<Feedback> &table);

int getEliminations(const Feedback &feedback);

void initAnsToDataTable(const Dict &answers);
void delAnsToDataTable(void);
int countEliminations(const Feedback &feedback);

#endif
