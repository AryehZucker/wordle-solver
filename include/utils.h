#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

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

int setSaveFile(const char *path);
struct prog loadProgress(double *total_elims, struct Node *tree[]);
void saveProgress(int answer, double *total_elims, struct Node *tree[]);
void showDataA(const char *word, const struct DataA *data);
void printData(int letters, const struct DataL *letter_data, int bad_letters, FILE *fp);
void printDataS(const struct DataS *data, FILE *fp);
void saveTree(struct Node *tree[]);
void printTree(struct Node *node, FILE *fp);

#endif
