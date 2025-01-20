#include "dictionary.h"
#include "tables.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//write tree test to file for large testing samples

void test(void);
void testSimplify(struct DataS *);
void testElimsTable(void);

extern struct Dict words;

int main() {
	test();
	testElimsTable();
}

void test(){
	char *word;
	int n;
	struct DataA ans_data;

	printf("Testing...\n");

	srand(time(NULL));

	//make sure a char is only one byte
	printf("Char size: %ld byte(s)\n\n", sizeof (char));
	printf("Int  size: %ld byte(s)\n\n", sizeof (int));

	//Test dictionary functions
	printf("Testing Guess-Dict:\n");
	for(int i=0; i<3; i++){
		n = rand()%words.guesses.len;
		printf("%d:\t%s\n", n, getWord(n,words.guesses));
	}
	printf("\n");

	printf("Testing Ans-Dict:\n");
	for(int i=0; i<3; i++){
		n = rand()%words.answers.len;
		printf("%d:\t%s\n", n, getWord(n,words.answers));
	}
	printf("\n\n");

	//Test wordToData
	printf("Testing 'wordToData':\n");
	n = rand()%words.answers.len;
	word = getWord(n, words.answers);
	wordToData(word, &ans_data);
	showDataA(word, &ans_data);
	printf("\n\n");

	//Test weight
	printf("Testing 'weight':\n");
	n = rand()%(1<<WORDLEN);
	printf("weight of ");
	for(int i=0; i<WORDLEN; i++)
		printf("%d", (n&(1<<i))>>i);
	printf(" = %d\n", weight(n));
	printf("\n\n");

	//Test simplify
	printf("Testing 'simplify':\n");
	//test already simple data
	printf("no change\n");
	struct DataS data1 = {1<<('l'-'a') | 1<<('o'-'a'), 1<<('e'-'a') | 1<<('h'-'a'),
			{{1|CAPPED, 8, 8},
			{1, 0, 7}}};
	testSimplify(&data1);

	//test each inference alone
	//capped:2 & bas_pos:3
	printf("capped:2 & bad_pos:3\n");
	struct DataS data2 = data1;
	data2.letter_data[1].amount = 4; //not capped
	data2.letter_data[1].known_pos = 23;
	data2.letter_data[1].bad_pos = 0x1F;
	testSimplify(&data2);

	/*
	//capped:3 & known_pos:2
	printf("capped:3 & known_pos:2\n");
	DataS data3 = data1;
	data3.letter_data[0].amount = 2; //not capped
	data3.letter_data[0].bad_pos = 17;
	testSimplify(&data3);

	//bad_pos:2
	printf("bad_pos:2\n");
	DataS data4 = data1;
	data4.letter_data[0].amount = 1|CAPPED;
	data4.letter_data[0].known_pos = 16;
	data4.letter_data[0].bad_pos = 0x1F;
	testSimplify(&data4);
	*/

	printf("\n\n");


	//Test genDataTable
	printf("Testing data table functions:\n");
	char *ans = getWord(rand()%words.answers.len, words.answers);
	struct DataS *d_table = malloc(words.guesses.len * sizeof (struct DataS));
	genDataTable(ans, words.guesses, d_table);
	n = rand()%words.guesses.len;
	word = getWord(n, words.guesses);
	printf("Guess: %s\t", word);
	printf("Answer: %s\n", ans);
	printDataS(d_table+n, stdout);
	free(d_table);
	printf("\n\n");

	//Done testing
	printf("\n\n");
}

void testSimplify(struct DataS *data){
	printDataS(data, stdout);
	simplify(data->letter_data, weight(data->letters));
	printDataS(data, stdout);
	printf("\n");

}

void testElimsTable(){
	long trials = 10000;
	FILE *outfile;
	char *ans;
	int g1, g2, elims;
	struct DataS *d_table = malloc(words.guesses.len * sizeof (struct DataS));
	struct Node *e_tree[WORDLEN+1];
	for(int i=0; i<WORDLEN+1; i++) e_tree[i] = NULL;

	printf("\n\nTesting elims table...\n\n");

	outfile = fopen("data/test.txt", "w");
	while(trials--){
		//pick an ans
		ans = getWord(rand()%words.answers.len, words.answers);
		fprintf(outfile, "Ans: %s\n", ans);
		//load data table
		genDataTable(ans, words.guesses, d_table);
		//pick 2 guesses
		g1 = rand()%words.guesses.len;
		g2 = rand()%words.guesses.len;
		fprintf(outfile, "Guess 1: %s\n", getWord(g1, words.guesses));
		fprintf(outfile, "Guess 2: %s\n", getWord(g2, words.guesses));
		printDataS(d_table+g1, outfile);
		printDataS(d_table+g2, outfile);
		//find combo elims
		elims = getComboElims(d_table+g1, d_table+g2, e_tree);
		fprintf(outfile, "Total elims (call 1): %d\n", elims);
		elims = getComboElims(d_table+g1, d_table+g2, e_tree);
		fprintf(outfile, "Total elims (call 2): %d\n", elims);
		fprintf(outfile, "\n");
	}

	printf("Save tree? ");
	if(getchar() == 'y') saveTree(e_tree);
	
	
	fclose(outfile);
	free(d_table);
	freeTree(e_tree, 6);

	while(getchar() != '\n') ;
	printf("Done testing.\nPress ENTER to continue.");
	getchar();
	printf("\n\n");
}
