#! /bin/bash

ANS_WORDS=dict/ans_words-shuffle.txt
GUESS_WORDS=dict/guess_words-shuffle.txt

if [ $1 ] && [ $1 == "-b" ]; then
	./firstword $ANS_WORDS $GUESS_WORDS &>out.txt &
elif [ $1 ] && [ $1 == "-t" ]; then
	./firstword -t $ANS_WORDS $GUESS_WORDS
else
	./firstword $ANS_WORDS $GUESS_WORDS
fi
