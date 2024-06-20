#include "headers.h"



//diagnostics
struct stat {
	struct { long successes, total; } lookups;
	struct { time_t start, last_update, t1, init, search, count; } times;
	long double runs, total_runs;
} diagnostics;

void initDiagnostics(Dict words){
	extern struct stat diagnostics;
	diagnostics.total_runs = (long double) words.answers.len * words.guesses.len * (words.guesses.len-1) / 2;
	diagnostics.runs = 0;
	diagnostics.lookups.successes = diagnostics.lookups.total = 0;
	diagnostics.times.init = diagnostics.times.search = diagnostics.times.count = 0;
	diagnostics.times.start = diagnostics.times.last_update = time(NULL);

	printf("%d answers\t%d guesses\n", words.answers.len, words.guesses.len);
	printf("Total runs: %.0Lf\n", diagnostics.total_runs);
}

void clearDiagnostics(){
	extern struct stat diagnostics;
        diagnostics.lookups.successes = diagnostics.lookups.total = 0;
        //diagnostics.times.init = diagnostics.times.search = diagnostics.times.count = 0;
}

void timeStart(){
	extern struct stat diagnostics;
	diagnostics.runs++;
	diagnostics.times.t1 = time(NULL);
}

void timeEnd(int mode){
	extern struct stat diagnostics;
	time_t now = time(NULL);
	time_t t = now - diagnostics.times.t1;
	diagnostics.times.t1 = now;
	switch(mode){
		case INIT:
			diagnostics.times.init += t;
			break;
		case SEARCH:
			diagnostics.times.search += t;
			break;
		case COUNT:
			diagnostics.times.count += t;
			break;
	}
}

void logLookup(int success){
	extern struct stat diagnostics;
	diagnostics.lookups.total++;
	if(success) diagnostics.lookups.successes++;
}


void printDiagnostics(){
	extern struct stat diagnostics;
	long double runs = diagnostics.runs, rcompleted;
	time_t now = time(NULL);
	double total_time = difftime(now,diagnostics.times.start);

	if(difftime(now, diagnostics.times.last_update) < 1) return;

	diagnostics.times.last_update = now;
	rcompleted = diagnostics.runs/diagnostics.total_runs; //ratio of completed to total
	putchar('\r');
	printf("total: %.4Lfms\t\t", 1000*(now-diagnostics.times.start)/runs);
	printf("init: %.4Lfms|srch: %.4Lfms|cnt: %.4Lfms\t",
		1000*diagnostics.times.init/runs, 1000*diagnostics.times.search/runs, 1000*diagnostics.times.count/runs);
	printf("%.2f%% suc\t", 100.0*diagnostics.lookups.successes/diagnostics.lookups.total);
	printf("[%.2Lf%% in %dm%ds; ", 100*rcompleted, (int)total_time/60, (int)total_time%60);
	printf("left: %.1Lf mins]\t", total_time*(1/rcompleted-1)/60);
	fflush(stdout);
}




char save_path[500];
void setSaveFile(const char *path){
	extern char save_path[];
	strncpy(save_path, path, min(sizeof save_path, strlen(path)));
}

int loadProgress(Node *tree[]){
	return 0;
}

void saveProgress(Node *tree[]){

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
