#include "utils.hpp"
#include "dictionary.h"
#include "tables.h"
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>


extern struct Dict words;

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

	std::cout << words.answers.len << " answers\t" << words.guesses.len << " guesses" << std::endl;
	std::cout << "Total runs: " << std::fixed << std::setprecision(0) << logging.total_runs << std::endl;
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
	long double runs = logging.runs, proportion_completed;
	time_t now = time(NULL);
	double total_time = logging.times.offset + difftime(now,logging.times.start);

	if(difftime(now, logging.times.last_update) < 1) return;

	logging.times.last_update = now;
	proportion_completed = logging.runs/logging.total_runs; //ratio of completed to total
	std::cout << "\r";
	std::cout << std::fixed << std::setprecision(4);
	std::cout << "total: " << 1000*(now-logging.times.start)/runs << "ms\t\t";
	std::cout << "init: " << 1000*logging.times.init/runs << "ms|";
	std::cout << "srch: " << 1000*logging.times.search/runs << "ms|";
	std::cout << "cnt: " << 1000*logging.times.count/runs << "ms\t";
	std::cout << std::setprecision(2);
	std::cout << (100.0*logging.lookups.successes/logging.lookups.total) << "% suc\t";
	std::cout << "[" << 100*proportion_completed << "% in " << (int)total_time/60 << "m" << (int)total_time%60 << "s; ";
	std::cout << std::setprecision(1);
	std::cout << "left: " << (total_time*(1/proportion_completed-1)/60) << " mins]\t";
	std::cout << std::flush;
}




char save_path[100] = "save.dat";
int setSaveFile(const char *path){
	if(sizeof save_path < strlen(path))
		return -1;

	strncpy(save_path, path, sizeof save_path);

	return 0;
}

void writeTree(struct Node *node, std::ostream &file){
	if(node == NULL) return;

	writeTree(node->left, file);
	file.write((char *)(node->hash), sizeof(unsigned int) * HASH_LEN);
	file.write((char *)(&node->elims), sizeof(int));
	writeTree(node->right, file);
}

long double treeSize(struct Node *node){
	if(node == NULL) return 0;

	return treeSize(node->left) + 1 + treeSize(node->right);
}

void saveProgress(int answer, double* total_elims, struct Node *tree[]){
	std::ofstream file(save_path, std::ios::binary);
	long double size;
	double total_time = logging.times.offset + difftime(time(NULL), logging.times.start);

	if(!file.is_open()){
		std::cerr << "Error opening " << save_path << ": " << std::strerror(errno) << std::endl;
		exit(1);
	}

	//write logging info
	file.write((char *)&logging.runs, sizeof (logging.runs));
	file.write((char *)&total_time, sizeof (total_time));
	file.write((char *)&logging.lookups.successes, sizeof (logging.lookups.successes));
	file.write((char *)&logging.lookups.total, sizeof (logging.lookups.total));

	//write which ans & guess we're upto
	file.write((char *)&answer, sizeof (answer));

	//write total_elims
	file.write((char *)&words.guesses.len, sizeof (words.guesses.len));
	file.write((char *)total_elims, sizeof (double) * words.guesses.len);

	//write elims tree
	for(int i=0; i<6; i++){
		size = treeSize(tree[i]);
		file.write((char *)&size, sizeof (size));
		writeTree(tree[i], file);
	}

	//ensure everything was written
	if(file.fail()){
		std::cerr << "Error writing to file " << save_path << ": " << std::strerror(errno) << std::endl;
		file.close();
		exit(1);
	}

	//close the file
	file.close();
}

struct Node *readTree(long double size, std::ifstream &file){
	if(size == 0) return NULL;

	struct Node *node = new struct Node;

	long double remaining_size = size - 1,
		left_size = floor(remaining_size / 2),
		right_size = remaining_size - left_size;

	node->left = readTree(left_size, file);

	file.read((char *)node->hash, sizeof(unsigned int) * HASH_LEN);
	file.read((char *)&node->elims, sizeof(int));

	node->right = readTree(right_size, file);

	return node;
}

struct prog loadProgress(double *total_elims, struct Node *tree[]){
	struct prog p, p_empty = {0, 0.0L, 0.0, 0L, 0L};
	std::ifstream file(save_path, std::ios::binary);
	int guesses_len;

	if(!file.is_open()){
		std::cout << "No saved data found" << std::endl;
		return p_empty;
	}

	std::cout << "Found saved data" <<std::endl;

	//read logging info
	file.read((char *)&p.runs, sizeof (p.runs));
	file.read((char *)&p.time_offset, sizeof (p.time_offset));
	file.read((char *)&p.lookup_successes, sizeof (p.lookup_successes));
	file.read((char *)&p.total_lookups, sizeof (p.total_lookups));

	//read which ans & guess we're upto
	file.read((char *)&p.answer, sizeof (p.answer));

	//read in total_elims
	file.read((char *)&guesses_len, sizeof (guesses_len));
	if(guesses_len != words.guesses.len){
		std::cerr << "Error reading from file " << save_path << ": amount of data does not match" << std::endl;
		std::cerr << "Starting calculations from the beginning." << std::endl;
		file.close();
		return p_empty;
	}
	file.read((char *)total_elims, sizeof (double) * words.guesses.len);
	if(file.eof()){
		std::cerr << "Error reading from file " << save_path << ": EOF encountered while parsing file" << std::endl;
		std::cerr << "Starting calculations from the beginning." << std::endl;
		file.close();
		memset(total_elims, 0, sizeof (double) * words.guesses.len);
		return p_empty;
	}

	//read in the elims tree
	long double size;
	for(int i=0; i<6; i++){
		//find the size of this section of the tree
		file.read((char *)&size, sizeof size);

		//generate a tree of that size and load it in
		tree[i] = readTree(size, file);
	}

	//ensure everything was read
	if(file.fail() || file.eof()){
		if(file.fail()){
			std::cerr << "Error reading from file  " << save_path << ": " << std::strerror(errno) << std::endl;
		}
		else{
			std::cerr << "Error reading from file " << save_path << ": EOF encountered while parsing file" << std::endl;
		}
		std::cerr << "Starting calculations from the beginning." << std::endl;

		p = p_empty;
		memset(total_elims, 0, sizeof (double) * words.guesses.len);
		deleteTree(tree, 6);
		for(int i=0; i<6; i++) tree[i] = NULL;
	}

	//close the file
	file.close();

	return p;
}




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

void printDataS(const struct DataS *data, std::ostream &file){
	printData(data->letters, data->letter_data, data->bad_letters, file);
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

	file << std::setw(8) << std::hex << std::setfill('0') << node->hash[0] << " " << std::left << std::setw(8) << node->hash[1] << " - ";
	for(int i=2; i<HASH_LEN; i++)
		file << std::setw(4) << std::hex << std::setfill('0') << (node->hash[i] >> 16)
				<< " " << std::setw(4) << std::hex << std::setfill('0') << (node->hash[i] & 0xFFFF)
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
