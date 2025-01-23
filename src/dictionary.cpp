#include "dictionary.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>

void loadDict(const char *ans_words_path, const char *guess_words_path, struct Dict &words){
	words.answers = loadWordList(ans_words_path);
	words.guesses = loadWordList(guess_words_path);
}

struct WordList loadWordList(const char *path){
	std::ifstream file(path, std::ios::binary);
	struct WordList wl;
	int word_count = 0;
	long filesize;

	if(!file.is_open()){
		std::cerr << "Error opening " << path << ": " << std::strerror(errno) << std::endl;
		wl.len = -1;
		return wl;
	}

	file.seekg(0, std::ios::end);
	filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	wl.words = new char[filesize];

	for(char *word = wl.words; file >> word; word += WORDLEN+1){
		word_count++;
		if(file.fail() || strlen(word) != 5){
			std::cerr << "Error: data corruption in " << path << std::endl;
			wl.len = -1;
			return wl;
		}
	}
	file.close();

	wl.len = word_count;

	return wl;
}

//return a pointer to the first letter of the word in the WordList
char *getWord(int index, struct WordList wl){
	if(index >= wl.len) return NULL;
	return wl.words+(index*(WORDLEN+1));
}
