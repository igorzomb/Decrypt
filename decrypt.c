#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
//#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#pragma warning(disable : 4996)
#pragma warning(disable : 6031)
#pragma warning(disable : 6262)

#define MAX 10240
#define MAX_WORD_COUNT 60000                //we have less than 60000 words
#define MAX_WORD_LENGTH 80                //each word is less than 80 letters

//Using these two global variables can be justified :)
char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];        //2-d array to hold all the words
int word_count = 0;                    //number of words, initilized to 0

//Note the words in the dictionary file are sorted
//This fact could be useful
void read_file_to_array(char *filename)
{
    FILE *fp;

    //open the file for reading
    fp = fopen(filename, "r");
    if(fp==NULL)
    {
        printf("Cannot open file %s.\n", filename);
        exit(-1);
    }
    //make sure when each word is saved in the array words,
    //it does not ends with a '\n'
    //see how this is done using fscanf 
    while(!feof(fp))
        fscanf(fp, "%s\n", words[word_count++]);
    fclose(fp);
}

//TODO
//Test wether a string word is in the dictionary
//Return 1 if word is in the dictionary
//Return 0 otherwise
//Be efficient in implementing this function
//Efficiency is needed to pass test cases in limited time
int in_dict(char *word)
{
    int low = 0;
    int high = MAX_WORD_COUNT - 1;
    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(word, words[mid]);

        if (cmp == 0)
            return 1;
        else if (cmp < 0)
            high = mid - 1;
        else low = mid + 1;
    }

    return 0;
}

//TODO
//Use key and shift to decrypt the encrypted message
//len is the length of the encrypted message
//Note the encrypted message is stored as an array of integers (not chars)
//The result is in decrypted

int encrypt1(unsigned char key, unsigned char shift, char in1)
{
    int out1 = (in1 << shift) ^ key;
    return out1;
}

char decrypt1(unsigned char key, unsigned char shift, int int1)
{
    int input = int1;
    int step1 = input ^ key;
    int step2 = step1 >> shift;

    return (char)step2;
}

void decryption(unsigned char key, unsigned char shift, const int *encrypted, int len, char *decrypted)
{
    for (int i = 0; i < len; i++)
    {
        int input = *(encrypted + i);
        int step1 = input ^ key;
        int myChar = step1 >> shift;
        *(decrypted + i) = myChar;
    }
}

//TODO
//calculate a score for a message msg
//the score is used to determine whether msg is the original message
int message_score(const char *msg)
{
    int score = 0;
    char temp[MAX];
    memcpy(temp, msg, MAX);

    char* token = strtok(temp, " ");
    // loop through the string to extract all other tokens
    while (token != NULL) {
        if (in_dict(token))
            score++;
        token = strtok(NULL, " ");
    }

    return score;
}



//search using all the (key, shift) combinations
//to find the original message
//result is saved in message
void search(const int *encrypted, int len, char *message)
{
	char decrypted[MAX];

    int max_score = 0;
    strcpy(message, "");
    for(unsigned char k = 0; k < 255; k++)
    {
        for(unsigned char shift = 0; shift <= 24; shift++)
        {
            decryption(k, shift, encrypted, len, decrypted);
			int score = message_score(decrypted);
			if(score > max_score)
			{	
				max_score = score;
                strcpy(message, decrypted);
                //char* newline = strtok(decrypted, "\n");
                //while (newline != NULL) {
                //    strcat(message, newline);
                //    strcat(message, "\n");
                //    newline = strtok(NULL, "\n");
                //}
                message[len] = 0;

			}
        }
    }
}

//TODO
//read the encrypted message from the file to encrypted
//return number of bytes read
int read_encrypted(char *filename, int *encrypted)
{
    FILE* file;
    file = fopen(filename, "rb");
    size_t bytesRead;
    int oneInt;
    int len = 0;

    while ((bytesRead = fread(&oneInt, 1, sizeof(oneInt), file)) > 0) {
        *(encrypted + len) = oneInt;
        len++;
    }

    fclose(file);

    return len;
}


//Do not change the main() function
int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("%s encrypted-message\n", argv[0]);
		return 0;
	}

	read_file_to_array("dict.txt");

    //int should_be_one_1 = in_dict("abate");
    //int should_be_zero_1 = in_dict("igor");
    //int should_be_one_2 = in_dict("miscellaneous");

    char test1 = '\'';
    unsigned char k = 100;
    unsigned char shift = 22;
    int t1 = encrypt1(k, shift, test1);
    char back1 = decrypt1(k, shift, t1);

	int encrypted[MAX];
	int len = read_encrypted(argv[1], encrypted);
	
	char message[MAX];
	strcpy(message, "");
	search(encrypted, len, message);
	printf("%s\n", message);
	return 0;
}
