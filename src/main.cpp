#include "dictionary.hpp"
#include "logging.hpp"
#include "tables.hpp"
#include "utils.hpp"
#include <csignal>
#include <iostream>


struct Dict words;

void calcElims(double *total_elims);


int main(int argc, char *argv[]){
	int arg = 1; //current argument being processed (skip past the name of the prog)

	double *total_elims;

	int best_words[100];
	double most_elims;

	//load dictionary files
	loadDict(argv[arg], argv[arg+1], words);
	arg += 2;
	if(words.guesses.len < 0 || words.answers.len < 0){
		std::cerr << "Error loading word lists" << std::endl;
		return 1;
	}

	//allot space to store the total eliminations and initialize to zero
	total_elims = new double[words.guesses.len]();

	std::cout << "Calculating eliminations...";
	calcElims(total_elims);
	std::cout << "Done" << std::endl;

	std::cout << "Finding best words...";
	most_elims = 0;
	for(int i=0; i<words.guesses.len; i++)
		if(total_elims[i] > most_elims)
			most_elims = total_elims[i];

	unsigned int j = 0;
	for(int i=0; i<words.guesses.len && j<(sizeof(best_words)/sizeof(int)); i++)
		if(total_elims[i] == most_elims)
			best_words[j++] = i;
	best_words[j] = -1; //mark end of list
	std::cout << "Done" << std::endl;

	//make the stored totals into averages
	for(int i=0; i<words.guesses.len; i++)
		total_elims[i] /= words.answers.len * (words.guesses.len-1);

	std::cout << std::endl << "Best words:" << std::endl;
	for(int i=0; best_words[i] >= 0; i++){
		std::cout << getWord(best_words[i], words.guesses) << std::endl;
	}

	delete[] total_elims;
	delete_ans_data();

	return 0;
}




void calcElims(double *total_elims){
	char *ans;
	struct DataS *data_table;
	int elims;

	std::cout << std::endl;

	//initialize word-to-data table
	init_ans_data(words);
	std::cout << "Answer to data table initialized." << std::endl;

	data_table = new struct DataS[words.guesses.len];
	std::cout << "Data table initialized." << std::endl;

	std::cout << "Beginning combinatorial calculations..." << std::endl;
	Logger logger(words);
	for(int ans_index = 0; ans_index < words.answers.len; ans_index++){
		ans = getWord(ans_index, words.answers);
		genDataTable(ans, words.guesses, data_table);
		for(int g1_index=0; g1_index < words.guesses.len-1; g1_index++){
			for(int g2_index=g1_index+1; g2_index < words.guesses.len; g2_index++){
				elims = getComboElims(data_table+g1_index, data_table+g2_index);
				total_elims[g1_index] += elims;
				total_elims[g2_index] += elims;
				logger.logCompletedIteration();
			}
			logger.displayProgress();
		}
	}

	std::cout << std::endl;

	delete[] data_table;
}
