#include "headers.h"



int testing = FALSE;
extern Dict words;



int main(int argc, char *argv[]){
	int arg = 1; //current argument being processed (skip past the name of the prog)

	double *total_elims;

	int best_words[100];
	double most_elims;

	if(strcmp(argv[arg], "-t") == 0){
		testing = TRUE;
		arg++;
	}

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

	if(testing) test(); //test

	printf("Calculating eliminations...");
	calcElims(total_elims);
	printf("Done\n");

	//DEBUG
	//output each word and its elims
	for(int i=0; i<words.guesses.len; i++){
		printf("%s: %.0f\n", getWord(i, words.guesses), total_elims[i]);
	}

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
	char current_letter = '\0';
	char *ans;
	DataS *data_table;
	Node *elims_tree[5+1];
	int elims;

	printf("\n");

	//initialize word-to-data table
	init_ans_data();
	printf("Answer to data table initialized.\n");

	data_table = (DataS *) malloc(words.guesses.len * sizeof (DataS));
	printf("Data table initialized.\n");

	for(int i=0; i<6; i++) elims_tree[i] = NULL;
	printf("Elims tree initialized.\n");

	if(testing) testElimsTable(); //test
	
	printf("Loading stored progress...\n");
	struct prog p = loadProgress(total_elims, elims_tree);
	int ans_index = p.answer;
	int g1_index = p.guess1;
	printf("Done\n");

	printf("Beginning combinatorical calculations...\n");
	initLogging(); //logging
	for(; ans_index < words.answers.len; ans_index++){
		ans = getWord(ans_index, words.answers);
		if(current_letter != ans[0]){
			current_letter = ans[0];
			printf("\n%c\n", toupper(current_letter));
		}

		genDataTable(ans, words.guesses, data_table);
		for(; g1_index < words.guesses.len-1; g1_index++){
			saveProgress(ans_index, g1_index, total_elims, elims_tree);
			
			for(int g2_index=g1_index+1; g2_index < words.guesses.len; g2_index++){
				timeStart(); //logging
				elims = getComboElims(data_table+g1_index, data_table+g2_index, elims_tree);
				total_elims[g1_index] += elims;
				total_elims[g2_index] += elims;
			}
			printLogging(); //logging
		}
		g1_index = 0;

		clearLoggingLookups(); //logging
	}
	
	//save final progress
	//	(set answer to answers length so any subsequent runs will skip past any calculations)
	saveProgress(words.answers.len, 0, total_elims, elims_tree);

	printf("\n");
	
	//free all malloced
	free(data_table);
	freeTree(elims_tree, 6);
}
