#include "utils.hpp"

#include "dictionary.hpp"
#include "feedback.hpp"
#include "tables.hpp"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

void showDataA(const char *word, const struct DataA *data){
	int bits, letters = data->letters;
	const struct DataLA *letter_data = data->letter_data;

	std::cout << word << std::endl;

	for(char c='a'; letters; c++, letters>>=1){
		if(letters & 1){
			std::cout << "  " << c << " - " << (int) letter_data->amount << " ";
			bits = letter_data->pos;
			for(int j=0; j<WORDLEN; j++)
				std::cout << ((bits&(1<<j))>>j);
			std::cout << std::endl;

			letter_data++;
		}
	}
}

void printData(int letters, const struct DataL *letter_data, int bad_letters, std::ostream &file){
	int bits;
	file << "L AMNT\tCAP\tK_POS\tB_POS" << std::endl;
	for(int c='a'; letters; c++, letters>>=1){
		if(!(letters & 1)) continue;

		file << c << " ";
		file << (int)(letter_data->amount&~CAPPED) << "\t";
		file << (letter_data->amount&CAPPED?'T':'F') << "\t";
		bits = letter_data->known_pos;
		for(int j=0; j<WORDLEN; j++)
			file << ((bits&(1<<j))>>j);
		file << "\t";
		bits = letter_data->bad_pos;
		for(int j=0; j<WORDLEN; j++)
			file << ((bits&(1<<j))>>j);
		file << std::endl;
		letter_data++;
	}

	file << "bad letters: ";
	for(int c='a'; bad_letters; c++, bad_letters>>=1)
		if(bad_letters & 1) file << c;
	file << std::endl;
}


void saveTree(struct Node *tree[]){
	std::ofstream file("tree.txt");
	for(int i=0; i<=WORDLEN; i++)
		printTree(tree[i], file);
	file.close();
}

void printTree(struct Node *node, std::ostream &file){
	if(node == NULL) return;

	printTree(node->left, file);

	file << std::setw(8) << std::hex << std::setfill('0') << node->feedback->data_hash[0] << " " << std::left << std::setw(8) << node->feedback->data_hash[1] << " - ";
	for(int i=2; i<HASH_LEN; i++)
		file << std::setw(4) << std::hex << std::setfill('0') << (node->feedback->data_hash[i] >> 16)
				<< " " << std::setw(4) << std::hex << std::setfill('0') << (node->feedback->data_hash[i] & 0xFFFF)
				<< " ";
	file << ": " << node->elims << std::endl;

	printTree(node->right, file);
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
