#! /bin/bash

head -n $1 dict/guess_words.txt >dict/guess_words-short.txt

if [ $2 ] && [ $2 == "-b" ]; then
	./firstword dict/ans_words.txt dict/guess_words-short.txt &>out.txt &
elif [ $2 ] && [ $2 == "-t" ]; then
	./firstword -t dict/ans_words.txt dict/guess_words-short.txt
else
	./firstword dict/ans_words.txt dict/guess_words-short.txt
fi
