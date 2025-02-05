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

#include "tables.hpp"
#include "dictionary.hpp"
#include "feedback.hpp"
#include "utils.hpp"


extern struct Dict words;
struct DataA *answers_data;

void wordToData(const char *word, struct DataA *data){
	static struct DataLA letter_data[26];
	struct DataLA *ldataptr;

	//initialize data to none
	for(int i=0; i<26; i++)
		letter_data[i].amount = letter_data[i].pos = 0;

	//mark all the positions of each letter
	for(int i=0; i<WORDLEN; i++){
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
void genDataTable(const char *ans, struct WordList guesses, struct DataS table[]){
	for(int i=0; i < guesses.len; i++)
		genData(getWord(i, guesses), ans, table+i);
}

//generate the data for the given guess and ans
void genData(const char *guess, const char *ans, struct DataS *data){
	char letter;
	static struct DataL letter_data[26];
	struct DataL *ldataptr;
	int guess_count, ans_count;

	data->bad_letters = 0;

	//initialize data to none
	for(int i=0; i<26; i++){
		letter_data[i].amount = letter_data[i].known_pos = 0;
		letter_data[i].bad_pos = 0x1F;
	}

	for(int i=0; i<WORDLEN; i++){
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
		for(int j=0; j<WORDLEN; j++){
			if(guess[j] == ('a'+i)) guess_count++;
			if(  ans[j] == ('a'+i))   ans_count++;
		}

		//find "amount"
		letter_data[i].amount = MIN(guess_count, ans_count);
		//find "capped"
		if(guess_count > letter_data[i].amount){
			if(ans_count == 0)	//the letter is not in the ans
				data->bad_letters |= 1<<i;
			else letter_data[i].amount |= CAPPED;
		}
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




struct Tree {
	Node *BST[WORDLEN+1];

	Tree() {
		for(int i=0; i<6; i++) BST[i] = NULL;
	}

	~Tree() {
		deleteTree(BST, 6);
	}
};

int getElims(const Feedback &feedback){
	static Tree elims_cache;

	int *elimsptr = searchTree(feedback, elims_cache.BST);

	if(*elimsptr == EMPTY)	//no entry for this data
		*elimsptr = countElims(&feedback.data);

	return *elimsptr;
}


int *searchTree(const Feedback &feedback, struct Node *tree[]){
	int len = weight(feedback.data.letters);
	struct Node **nodeptr = &tree[len]; //a pointer to where the node is held in the tree

	while(*nodeptr != NULL){
		int cmp = Feedback::compare(feedback, *(*nodeptr)->feedback);
		if (cmp == 0)
			return &(*nodeptr)->elims;
		nodeptr = (cmp < 0) ? &(*nodeptr)->left : &(*nodeptr)->right;
	}

	*nodeptr = new struct Node;
	(*nodeptr)->feedback = new Feedback(feedback);
	(*nodeptr)->elims = EMPTY;
	(*nodeptr)->left = (*nodeptr)->right = NULL;

	return &(*nodeptr)->elims;
}


void deleteNode(struct Node *node){
	if(node == NULL)
		return;
	
	deleteNode(node->left);
	deleteNode(node->right);
	delete node->feedback;
	delete node;
}

void deleteTree(struct Node *tree[], int size){
	for(int i=0; i<size; i++)
		deleteNode(tree[i]);
}




void initAnsToDataTable(struct Dict words){
	answers_data = new struct DataA[words.answers.len+1];
	for(int i=0; i<words.answers.len; i++)
		wordToData(getWord(i, words.answers), &answers_data[i]);
	answers_data[words.answers.len].letters = 0;
}

void delAnsToDataTable(){
	delete answers_data;
}

int countElims(const struct DataC *data){
	struct DataA *ans_data = answers_data;
	int eliminations = 0;
	while(ans_data->letters){
		if(!fits(data, ans_data++))
			eliminations++;
	}
	return eliminations;
}


int fits(const struct DataC *guess_data, const struct DataA *ans_data){
	//test that the letters don't contradict
	if(guess_data->letters & ~ans_data->letters || guess_data->bad_letters & ans_data->letters)
		return 0;

	else {
		const struct DataL *gldp  = guess_data->letter_data;	//pointer to the guess letter_data that we're upto
		const struct DataLA *aldp =   ans_data->letter_data;	//pointer to the answer letter_data that we're upto

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


void simplify(struct DataL *letter_data, const int data_len){
	int i, changed, total_amount, taken = 0;

	do{
		changed = 0;

		//count total known letters
		total_amount = 0;
		for(i=0; i<data_len; i++)
			total_amount += letter_data[i].amount & ~CAPPED;

		for(i=0; i<data_len; i++){
			//capped:3 & known_pos:2
			if((letter_data[i].amount & ~CAPPED) == (unsigned int) weight(letter_data[i].bad_pos)){
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
			else if(total_amount == WORDLEN){
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
