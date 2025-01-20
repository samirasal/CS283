#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void word_print(char *, int);

int setup_buff(char *buff, char *user_str, int len) {
    int i = 0; 
    int j = 0; 
    int white_spaces = 0; 

    while (*(user_str + j) != '\0' && i < len) {
        if (*(user_str + j) != ' ' && *(user_str + j) != '\t') {
            *(buff + i) = *(user_str + j);
            i++;
            white_spaces = 0;
        } else if (!white_spaces) {
            *(buff + i) = ' ';
            i++;
            white_spaces = 1;
        }
        j++;
    }

    if (*(user_str + j) != '\0') {
        return -1; 
    }

    while (i < len) {
        *(buff + i) = '.';
        i++;
    }

    return j; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }

    printf("Word Count: %d\n", count);
    return count;
}
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void reverse_string(char *buff, int str_len) {
    char temp;
    for (int i = 0; i < str_len / 2; i++) {
        temp = *(buff + i);
        *(buff + i) = *(buff + str_len - i - 1);
        *(buff + str_len - i - 1) = temp;
    }

    printf("Reversed String: ");
    for (int i = 0; i < str_len; i++) {
        putchar(*(buff + i));
    }
    putchar('\n');
}
void word_print(char *buff, int str_len) {
    int word_start = 0, word_length = 0, word_count = 0;

    printf("Word Print\n----------\n");

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (word_length == 0) {
                word_start = i;
            }
            word_length++;
        } else if (word_length > 0) {
            word_count++;
            printf("%d. ", word_count);
            for (int j = word_start; j < word_start + word_length; j++) {
                putchar(*(buff + j));
            }
            printf(" (%d)\n", word_length);
            word_length = 0;
        }
    }


    // Handle the last word if it ends at the end of the string
    if (word_length > 0) {
        word_count++;
        printf("%d. ", word_count);
        for (int j = word_start; j < word_start + word_length; j++) {
            putchar(*(buff + j));
        }
        printf(" (%d)\n", word_length);
    }
}


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    // This ensures argv[1] exists and starts with '-', preventing invalid memory access.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // This ensures that the required string argument is provided for operations.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        fprintf("Error setting up buffer, error = %d", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len); //you need to implement
            break;
        case 'r':
            reverse_string(buff, user_str_len); 
            break;
        case 'w':
            word_print(buff, user_str_len);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
   free(buff);
    return 0;
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
//          It prevents buffer overflows, 
//          allows functions to be reused with different buffer sizes, 
//          and makes the code easier to read and maintain.
