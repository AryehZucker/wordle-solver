#include "headers.h"

//write tree test to file for large testing samples

void test(Dict words){
	char word[6];
	word[5] = '\0';
	int n;
	DataA ans_data;

	printf("Testing...\n");

	srand(time(NULL));

	//make sure a char is only one byte
	printf("Char size: %d byte(s)\n\n", sizeof (char));
	printf("Int  size: %d byte(s)\n\n", sizeof (int));

	//Test dictionary functions
	printf("Testing Guess-Dict:\n");
	for(int i=0; i<3; i++){
		n = rand()%words.guesses.len;
		printf("%d:\t%s\n", n, strncpy(word, getWord(n,words.guesses), 5));
	}
	printf("\n");

	printf("Testing Ans-Dict:\n");
	for(int i=0; i<3; i++){
		n = rand()%words.answers.len;
		printf("%d:\t%s\n", n, strncpy(word, getWord(n,words.answers), 5));
	}
	printf("\n\n");

	//Test wordToData
	printf("Testing 'wordToData':\n");
	n = rand()%words.answers.len;
	strncpy(word, getWord(n, words.answers), 5);
	wordToData(word, &ans_data);
	showDataA(word, &ans_data);
	printf("\n\n");

	//Test weight
	printf("Testing 'weight':\n");
	n = rand()%(1<<5);
	printf("weight of ");
	for(int i=0; i<5; i++)
		printf("%d", (n&(1<<i))>>i);
	printf(" = %d\n", weight(n));
	printf("\n\n");

	//Test simplify
	printf("Testing 'simplify':\n");
	//test already simple data
	printf("no change\n");
	DataS data1 = {1<<('l'-'a') | 1<<('o'-'a'), 1<<('e'-'a') | 1<<('h'-'a'),
			{{1|CAPPED, 8, 8},
			{1, 0, 7}}};
	testSimplify(&data1);

	//test each inference alone
	//capped:2 & bas_pos:3
	printf("capped:2 & bad_pos:3\n");
	DataS data2 = data1;
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
	DataS *d_table = malloc(words.guesses.len * sizeof (DataS));
	genDataTable(ans, words.guesses, d_table);
	n = rand()%words.guesses.len;
	strncpy(word, getWord(n, words.guesses), 5);
	printf("Guess: %s\t", word);
	strncpy(word, ans, 5);
	printf("Answer: %s\n", word);
	printDataS(d_table+n, stdout);
	free(d_table);
	printf("\n\n");

	//Done testing
	printf("\n\n");
}

void testSimplify(DataS *data){
	printDataS(data, stdout);
	simplify(data->letter_data, weight(data->letters));
	printDataS(data, stdout);
	printf("\n");

}

void testElimsTable(Dict words, DataA *ans_data){
	long trials = 10000;
	FILE *outfile;
	char *ans, word[6];
	int g1, g2, elims;
	word[5] = '\0';
	DataS *d_table = malloc(words.guesses.len * sizeof (DataS));
	Node *e_tree[11];
	for(int i=0; i<11; i++) e_tree[i] = NULL;

	printf("\n\nTesting elims table...\n\n");

	outfile = fopen("data/test.txt", "w");
	while(trials--){
		//pick an ans
		ans = getWord(rand()%words.answers.len, words.answers);
		strncpy(word, ans, 5);
		fprintf(outfile, "Ans: %s\n", word);
		//load data table
		genDataTable(ans, words.guesses, d_table);
		//pick 2 guesses
		g1 = rand()%words.guesses.len;
		strncpy(word, getWord(g1, words.guesses), 5);
		fprintf(outfile, "Guess 1: %s\n", word);
		printDataS(d_table+g1, outfile);
		g2 = rand()%words.guesses.len;
		strncpy(word, getWord(g2, words.guesses), 5);
		fprintf(outfile, "Guess 2: %s\n", word);
		printDataS(d_table+g2, outfile);
		//find combo elims
		elims = getComboElims(d_table+g1, d_table+g2, ans_data, e_tree);
		fprintf(outfile, "Total elims (call 1): %d\n", elims);
		elims = getComboElims(d_table+g1, d_table+g2, ans_data, e_tree);
		fprintf(outfile, "Total elims (call 2): %d\n", elims);
		fprintf(outfile, "\n");
	}

	fclose(outfile);
	free(d_table);
	//i know that i'm leaving the whole tree floating out in memory, but i don't care

	printf("Save tree? ");
	if(getchar() == 'y') saveTree(e_tree);

	while(getchar() != '\n') ;
	printf("Done testing.\nPress ENTER to continue.");
	getchar();
	printf("\n\n");
}
