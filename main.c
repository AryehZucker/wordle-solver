#include "headers.h"



Dict words;
extern DataA *answers_data;



int main(int argc, char *argv[]){
	int arg = 1; //skip past the name of the prog
	int t = 0; //test bool

	double *total_elims;

	int best_words[100];
	char *word;
	double most_elims;

	if(strcmp(argv[arg], "-t") == 0){
		t = 1;
		arg++;
	}

	//load dictionary files
	words = getDict(argv[arg], argv[arg+1]);
	arg += 2;
	if(words.guesses.len < 0 || words.answers.len < 0){
		fprintf(stderr, "Error loading word lists\n");
		return -1;
	}

	if(arg < argc) setSaveFile(argv[arg++]);

	//allot space to store the total eliminations and initialize to zero
	total_elims = (double *) malloc(words.guesses.len * sizeof (double));
	for(int i=0; i<words.guesses.len; i++) total_elims[i] = 0;

	if(t) test(); //test

	printf("Calculating eliminations...");
	calcElims(total_elims, t);
	printf("Done\n");

	//DEBUG
	//output each word and its elims
	for(int i=0; i<words.guesses.len; i++){
		char word[6];
		strncpy(word, getWord(i, words.guesses), 5);
		word[5] = '\0';
		printf("%s: %.0f\n", word, total_elims[i]);
	}

	printf("Finding best words...");
	most_elims = 0;
	for(int i=0; i<words.guesses.len; i++)
		if(total_elims[i] > most_elims)
			most_elims = total_elims[i];

	int j = 0;
	for(int i=0; i<words.guesses.len && j<100; i++)
		if(total_elims[i] == most_elims)
			best_words[j++] = i;
	best_words[j] = -1; //mark end of list
	printf("Done\n");

	//make the stored totals into averages
	for(int i=0; i<words.guesses.len; i++)
		total_elims[i] /= words.answers.len * (words.guesses.len-1);

	printf("\nBest words:\n");
	for(int i=0; best_words[i] >= 0; i++){
		word = getWord(best_words[i], words.guesses);
		for(int j=0; j<5; j++)
			putchar(word[j]);
		putchar('\n');
	}

	printf("\nSaving data...");
	if(saveData(total_elims, words.guesses.len) != 0)
		fprintf(stderr, "Error saving data :(\n");
	else
		printf("Done\n");

	return 0;
}




void calcElims(double *total_elims, int test){
	char current_letter = '\0';
	char *ans;
	DataS *data_table;
	Node *e_tree[5+1];
	int elims;

	printf("\n");

	//initialize word-to-data table
	init_ans_data();
	printf("Answer to data table initialized.\n");

	data_table = (DataS *) malloc(words.guesses.len * sizeof (DataS));
	printf("Data table initialized.\n");

	for(int i=0; i<=5; i++) e_tree[i] = NULL;
	printf("Elims tree initialized.\n");

	if(test) testElimsTable(); //test

	printf("Beginning combinatorical calculations...\n");
	initLogging(); //logging
	for(int ans_index = loadProgress(e_tree); ans_index < words.answers.len; ans_index++){
		ans = getWord(ans_index, words.answers);
		if(current_letter != ans[0]){
			current_letter = ans[0];
			printf("\n%c\n", toupper(current_letter));
		}

		genDataTable(ans, words.guesses, data_table);
		for(int g1_index=0; g1_index < words.guesses.len-1; g1_index++){
			for(int g2_index=g1_index+1; g2_index < words.guesses.len; g2_index++){
				timeStart(); //logging
				elims = getComboElims(data_table+g1_index, data_table+g2_index, e_tree);
				total_elims[g1_index] += elims;
				total_elims[g2_index] += elims;
			}
			printLogging(); //logging
		}

		saveProgress(e_tree);

                //for(int i=0; i<5; i++) putchar(ans[i]);
                //putchar('\n');
		clearLogging(); //logging
	}

	saveTree(e_tree); //DEBUG

	printf("\n");
}
