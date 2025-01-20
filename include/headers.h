#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <time.h>




#define WORDLEN 5

#define CAPPED 010U

#define EMPTY (-1)

//letters + bad_letters + ceil(5/2) letters
#define HASH_LEN 5

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define TRUE 1
#define FALSE 0

enum { INIT , SEARCH , COUNT };

typedef struct{
	char *words;
	int len;
} WordList;

typedef struct{
	WordList answers;
	WordList guesses;
} Dict;

//data on a letter
typedef struct{
	//amount includes flag for CAPPED
	unsigned char amount, known_pos, bad_pos;
} DataL;

//data on a letter for DataA
typedef struct{
	//amount includes flag for CAPPED
	unsigned char amount, pos;
} DataLA;

//data of an answer
typedef struct{
	int letters;
	DataLA letter_data[5];
} DataA;

//data on a single word
typedef struct{
	int letters, bad_letters;
	DataL letter_data[5];
} DataS;

//data on a 2 word combo
typedef struct{
	int letters, bad_letters;
	DataL letter_data[10];
} DataC;


struct node{
	int elims;
	unsigned int hash[HASH_LEN];
	struct node *left, *right;
};
typedef struct node Node;


struct prog {
	int answer;
	long double runs;
	double time_offset;
	long lookup_successes, total_lookups;
};


void loadDict(const char *ans_words_path, const char *guess_words_path);
WordList loadWordList(const char *path);
char *getWord(int index, WordList wl);

void wordToData(const char *word, DataA *data);

void genDataTable(const char *ans, WordList guesses, DataS table[]);
void genData(const char *guess, const char *ans, DataS *data);

int getComboElims(DataS *data1, DataS *data2, Node *tree[]);
void combine(const DataS *data1, const DataS *data2, DataC *combo_data);
int *searchTree(const unsigned int *data_hash, const int len, Node *tree[]);
void freeTree(Node *tree[], int size);

void init_ans_data(void);
void free_ans_data(void);
int countElims(const DataC *data);
int fits(const DataC *data, const DataA *ans_data);

void simplify(DataL *letter_data, const int data_len);
void hashData(const DataC *data, unsigned int *buffer);

int weight(int bitstr);

void calcElims(double *total_elims);

void initLogging(struct prog p);
void clearLookups(void);
void timeStart(void);
void timeEnd(int mode);
void logLookup(int success);
void printLogging();

int setSaveFile(const char *path);
struct prog loadProgress(double *total_elims, Node *tree[]);
void saveProgress(int answer, double *total_elims, Node *tree[]);
void showDataA(const char *word, const DataA *data);
void printData(int letters, const DataL *letter_data, int bad_letters, FILE *fp);
void printDataS(const DataS *data, FILE *fp);
void saveTree(Node *tree[]);
void printTree(Node *node, FILE *fp);

void test();
void testSimplify(DataS *data);
void testElimsTable(void);
