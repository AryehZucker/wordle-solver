#include "headers.h"

Dict words;

void loadDict(const char *ans_words_path, const char *guess_words_path){
	words.answers = loadWordList(ans_words_path);
	shuffleWordList(words.answers);
	
	words.guesses = loadWordList(guess_words_path);
	shuffleWordList(words.guesses);
}

WordList loadWordList(const char *path){
	FILE *fp;
	WordList wl;
	int n, word_count = 0;
	long filesize;

	fp = fopen(path, "r");
	if(fp == NULL){
		char message[100];
		sprintf(message, "Error opening %s", path);
		perror(message);
		wl.len = -1;
		return wl;
	}

	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);

	wl.words = (char *) malloc(filesize);

	//note: "%5c\n" is 5 because WORDLEN = 5. if WORDLEN changes, change 5 to match.
	for(char *word = wl.words; (n = fscanf(fp, "%5c\n", word)) == 1; word += WORDLEN+1){
		word_count++;
		word[WORDLEN] = '\0';
	}
	fclose(fp);
	if(n == 0){
		fprintf(stderr, "Error: data corruption in %s\n", path);
		wl.len = -1;
		return wl;
	}

	wl.len = word_count;

	return wl;
}

void shuffleWordList(WordList wl){
	int index;
	
	srand(time(NULL));
	
	for(int i=0; i<wl.len; i++){
		index = rand() % (wl.len - i) + i; //pick a random word from what's left
		swapWords(getWord(i, wl), getWord(index, wl));
	}
}

void swapWords(char *word1, char *word2){
	char temp[WORDLEN+1];
	strncpy(temp, word1, WORDLEN);
	strncpy(word1, word2, WORDLEN);
	strcpy(word2, temp);
}

//return a pointer to the first letter of the word in the WordList
char *getWord(int index, WordList wl){
	if(index >= wl.len) return NULL;
	return wl.words+(index*(WORDLEN+1));
}
