#ifndef DICTIONARY_H
#define DICTIONARY_H

#define WORDLEN 5

class Dict {
private:
	char *words;
	int len;
public:
	Dict(const char *path);
	~Dict();
	int getLength() const;
	const char *getWord(int index) const;
};

#endif
