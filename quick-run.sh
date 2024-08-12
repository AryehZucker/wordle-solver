#! /bin/bash

ANS_WORDS=dict/ans_words-shuffle.txt
GUESS_WORDS=dict/guess_words-shuffle.txt
GUESS_WORDS_SHORT=dict/guess_words-short-shuffle.txt

head -n $1 $GUESS_WORDS >$GUESS_WORDS_SHORT

if [ $2 ] && [ $2 == "-b" ]; then
	./firstword $ANS_WORDS $GUESS_WORDS_SHORT &>out.txt &
elif [ $2 ] && [ $2 == "-t" ]; then
	./firstword -t $ANS_WORDS $GUESS_WORDS_SHORT
else
	./firstword $ANS_WORDS $GUESS_WORDS_SHORT
fi
