#include "headers.h"



extern Dict words;

//logging
struct stat {
	struct { long successes, total; } lookups;
	struct { time_t offset, start, last_update, t1, init, search, count; } times;
	long double runs, total_runs;
} logging;

void initLogging(struct prog p){
	logging.total_runs = (long double) words.answers.len * words.guesses.len * (words.guesses.len-1) / 2;
	logging.runs = p.runs;
	logging.lookups.successes = p.lookup_successes;
	logging.lookups.total = p.total_lookups;
	logging.times.offset = p.time_offset;
	logging.times.init = logging.times.search = logging.times.count = 0;
	logging.times.start = logging.times.last_update = time(NULL);

	printf("%d answers\t%d guesses\n", words.answers.len, words.guesses.len);
	printf("Total runs: %.0Lf\n", logging.total_runs);
}

void clearLookups(){
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
	double total_time = logging.times.offset + difftime(now,logging.times.start);

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




char save_path[100] = "save.dat";
int setSaveFile(const char *path){
	if(sizeof save_path < strlen(path))
		return -1;

	strncpy(save_path, path, sizeof save_path);

	return 0;
}

void writeTree(Node *node, FILE *fp){
	if(node == NULL) return;
	
	writeTree(node->left, fp);
	fwrite(node->hash, sizeof(unsigned int), HASH_LEN, fp);
	fwrite(&node->elims, sizeof(int), 1, fp);
	writeTree(node->right, fp);
}

long double treeSize(Node *node){
	if(node == NULL) return 0;
	
	return treeSize(node->left) + 1 + treeSize(node->right);
}

void saveProgress(int answer, double* total_elims, Node *tree[]){
	FILE *fp;
	long double size;
	double total_time = logging.times.offset + difftime(time(NULL), logging.times.start);

	if((fp = fopen(save_path, "wb")) == NULL){
		char message[200];
		sprintf(message, "Error opening %s", save_path);
		perror(message);
		exit(1);
	}

	//write logging info
	fwrite(&logging.runs, sizeof (logging.runs), 1, fp);
	fwrite(&total_time, sizeof (total_time), 1, fp);
	fwrite(&logging.lookups.successes, sizeof (logging.lookups.successes), 1, fp);
	fwrite(&logging.lookups.total, sizeof (logging.lookups.total), 1, fp);
	
	//write which ans & guess we're upto
	fwrite(&answer, sizeof (answer), 1, fp);

	//write total_elims
	fwrite(&words.guesses.len, sizeof (words.guesses.len), 1, fp);
	fwrite(total_elims, sizeof (double), words.guesses.len, fp);

	//write elims tree
	for(int i=0; i<6; i++){
		size = treeSize(tree[i]);
		fwrite(&size, sizeof (size), 1, fp);
		writeTree(tree[i], fp);
	}
	
	//ensure everything was written
	if(ferror(fp)){
		char message[200];
		sprintf(message, "Error writing to file %s", save_path);
		perror(message);
		fclose(fp);
		exit(1);
	}
	
	//close the file
	fclose(fp);
}

Node *readTree(long double size, FILE *fp){
	if(size == 0) return NULL;
	
	Node *node = malloc(sizeof(Node));
	
	long double remaining_size = size - 1,
		left_size = floor(remaining_size / 2),
		right_size = remaining_size - left_size;
	
	node->left = readTree(left_size, fp);
	
	fread(node->hash, sizeof(unsigned int), HASH_LEN, fp);
	fread(&node->elims, sizeof(int), 1, fp);
	
	node->right = readTree(right_size, fp);
	
	return node;
}

struct prog loadProgress(double *total_elims, Node *tree[]){
	struct prog p, p_empty = {0, 0.0L, 0.0, 0L, 0L};
	FILE *fp;
	int guesses_len;

	if((fp = fopen(save_path, "rb")) == NULL){
		printf("No saved data found\n");
		return p_empty;
	}
	
	printf("Found saved data\n");

	//read logging info
	fread(&p.runs, sizeof (p.runs), 1, fp);
	fread(&p.time_offset, sizeof (p.time_offset), 1, fp);
	fread(&p.lookup_successes, sizeof (p.lookup_successes), 1, fp);
	fread(&p.total_lookups, sizeof (p.total_lookups), 1, fp);
	
	//printf("%.0Lf %ld %ld\n", p.runs, p.lookup_successes, p.total_lookups);	//DEBUG
	
	//read which ans & guess we're upto
	fread(&p.answer, sizeof (p.answer), 1, fp);
	
	//read in total_elims
	fread(&guesses_len, sizeof (guesses_len), 1, fp);
	if(guesses_len != words.guesses.len){
		fprintf(stderr, "Error reading from file %s: amount of data does not match\n", save_path);
		fprintf(stderr, "Starting calculations from the beginning.\n");
		fclose(fp);
		return p_empty;
	}
	fread(total_elims, sizeof (double), words.guesses.len, fp);
	if(feof(fp)){
		fprintf(stderr, "Error reading from file %s: EOF encountered while parsing file\n", save_path);
		fprintf(stderr, "Starting calculations from the beginning.\n");
		fclose(fp);
		memset(total_elims, 0, sizeof (double) * words.guesses.len);
		return p_empty;
	}

	//read in the elims tree
	long double size;
	for(int i=0; i<6; i++){
		//find the size of this section of the tree
		fread(&size, sizeof size, 1, fp);
		
		//generate a tree of that size and load it in
		tree[i] = readTree(size, fp);
	}
	
	//ensure averything was read
	if(ferror(fp) || feof(fp)){
		if(ferror(fp)){
			char message[200];
			sprintf(message, "Error reading from file %s", save_path);
			perror(message);
		}
		else{
			fprintf(stderr, "Error reading from file %s: EOF encountered while parsing file\n", save_path);
		}
		fprintf(stderr, "Starting calculations from the beginning.\n");
		
		p = p_empty;
		memset(total_elims, 0, sizeof (double) * words.guesses.len);
		freeTree(tree, 6);
		for(int i=0; i<6; i++) tree[i] = NULL;
	}
	
	//close the file
	fclose(fp);

	return p;
}




void showDataA(const char *word, const DataA *data){
	int bits, letters = data->letters;
	const DataLA *letter_data = data->letter_data;

	printf("%s\n", word);

	for(char c='a'; letters; c++, letters>>=1){
		if(letters & 1){
			printf("  %c - %d ", c, (int) letter_data->amount);
			bits = letter_data->pos;
			for(int j=0; j<WORDLEN; j++)
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
		for(int j=0; j<WORDLEN; j++)
			fprintf(fp, "%d", (bits&(1<<j))>>j);
		fprintf(fp, "\t");
		bits = letter_data->bad_pos;
		for(int j=0; j<WORDLEN; j++)
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
	for(int i=0; i<=WORDLEN; i++)
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
