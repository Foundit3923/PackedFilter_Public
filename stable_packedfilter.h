//
// Created by michael on 2/13/2024.
//

//#include "include/main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>


#define LAST_BITS_ON 0x101010101010101UL
#define LAST_BITS_NOT_ON 0xFEFEFEFEFEFEFEFEUL
#define SIGNIFICANT_BITS_ON 0x8080808080808080UL
#define SIGNIFICANT_BITS_NOT_ON 0x7F7F7F7F7F7F7F7FUL
#define ALL_ON 0XFFFFFFFFFFFFFFFFUL

//Compares a character (c) to every character in (t). Results in a fully set byte when there is a match. Fully set byte is at match location.
#define xnor(t,q,c) t ^ q * (c) // & (t ^ LAST_BITS_ON * (c))

//Reduces fully set bytes to a byte with a 1 in the LSB position, non fully set bytes are reudced to 0
//#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7 //Original Method
//#define reduce(t,q,c) (~(xnor(t,q,c) + LAST_BITS_ON) / 128) & LAST_BITS_ON

//Counts the number of set bits in reduced integer
#define bitcount(q) (q + q/255) & 255

//Keeps track of found matches
#define count(v) result += bitcount(v)

//Compares and reduces the comparison of a character (c) to every character in (t). Results in a fully set byte reduced to a byte with the LSB position on. Non-fully set bytes are 0
//(((LAST_BITS_NOT_ON - x) ^ x) >> 7) & LAST_BITS_ON
//#define reduce(x) (((LAST_BITS_NOT_ON - x) ^ x) >> 7) & LAST_BITS_ON//(((x - LAST_BITS_ON) ^ x) & SIGNIFICANT_BITS_ON) >> 7 
//#define reduce_xnor(t,q,c) ((LAST_BITS_NOT_ON - ~(t ^ q * (c))) / 128) & LAST_BITS_ON
//#define reduce_xnor(t,q,c) ((LAST_BITS_ON*(127+(c))-((t)&LAST_BITS_ON*127))&~(t)&LAST_BITS_ON*128)
#define reduce_xnor(t,q,c) ((((t)&LAST_BITS_ON*127)+LAST_BITS_ON*(127-(c))|(t))&LAST_BITS_ON*128)

//Unused
#define distance(x) ffsll(x >> 1) / 8

union Window {
    uint64_t* i;
    unsigned char* c;
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
};

union Test {
    char* c;
    //char l[8];
    uint64_t i;
};

int search_test (unsigned char* query_array,
            int query_len,
            unsigned char* text,
            int text_len) {

    //Setup
    int result = 0;
    int text_offset = 0;
    int len_check = text_len-1;

    unsigned char* char_ptr = &query_array[0];
    unsigned char* last_char = &query_array[query_len-1];
    unsigned char* first_char = &query_array[0];
    
    union Window text_window;
    text_window.c = &text[0];

    uint64_t query_matches = LAST_BITS_ON;
    uint64_t value;
    
    //While the address of the first char of text_window.c is not the address of the last char of the text. 
    while(!(&*text_window.c > &text[len_check])) {
        //Check if *char_ptr matches any chars in *text_window  
        //reduce any found matches to a single bit occupying the lsb position of a byte
        //compare query_matches with reduce to only keep desired matches
        //value = xnor(*text_window.i, query_matches, *char_ptr);
        if((bool)(query_matches = reduce_xnor(*text_window.i, query_matches, *char_ptr) & query_matches)) {
            if (&char_ptr[0] == last_char) {
                count(query_matches);
                char_ptr = first_char;
            } else {
                //Character match found: move to next char in text and query
                char_ptr++;
                text_window.c++;
            }
        } else {
            //No match found in window: move window and reset
            char_ptr = first_char;
            text_offset += 8;
            text_window.c = &text[text_offset];
            query_matches = LAST_BITS_ON;
        }
    }
    //Return False if the text is searched and nothing is found.
    return result;
}


