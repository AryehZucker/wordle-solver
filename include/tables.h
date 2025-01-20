#ifndef TABLES_H
#define TABLES_H

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

//letters + bad_letters + ceil(5/2) letters
#define HASH_LEN 5

#define CAPPED 010U
#define EMPTY (-1)

struct WordList;

//data on a letter
struct DataL {
	//amount includes flag for CAPPED
	unsigned char amount, known_pos, bad_pos;
};

//data on a letter for DataA
struct DataLA {
	//amount includes flag for CAPPED
	unsigned char amount, pos;
};

//data of an answer
struct DataA {
	int letters;
	struct DataLA letter_data[5];
};

//data on a single word
struct DataS {
	int letters, bad_letters;
	struct DataL letter_data[5];
};

//data on a 2 word combo
struct DataC {
	int letters, bad_letters;
	struct DataL letter_data[10];
};


struct Node {
	int elims;
	unsigned int hash[HASH_LEN];
	struct Node *left, *right;
};

void wordToData(const char *word, struct DataA *data);

void genDataTable(const char *ans, struct WordList guesses, struct DataS table[]);
void genData(const char *guess, const char *ans, struct DataS *data);

int getComboElims(struct DataS *data1, struct DataS *data2, struct Node *tree[]);
void combine(const struct DataS *data1, const struct DataS *data2, struct DataC *combo_data);
int *searchTree(const unsigned int *data_hash, const int len, struct Node *tree[]);
void freeTree(struct Node *tree[], int size);

void init_ans_data(void);
void free_ans_data(void);
int countElims(const struct DataC *data);
int fits(const struct DataC *data, const struct DataA *ans_data);

void simplify(struct DataL *letter_data, const int data_len);
void hashData(const struct DataC *data, unsigned int *buffer);

#endif
