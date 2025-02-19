#include "dictionary.hpp"
#include "feedback.hpp"
#include "logging.hpp"
#include "tables.hpp"

#include <iostream>
#include <vector>


struct Dict words;

void calculateEliminations(double *total_eliminations);


int main(int argc, char *argv[]){
	double *total_eliminations;
	int best_words[100];
	double max_eliminations;

	loadDict(argv[1], argv[2], words);
	if(words.guesses.len < 0 || words.answers.len < 0){
		std::cerr << "Error loading word lists" << std::endl;
		return 1;
	}

	//allot space to store the total eliminations and initialize to zero
	total_eliminations = new double[words.guesses.len]();

	std::cout << "Calculating eliminations..." << std::endl;
	calculateEliminations(total_eliminations);
	std::cout << "Done" << std::endl;

	std::cout << "Finding best words...";
	max_eliminations = 0;
	for(int i=0; i<words.guesses.len; i++)
		if(total_eliminations[i] > max_eliminations)
			max_eliminations = total_eliminations[i];

	unsigned int j = 0;
	for(int i=0; i<words.guesses.len && j<(sizeof(best_words)/sizeof(int)); i++)
		if(total_eliminations[i] == max_eliminations)
			best_words[j++] = i;
	best_words[j] = -1; //mark end of list
	std::cout << "Done" << std::endl;

	//make the stored totals into averages
	for(int i=0; i<words.guesses.len; i++)
		total_eliminations[i] /= words.answers.len * (words.guesses.len-1);

	std::cout << std::endl << "Best words:" << std::endl;
	for(int i=0; best_words[i] >= 0; i++){
		std::cout << getWord(best_words[i], words.guesses) << std::endl;
	}

	delete[] total_eliminations;
	delAnsToDataTable();

	return 0;
}




void calculateEliminations(double *total_eliminations){
	int eliminations;

	//initialize word-to-data table
	initAnsToDataTable(words);
	std::cout << "Answer to data table initialized." << std::endl;

	std::vector<Feedback> data_table;
	data_table.reserve(words.guesses.len);

	std::cout << "Beginning combinatorial calculations..." << std::endl;
	Logger logger(words);
	for(int ans_index = 0; ans_index < words.answers.len; ans_index++){
		genDataTable(getWord(ans_index, words.answers), words.guesses, data_table);
		for(int g1_index=0; g1_index < words.guesses.len-1; g1_index++){
			for(int g2_index=g1_index+1; g2_index < words.guesses.len; g2_index++){
				eliminations = getElims(data_table[g1_index] + data_table[g2_index]);
				total_eliminations[g1_index] += eliminations;
				total_eliminations[g2_index] += eliminations;
				logger.logCompletedIteration();
			}
			logger.displayProgress();
		}
	}

	std::cout << std::endl;
}
