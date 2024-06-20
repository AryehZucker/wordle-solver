#! /bin/bash

if [ $1 ] && [ $1 == "-b" ]; then
	./firstword dict/ans_words.txt dict/guess_words.txt &>out.txt &
elif [ $1 ] && [ $1 == "-t" ]; then
	./firstword -t dict/ans_words.txt dict/guess_words.txt
else
	./firstword dict/ans_words.txt dict/guess_words.txt
fi
