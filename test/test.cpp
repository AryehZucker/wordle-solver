#include "dictionary.hpp"
#include "tables.hpp"
#include "utils.hpp"
#include <iostream>

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

	std::cout << "Testing..." << std::endl;

	srand(time(NULL));

	//make sure a char is only one byte
	std::cout << "Char size: " << sizeof (char) << " byte(s)" << std::endl << std::endl;
	std::cout << "Int  size: " << sizeof (int) << " byte(s)" << std::endl << std::endl;

	//Test dictionary functions
	std::cout << "Testing Guess-Dict:" << std::endl;
	for(int i=0; i<3; i++){
		n = rand()%words.guesses.len;
		std::cout << n << ":\t" << getWord(n,words.guesses) << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Testing Ans-Dict:" << std::endl;
	for(int i=0; i<3; i++){
		n = rand()%words.answers.len;
		std::cout << n << ":\t" << getWord(n,words.answers) << std::endl;
	}
	std::cout << std::endl << std::endl;

	//Test wordToData
	std::cout << "Testing 'wordToData':" << std::endl;
	n = rand()%words.answers.len;
	word = getWord(n, words.answers);
	wordToData(word, &ans_data);
	showDataA(word, &ans_data);
	std::cout << std::endl << std::endl;

	//Test weight
	std::cout << "Testing 'weight':" << std::endl;
	n = rand()%(1<<WORDLEN);
	std::cout << "weight of ";
	for(int i=0; i<WORDLEN; i++)
		std::cout << ((n&(1<<i))>>i);
	std::cout << " = " << weight(n) << std::endl;
	std::cout << std::endl << std::endl;

	//Test simplify
	std::cout << "Testing 'simplify':" << std::endl;
	//test already simple data
	std::cout << "no change" << std::endl;
	struct DataS data1 = {1<<('l'-'a') | 1<<('o'-'a'), 1<<('e'-'a') | 1<<('h'-'a'),
			{{1|CAPPED, 8, 8},
			{1, 0, 7}}};
	testSimplify(&data1);

	//test each inference alone
	//capped:2 & bas_pos:3
	std::cout << "capped:2 & bad_pos:3" << std::endl;
	struct DataS data2 = data1;
	data2.letter_data[1].amount = 4; //not capped
	data2.letter_data[1].known_pos = 23;
	data2.letter_data[1].bad_pos = 0x1F;
	testSimplify(&data2);

	/*
	//capped:3 & known_pos:2
	std::cout << "capped:3 & known_pos:2" << std::endl;
	DataS data3 = data1;
	data3.letter_data[0].amount = 2; //not capped
	data3.letter_data[0].bad_pos = 17;
	testSimplify(&data3);

	//bad_pos:2
	std::cout << "bad_pos:2\n" << std::endl;
	DataS data4 = data1;
	data4.letter_data[0].amount = 1|CAPPED;
	data4.letter_data[0].known_pos = 16;
	data4.letter_data[0].bad_pos = 0x1F;
	testSimplify(&data4);
	*/

	std::cout << std::endl << std::endl;


	//Test genDataTable
	std::cout << "Testing data table functions:" << std::endl;
	char *ans = getWord(rand()%words.answers.len, words.answers);
	struct DataS *d_table = new struct DataS[words.guesses.len];
	genDataTable(ans, words.guesses, d_table);
	n = rand()%words.guesses.len;
	word = getWord(n, words.guesses);
	std::cout << "Guess: " << word << "\t";
	std::cout << "Answer: " << ans << std::endl;
	printDataS(d_table+n, std::cout);
	delete d_table;
	std::cout << std::endl << std::endl;

	//Done testing
	std::cout << std::endl << std::endl;
}

void testSimplify(struct DataS *data){
	printDataS(data, std::cout);
	simplify(data->letter_data, weight(data->letters));
	printDataS(data, std::cout);
	std::cout << std::endl;
}

void testElimsTable(){
	long trials = 10000;
	std::ofstream outfile("test.txt");
	char *ans;
	int g1, g2, elims;
	struct DataS *d_table = new struct DataS[words.guesses.len];
	struct Node *e_tree[WORDLEN+1];
	for(int i=0; i<WORDLEN+1; i++) e_tree[i] = NULL;

	std::cout << std::endl << std::endl;
	std::cout << "Testing elims table...";
	std::cout << std::endl << std::endl;

	while(trials--){
		//pick an ans
		ans = getWord(rand()%words.answers.len, words.answers);
		outfile << "Ans: " << ans << std::endl;
		//load data table
		genDataTable(ans, words.guesses, d_table);
		//pick 2 guesses
		g1 = rand()%words.guesses.len;
		g2 = rand()%words.guesses.len;
		outfile << "Guess 1: " << getWord(g1, words.guesses) << std::endl;
		outfile << "Guess 2: " << getWord(g2, words.guesses) << std::endl;
		printDataS(d_table+g1, outfile);
		printDataS(d_table+g2, outfile);
		//find combo elims
		elims = getComboElims(d_table+g1, d_table+g2, e_tree);
		outfile << "Total elims (call 1): " << elims << std::endl;
		elims = getComboElims(d_table+g1, d_table+g2, e_tree);
		outfile << "Total elims (call 2): " << elims << std::endl;
		outfile  << std::endl;
	}

	std::cout << "Save tree? ";
	if(std::cin.get() == 'y') saveTree(e_tree);
	
	
	outfile.close();
	delete d_table;
	deleteTree(e_tree, 6);

	while(std::cin.get() != '\n') ;
	std::cout << "Done testing." << std::endl;
	std::cout << "Press ENTER to continue.";
	std::cin.get();
	std::cout << std::endl << std::endl;
}
