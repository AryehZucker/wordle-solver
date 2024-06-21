/*Algorithm:
    Store information by letter.
    Process information told to us (i.e. directly given by the score).
    Process inferred information repeatedly until there are no more inferences to be made.

    Information on a letter:
        amount:    at least how many of the letter are in the word

        capped:    1) we are told it's capped - we have a gray
                   2) we know all 5 letters (sum of amounts == 5)
                   3) there are no more spots for this letter to go

        known_pos: 1) we see a green
                   2) there are just enough possible spots for the amount

        bad_pos:   1) it's yellow or gray
                   2) if we know everything about this letter,
                      i.e. the amount is capped and we know all the positions for this letter,
                      then all the rest are bad (i.e. bad_pos == known_pos)
                   3) any spot that must have some other letter there
                      (eg: L='a', 'b'.ppos:10001, 'c'.ppos:'10001'; first and last spots must have a&c)
                      (eg: L='a', there's a green 'b' in the first spot)
*/

/*Elims Tree structure/lookup:
 *	tree[data_size] points to a binary tree of hashed data and elims
 *If a data entry that you're looking for doesn't exist, create it.
 */

#include "headers.h"


extern Dict words;
DataA *answers_data;

void wordToData(const char *word, DataA *data){
	static DataLA letter_data[26];
	DataLA *ldataptr;

	//initialize data to none
	for(int i=0; i<26; i++)
		letter_data[i].amount = letter_data[i].pos = 0;

	//mark all the positions of each letter
	for(int i=0; i<5; i++){
		letter_data[word[i]-'a'].amount++;
		letter_data[word[i]-'a'].pos |= 1<<i;
	}

	//load all non-empty data into "data"
	data->letters = 0;
	ldataptr = data->letter_data;
	for(int i=0; i<26; i++){
		if(letter_data[i].amount != 0){
			data->letters |= 1<<i;
			*ldataptr++ = letter_data[i];
		}
	}
}


//generate a list of DataS, where the positions correspond to a word
void genDataTable(const char *ans, WordList guesses, DataS table[]){
	for(int i=0; i < guesses.len; i++)
		genData(getWord(i, guesses), ans, table+i);
}

//generate the data for the given guess and ans
void genData(const char *guess, const char *ans, DataS *data){
	char letter;
	static DataL letter_data[26];
	DataL *ldataptr;
	int guess_count, ans_count;

	data->bad_letters = 0;

	//initialize data to none
	for(int i=0; i<26; i++){
		letter_data[i].amount = letter_data[i].known_pos = 0;
		letter_data[i].bad_pos = 0x1F;
	}

	for(int i=0; i<5; i++){
		//find all "known_pos"
		if((letter = guess[i]) == ans[i])
			letter_data[letter-'a'].known_pos |= 1<<i; //turn on the i-th bit

		//find all "bad_pos"
		else
			letter_data[letter-'a'].bad_pos &= ~(1<<i); //turn off the i-th bit
	}

	for(int i=0; i<26; i++){
		//count how many letter "i"s are in guess and answer
		guess_count = ans_count = 0;
		for(int j=0; j<5; j++){
			if(guess[j] == ('a'+i)) guess_count++;
			if(  ans[j] == ('a'+i))   ans_count++;
		}

		//find "amount"
		letter_data[i].amount = min(guess_count, ans_count);
		//find "capped"
		if(guess_count > letter_data[i].amount)
			if(ans_count == 0)	//the letter is not in the ans
				data->bad_letters |= 1<<i;
			else letter_data[i].amount |= CAPPED;
	}

	//load all non-empty data into "data"
	data->letters = 0;
	ldataptr = data->letter_data;
	for(int i=0; i<26; i++){
		if(letter_data[i].amount != 0){
			data->letters |= 1<<i;
			*ldataptr++ = letter_data[i];
		}
	}

	simplify(data->letter_data, weight(data->letters));
}




int getComboElims(DataS *data1, DataS *data2, Node *tree[]){
	int *elimsptr;
	static unsigned int data_hash[HASH_LEN];
	DataC combo_data;

	combine(data1, data2, &combo_data);
	hashData(&combo_data, data_hash);
	timeEnd(INIT); //logging

	elimsptr = searchTree(data_hash, weight(combo_data.letters), tree);
	timeEnd(SEARCH); //logging
	logLookup(*elimsptr != EMPTY); //logging

	if(*elimsptr == EMPTY)	//no entry for this data
		*elimsptr = countElims(&combo_data);

	timeEnd(COUNT); //logging

	return *elimsptr;
}


//combine 'data1' and 'data2' into one data object, 'combo_data'
void combine(const DataS *data1, const DataS *data2, DataC *combo_data){
	unsigned int d1_letters, d2_letters, c_letters;
	//pointers to letter_data that we're up to
	DataL *combo_ldp;
	const DataL *d1_ldp, *d2_ldp;

	//combine good and bad letters
	combo_data->letters = data1->letters | data2->letters;
	combo_data->bad_letters = data1->bad_letters | data2->bad_letters;

	//keep track of which good letters are in which data
	d1_letters = data1->letters & ~data2->letters;	//letters only found in data1
	d2_letters = data2->letters & ~data1->letters;	//letters only found in data2
	c_letters  = data1->letters &  data2->letters;	//letters found in both

	d1_ldp    = data1->letter_data;
	d2_ldp    = data2->letter_data;
	combo_ldp = combo_data->letter_data;

	//combine letter_data
	while(d1_letters || d2_letters || c_letters){
		if(c_letters & 1){
			//this automatically acounts for CAPPED
			combo_ldp->amount = max(d1_ldp->amount, d2_ldp->amount);
			combo_ldp->known_pos = d1_ldp->known_pos | d2_ldp->known_pos;
			combo_ldp->bad_pos = d1_ldp->bad_pos & d2_ldp->bad_pos;
			d1_ldp++, d2_ldp++, combo_ldp++;
		}
		else if(d1_letters & 1)
			*combo_ldp++ = *d1_ldp++;
		else if(d2_letters & 1)
			*combo_ldp++ = *d2_ldp++;

		d1_letters >>= 1;
        	d2_letters >>= 1;
	        c_letters  >>= 1;
	}

        simplify(combo_data->letter_data, weight(combo_data->letters));
}


int *searchTree(const unsigned int *data_hash, const int len, Node *tree[]){
	int i;
	register Node **nodeptr = &tree[len]; //a pointer to where the node is held in the tree

	while(*nodeptr != NULL){
		for(i=0; data_hash[i] == (*nodeptr)->hash[i]; i++){
			if(i == HASH_LEN-1)	//the entire hash has been compared and matches
				return &(*nodeptr)->elims;
		}
		nodeptr = (data_hash[i]<(*nodeptr)->hash[i]) ? &(*nodeptr)->left : &(*nodeptr)->right;
	}

	*nodeptr = malloc(sizeof (Node));
	for(i=0; i<HASH_LEN; i++) (*nodeptr)->hash[i] = data_hash[i];
	(*nodeptr)->elims = EMPTY;
	(*nodeptr)->left = (*nodeptr)->right = NULL;

	return &(*nodeptr)->elims;
}




void init_ans_data(){
	answers_data = (DataA *) malloc((words.answers.len+1) * sizeof (DataA));
	for(int i=0; i<words.answers.len; i++)
		wordToData(getWord(i, words.answers), &answers_data[i]);
	answers_data[words.answers.len].letters = 0;
}

int countElims(const DataC *data){
	DataA *ans_data = answers_data;
	int elims = 0;
	while(ans_data->letters){
		if(!fits(data, ans_data++))
			elims++;
	}
	return elims;
}


int fits(const DataC *guess_data, const DataA *ans_data){
	//test that the letters don't contradict
	if(guess_data->letters & ~ans_data->letters || guess_data->bad_letters & ans_data->letters)
		return 0;

	else {
		const DataL *gldp  = guess_data->letter_data;	//pointer to the guess letter_data that we're upto
		const DataLA *aldp =   ans_data->letter_data;	//pointer to the answer letter_data that we're upto

		int guess_letters = guess_data->letters,
		    ans_letters   =   ans_data->letters;

		//loop through all the letters in guess data
		while(guess_letters){
			if(guess_letters & 1){	//the bit for this letter is on, meaning there is data for this letter
				//check amount
	                        if(aldp->amount < (gldp->amount & ~CAPPED))
					return 0;

				//check capped
	                        if(gldp->amount & CAPPED && aldp->amount != (gldp->amount & ~CAPPED))
                 	                return 0;

                 	        //check known_pos
                        	if(~aldp->pos & gldp->known_pos) return 0;

                	        //check bad_pos
        	                if(aldp->pos & ~gldp->bad_pos) return 0;

        	                gldp++, aldp++;
			}
			else if(ans_letters & 1)	//the bit for this letter is on
				aldp++;

			guess_letters >>= 1;
			ans_letters >>= 1;
		}
	}

	return 1;
}


void simplify(DataL *letter_data, const int data_len){
	int i, changed, total_amount, taken = 0;

	do{
		changed = 0;

		//count total known letters
		total_amount = 0;
		for(i=0; i<data_len; i++)
			total_amount += letter_data[i].amount & ~CAPPED;

		for(i=0; i<data_len; i++){
			//capped:3 & known_pos:2
			if((letter_data[i].amount & ~CAPPED) == weight(letter_data[i].bad_pos)){
				if( !(letter_data[i].amount & CAPPED)){
					letter_data[i].amount |= CAPPED;
					changed = 1;
				}
				if(letter_data[i].known_pos != letter_data[i].bad_pos){
					letter_data[i].known_pos = letter_data[i].bad_pos;
					changed = 1;
				}
			}
			//capped:2
			else if(total_amount == 5){
				if( !(letter_data[i].amount & CAPPED)){
					letter_data[i].amount |= CAPPED;
					changed = 1;
				}
			}

			//bad_pos:2
			if(letter_data[i].amount == (weight(letter_data[i].known_pos)|CAPPED))
				if(letter_data[i].bad_pos != letter_data[i].known_pos){
					letter_data[i].bad_pos = letter_data[i].known_pos;
					changed = 1;
				}

			//mark all taken positions
			taken |= letter_data[i].known_pos;
		}

		//bad_pos:3
		for(int i=0; i<data_len; i++)
			if(letter_data[i].bad_pos & taken & ~letter_data[i].known_pos){
				letter_data[i].bad_pos &= ~taken | letter_data[i].known_pos;
				changed = 1;
			}
	} while(changed);

	/*i choose not to draw inferences regarding the second instance of bad_pos:3
	 *this will not affect whether data fits a word (and so is of no concern),
	 *it will only affect considering data as equal
	 */
}




void hashData(const DataC *data, unsigned int *buffer){
	int i, len = weight(data->letters);
	unsigned int tmp;

	//inefficient!!
	//initialize the hash to null data (0 is null because amount can't be 0)
	for(i=0; i<HASH_LEN; i++)
		buffer[i] = 0;

	//which is better to go first? which is more commonly diff?
	*buffer++ = data->bad_letters;
	*buffer++ = data->letters;

	//multiple data can be combined into one int
	for(i=0; i<len; i++){
		tmp = data->letter_data[i].amount << 10;
		tmp |= data->letter_data[i].known_pos << 5;
		tmp |= data->letter_data[i].bad_pos;

		if(i%2 == 0){
			*buffer = tmp << 16;
		}
		else {
			*buffer++ |= tmp;
		}
	}
}
