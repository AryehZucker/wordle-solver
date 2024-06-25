#include "headers.h"



extern Dict words;

//logging
struct stat {
	struct { long successes, total; } lookups;
	struct { time_t start, last_update, t1, init, search, count; } times;
	long double runs, total_runs;
} logging;

void initLogging(){
	logging.total_runs = (long double) words.answers.len * words.guesses.len * (words.guesses.len-1) / 2;
	logging.runs = 0;
	logging.lookups.successes = logging.lookups.total = 0;
	logging.times.init = logging.times.search = logging.times.count = 0;
	logging.times.start = logging.times.last_update = time(NULL);

	printf("%d answers\t%d guesses\n", words.answers.len, words.guesses.len);
	printf("Total runs: %.0Lf\n", logging.total_runs);
}

void clearLoggingLookups(){
	logging.lookups.successes = logging.lookups.total = 0;
}

void timeStart(){
	logging.runs++;
	logging.times.t1 = time(NULL);
}

void timeEnd(int mode){
	time_t now = time(NULL);
	time_t t = now - logging.times.t1;
	logging.times.t1 = now;
	switch(mode){
		case INIT:
			logging.times.init += t;
			break;
		case SEARCH:
			logging.times.search += t;
			break;
		case COUNT:
			logging.times.count += t;
			break;
	}
}

void logLookup(int success){
	logging.lookups.total++;
	if(success) logging.lookups.successes++;
}


void printLogging(){
	long double runs = logging.runs, rcompleted;
	time_t now = time(NULL);
	double total_time = difftime(now,logging.times.start);

	if(difftime(now, logging.times.last_update) < 1) return;

	logging.times.last_update = now;
	rcompleted = logging.runs/logging.total_runs; //ratio of completed to total
	putchar('\r');
	printf("total: %.4Lfms\t\t", 1000*(now-logging.times.start)/runs);
	printf("init: %.4Lfms|srch: %.4Lfms|cnt: %.4Lfms\t",
		1000*logging.times.init/runs, 1000*logging.times.search/runs, 1000*logging.times.count/runs);
	printf("%.2f%% suc\t", 100.0*logging.lookups.successes/logging.lookups.total);
	printf("[%.2Lf%% in %dm%ds; ", 100*rcompleted, (int)total_time/60, (int)total_time%60);
	printf("left: %.1Lf mins]\t", total_time*(1/rcompleted-1)/60);
	fflush(stdout);
}




char save_path[500] = "data/save.dat";
int setSaveFile(const char *path){
	if(sizeof save_path < strlen(path))
		return -1;
	
	strncpy(save_path, path, sizeof save_path);
	
	return 0;
}

struct prog loadProgress(double *total_elims, Node *tree[]){
	struct prog p;
	p.answer = 0;
	p.guess1 = 0;
	return p;
}

void saveProgress(int answer, int guess1, double* total_elims, Node *tree[]){
	
}




int saveData(double *data, int len){
	FILE *fp;

	if((fp = fopen(SAVE_PATH, "wb")) == NULL){
		char message[100];
		sprintf(message, "Error opening %s", SAVE_PATH);
		perror(message);
		return -1;
	}

	if(fwrite(data, sizeof (double), len, fp) != len){
		char message[100];
		sprintf(message, "Error writing to file %s", SAVE_PATH);
		perror(message);
		return -1;
	}

	fclose(fp);
	return 0;
}




void showDataA(const char *word, const DataA *data){
	int bits, letters = data->letters;
	const DataLA *letter_data = data->letter_data;

	printf("%s\n", word);

	for(char c='a'; letters; c++, letters>>=1){
		if(letters & 1){
			printf("  %c - %d ", c, (int) letter_data->amount);
			bits = letter_data->pos;
			for(int j=0; j<5; j++)
				printf("%d", (bits&(1<<j))>>j);
			printf("\n");

			letter_data++;
		}
	}
}

void printDataS(const DataS *data, FILE *fp){
	printData(data->letters, data->letter_data, data->bad_letters, fp);
}

void printData(int letters, const DataL *letter_data, int bad_letters, FILE *fp){
	int bits;
	fprintf(fp, "L AMNT\tCAP\tK_POS\tB_POS\n");
	for(int c='a'; letters; c++, letters>>=1){
		if(!(letters & 1)) continue;

		fprintf(fp, "%c ", c);
		fprintf(fp, "%d\t", (int) letter_data->amount&~CAPPED);
		fprintf(fp, "%c\t", (letter_data->amount&CAPPED?'T':'F'));
		bits = letter_data->known_pos;
		for(int j=0; j<5; j++)
			fprintf(fp, "%d", (bits&(1<<j))>>j);
		fprintf(fp, "\t");
		bits = letter_data->bad_pos;
		for(int j=0; j<5; j++)
			fprintf(fp, "%d", (bits&(1<<j))>>j);
		fprintf(fp, "\n");
		letter_data++;
	}

	fprintf(fp, "bad letters: ");
	for(int c='a'; bad_letters; c++, bad_letters>>=1)
		if(bad_letters & 1) fprintf(fp, "%c", c);
	fprintf(fp, "\n");
}


void saveTree(Node *tree[]){
	FILE *fp = fopen("data/tree.txt", "w");
	for(int i=0; i<=5; i++)
		printTree(tree[i], fp);
	fclose(fp);
}

void printTree(Node *node, FILE *fp){
	if(node == NULL) return;

	printTree(node->left, fp);

	fprintf(fp, "%8x %-8x - ", node->hash[0], node->hash[1]);
	for(int i=2; i<HASH_LEN; i++)
		fprintf(fp, "%04x %04x ", node->hash[i]>>16, node->hash[i]&0xFFFF);
	fprintf(fp, ": %d\n", node->elims);

	printTree(node->right, fp);
}


//calculate and return the weight of a bitstring
int weight(int bitstr){
	int w = 0;
	while(bitstr){
		w += bitstr & 1;
		bitstr >>= 1;
	}
	return w;
}
