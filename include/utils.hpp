#ifndef UTILS_H
#define UTILS_H

#include <fstream>

struct Node;
struct DataA;
struct DataL;
struct DataS;

enum { INIT , SEARCH , COUNT };

struct prog {
	int answer;
	long double runs;
	double time_offset;
	long lookup_successes, total_lookups;
};

int weight(int bitstr);

void initLogging(struct prog p);
void clearLookups(void);
void timeStart(void);
void timeEnd(int mode);
void logLookup(int success);
void printLogging();

void showDataA(const char *word, const struct DataA *data);
void printData(int letters, const struct DataL *letter_data, int bad_letters, std::ostream &file);
void printDataS(const struct DataS *data, std::ostream &file);
void saveTree(struct Node *tree[]);
void printTree(struct Node *node, std::ostream &file);

#endif
