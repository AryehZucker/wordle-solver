#include "dictionary.h"
#include "tables.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


int interrupt = 0;
extern struct Dict words;

void calcElims(double *total_elims);


void handler(int sig){
	if(sig == SIGINT){
		interrupt = 1;
	}
}

int main(int argc, char *argv[]){
	int arg = 1; //current argument being processed (skip past the name of the prog)

	double *total_elims;

	int best_words[100];
	double most_elims;

	signal(SIGINT, handler);

	//load dictionary files
	loadDict(argv[arg], argv[arg+1]);
	arg += 2;
	if(words.guesses.len < 0 || words.answers.len < 0){
		fprintf(stderr, "Error loading word lists\n");
		return 1;
	}

	if(arg < argc){
		if(setSaveFile(argv[arg++]) < 0){
			fprintf(stderr, "Error setting save path\n");
			return 1;
		}
	}

	//allot space to store the total eliminations and initialize to zero
	total_elims = (double *) calloc(words.guesses.len, sizeof (double));

	printf("Calculating eliminations...");
	calcElims(total_elims);
	printf("Done\n");

	//DEBUG
	//output each word and its elims
	//for(int i=0; i<words.guesses.len; i++){
	//	printf("%s: %.0f\n", getWord(i, words.guesses), total_elims[i]);
	//}

	printf("Finding best words...");
	most_elims = 0;
	for(int i=0; i<words.guesses.len; i++)
		if(total_elims[i] > most_elims)
			most_elims = total_elims[i];

	int j = 0;
	for(int i=0; i<words.guesses.len && j<(sizeof(best_words)/sizeof(int)); i++)
		if(total_elims[i] == most_elims)
			best_words[j++] = i;
	best_words[j] = -1; //mark end of list
	printf("Done\n");

	//make the stored totals into averages
	for(int i=0; i<words.guesses.len; i++)
		total_elims[i] /= words.answers.len * (words.guesses.len-1);

	printf("\nBest words:\n");
	for(int i=0; best_words[i] >= 0; i++){
		printf("%s\n", getWord(best_words[i], words.guesses));
	}

	//free all malloced memory
	free(total_elims);
	free_ans_data();

	return 0;
}




void calcElims(double *total_elims){
	char *ans;
	struct DataS *data_table;
	struct Node *elims_tree[WORDLEN+1];
	int elims;

	printf("\n");

	//initialize word-to-data table
	init_ans_data();
	printf("Answer to data table initialized.\n");

	data_table = (struct DataS *) malloc(words.guesses.len * sizeof (struct DataS));
	printf("Data table initialized.\n");

	for(int i=0; i<6; i++) elims_tree[i] = NULL;
	printf("Elims tree initialized.\n");

	printf("Loading stored progress...\n");
	struct prog p = loadProgress(total_elims, elims_tree);
	int ans_index = p.answer;
	printf("Done\n");

	printf("Beginning combinatorical calculations...\n");
	initLogging(p); //logging
	for(; ans_index < words.answers.len; ans_index++){
		printf("\nSaving\n");
		saveProgress(ans_index, total_elims, elims_tree);

		ans = getWord(ans_index, words.answers);
		genDataTable(ans, words.guesses, data_table);
		for(int g1_index=0; g1_index < words.guesses.len-1; g1_index++){
			for(int g2_index=g1_index+1; g2_index < words.guesses.len; g2_index++){
				timeStart(); //logging
				elims = getComboElims(data_table+g1_index, data_table+g2_index, elims_tree);
				total_elims[g1_index] += elims;
				total_elims[g2_index] += elims;
			}
			printLogging(); //logging
			if(interrupt) exit(1);
		}
	}

	//save final progress
	//	(set answer to answers length so any subsequent runs will skip past any calculations)
	saveProgress(words.answers.len, total_elims, elims_tree);

	printf("\n");

	//free all malloced
	free(data_table);
	freeTree(elims_tree, 6);
}
