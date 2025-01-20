#ifndef DICTIONARY_H
#define DICTIONARY_H

#define WORDLEN 5

struct WordList {
	char *words;
	int len;
};

struct Dict {
	struct WordList answers;
	struct WordList guesses;
};

void loadDict(const char *ans_words_path, const char *guess_words_path);
struct WordList loadWordList(const char *path);
char *getWord(int index, struct WordList wl);

#endif
