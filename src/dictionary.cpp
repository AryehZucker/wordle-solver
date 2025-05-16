#include "dictionary.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>

Dict::Dict(const char *path){
	std::ifstream file(path, std::ios::binary);
	int word_count = 0;
	long filesize;

	if(!file.is_open()){
		std::cerr << "Error opening " << path << ": " << std::strerror(errno) << std::endl;
		len = -1;
		return;
	}

	file.seekg(0, std::ios::end);
	filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	words = new char[filesize];

	for(char *word = words; file >> word; word += WORDLEN+1){
		word_count++;
		if(file.fail() || strlen(word) != 5){
			std::cerr << "Error: data corruption in " << path << std::endl;
			len = -1;
			return;
		}
	}
	file.close();

	len = word_count;
}

Dict::~Dict()
{
	delete[] words;
}

int Dict::getLength() const
{
    return len;
}

//return a pointer to the first letter of the word in the Dict
const char *Dict::getWord(int index) const
{
	if(index >= len) return NULL;
	return words+(index*(WORDLEN+1));
}
