//
// Created by olson on 3/17/2021.
//


//#include "include/main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>


#define LAST_BITS_ON 0x101010101010101UL
#define SIGNIFICANT_BITS_ON 0x8080808080808080UL
#define HALF_BITS_ON_1 0x1000100UL
#define HALF_BITS_ON_2 0x10001UL
#define hassetbyte(v) ((~(v) - 0x0101010101010101UL) & (v) & 0x8080808080808080UL)
#define reduce(v) (((~v - LAST_BITS_ON) ^ ~v) & SIGNIFICANT_BITS_ON) >> 7
#define xnor(t,q) ~(t ^ q) & ~(t ^ LAST_BITS_ON)
#define NO_OF_CHARS 256

union Window {
    uint64_t i;
    unsigned char c[8];
}Window;

union Query {
    uint64_t* i;
    unsigned char* c;
}Query;

union Index {
    uint64_t* i;
    unsigned char* c;
}Index;

union MultiChar {
    uint64_t* i;
    unsigned char* c;
}MultiChar;

union Matches {
    uint64_t i;
    unsigned char c[8];
}Matches;

union Offset {
    uint64_t i;
    int o[8];
}Offset;

union BadChar {
    uint64_t i;
    int o[8];
}BadChar;

int max (int a, int b) { return (a > b)? a: b; }

void badCharheuristic(unsigned char *str, int size, int badchar[NO_OF_CHARS], int last_query) {
    int i;

    for (i = 0; i < NO_OF_CHARS; i++){
        badchar[i] = last_query;
    }

    for (i = 0; i < size; i++){
        badchar[(int) str[i]] = i;
    }
}

int search_test (unsigned char* query_array,
                 int query_len,
                 unsigned char* st,
                 int text_len) {





    int mismatch_move_count = 0;
    int result = 0;
    int last_query_location = query_len;
    int text_section_size = floor(text_len / 9);
    union Offset text_offset;
    text_offset.o[0] = query_len;
    text_offset.o[1] = floor(text_section_size) + (query_len);
    text_offset.o[2] = floor(text_section_size * 2) + (query_len);
    text_offset.o[3] = floor(text_section_size * 3) + (query_len);
    text_offset.o[4] = floor(text_section_size * 4) + (query_len);
    text_offset.o[5] = floor(text_section_size * 5) + (query_len);
    text_offset.o[6] = floor(text_section_size * 6) + (query_len);
    text_offset.o[7] = floor(text_section_size * 7) + (query_len);

    union Offset query_offset;
    query_offset.o[0] = last_query_location;
    query_offset.o[1] = last_query_location;
    query_offset.o[2] = last_query_location;
    query_offset.o[3] = last_query_location;
    query_offset.o[4] = last_query_location;
    query_offset.o[5] = last_query_location;
    query_offset.o[6] = last_query_location;
    query_offset.o[7] = last_query_location;

    int badchar[NO_OF_CHARS];

    badCharheuristic(query_array,query_len,badchar, last_query_location);

//    int stack[text_len];
//    int stack_count = 0;


    unsigned char* char_ptr;

    union Window t_w;
    //t_w.c = &st[0];

    union Index idx;

    union Index last_idx;
    last_idx.i = LAST_BITS_ON * (query_len - 1);

    union Matches query_matches;
    query_matches.i = LAST_BITS_ON;

    union Matches inverse_matches;

    union Matches last_query_match;
    uint64_t value;
    uint64_t match_count;
    uint64_t finished_queries;
    uint64_t finsihed_qeuries_offset;
    uint64_t finished_queries_offset_plus_query_len;
    uint64_t found_queries;

    union Matches match_comparison;

    union BadChar bc;
    bc.i = 0;

    char_ptr = &query_array[0];

    while(text_offset.o[6] <= text_len) {// && *query_matches.i != 0UL) {

        //*last_query_match.i = *query_matches.i;

        //build multimask

        query_matches.c[0] = query_array[query_offset.o[0]];
        query_matches.c[1] = query_array[query_offset.o[1]];
        query_matches.c[2] = query_array[query_offset.o[2]];
        query_matches.c[3] = query_array[query_offset.o[3]];
        query_matches.c[4] = query_array[query_offset.o[4]];
        query_matches.c[5] = query_array[query_offset.o[5]];
        query_matches.c[6] = query_array[query_offset.o[6]];
        query_matches.c[7] = query_array[query_offset.o[7]];

        //build text
        t_w.c[0] = st[text_offset.o[0]];
        t_w.c[1] = st[text_offset.o[1]];
        t_w.c[2] = st[text_offset.o[2]];
        t_w.c[3] = st[text_offset.o[3]];
        t_w.c[4] = st[text_offset.o[4]];
        t_w.c[5] = st[text_offset.o[5]];
        t_w.c[6] = st[text_offset.o[6]];
        if(text_offset.o[7] <= text_len) {
            t_w.c[7] = st[text_offset.o[7]];
        }

        //check multi-mask against text
        value = ~((t_w.i) ^ (query_matches.i));
        //value = xnor(*t_w.i, *query_matches.i);

        //if matches
        if((bool)(query_matches.i = reduce(value))) {
            //decrease matches by 1
            //text_offset.i -= query_matches.i;

            text_offset.o[0] -= query_matches.c[0];
            text_offset.o[1] -= query_matches.c[1];
            text_offset.o[2] -= query_matches.c[2];
            text_offset.o[3] -= query_matches.c[3];
            text_offset.o[4] -= query_matches.c[4];
            text_offset.o[5] -= query_matches.c[5];
            text_offset.o[6] -= query_matches.c[6];
            text_offset.o[7] -= query_matches.c[7];


            //increment query_offset
            //query_offset.i -= query_matches.i;

            query_offset.o[0] -= query_matches.c[0];
            query_offset.o[1] -= query_matches.c[1];
            query_offset.o[2] -= query_matches.c[2];
            query_offset.o[3] -= query_matches.c[3];
            query_offset.o[4] -= query_matches.c[4];
            query_offset.o[5] -= query_matches.c[5];
            query_offset.o[6] -= query_matches.c[6];
            query_offset.o[7] -= query_matches.c[7];

            if((query_matches.i & LAST_BITS_ON) != LAST_BITS_ON) {
                //shift by badchar value
                //to get mismatches
                //invert query_matches then AND with LAST_BITS_ON
                //only mismatches will be shifted
                inverse_matches.i = (~query_matches.i & LAST_BITS_ON);

                bc.o[0] = query_len - badchar[st[text_offset.o[0]]] * inverse_matches.c[0];
                bc.o[1] = query_len - badchar[st[text_offset.o[1]]] * inverse_matches.c[1];
                bc.o[2] = (query_len - badchar[st[text_offset.o[2]]]) * inverse_matches.c[2];
                bc.o[3] = query_len - badchar[st[text_offset.o[3]]] * inverse_matches.c[3];
                bc.o[4] = query_len - badchar[st[text_offset.o[4]]] * inverse_matches.c[4];
                bc.o[5] = query_len - badchar[st[text_offset.o[5]]] * inverse_matches.c[5];
                bc.o[6] = query_len - badchar[st[text_offset.o[6]]] * inverse_matches.c[6];
                bc.o[7] = query_len - badchar[st[text_offset.o[7]]] * inverse_matches.c[7];

                //increment text_offset by badchar value
                //text_offset.i += bc.i;

                text_offset.o[0] += bc.o[0];
                text_offset.o[1] += bc.o[1];
                text_offset.o[2] += bc.o[2];
                text_offset.o[3] += bc.o[3];
                text_offset.o[4] += bc.o[4];
                text_offset.o[5] += bc.o[5];
                text_offset.o[6] += bc.o[6];
                text_offset.o[7] += bc.o[7];


                //increment text_offset by badchar value
                //query_offset.i = bc.i;
                /*
                query_offset.o[0] += bc.o[0];
                query_offset.o[1] += bc.o[1];
                query_offset.o[2] += bc.o[2];
                query_offset.o[3] += bc.o[3];
                query_offset.o[4] += bc.o[4];
                query_offset.o[5] += bc.o[5];
                query_offset.o[6] += bc.o[6];
                query_offset.o[7] += bc.o[7];
                */
            }

        }
        //if mismatches
        else if((query_matches.i & LAST_BITS_ON) != LAST_BITS_ON) {
            //shift by badchar value
            //to get mismatches
            //invert query_matches then AND with LAST_BITS_ON
            //only mismatches will be shifted
            inverse_matches.i = (~query_matches.i & LAST_BITS_ON);

            bc.o[0] = badchar[st[text_offset.o[0]]] * inverse_matches.c[0];
            bc.o[1] = badchar[st[text_offset.o[1]]] * inverse_matches.c[1];
            bc.o[2] = badchar[st[text_offset.o[2]]] * inverse_matches.c[2];
            bc.o[3] = badchar[st[text_offset.o[3]]] * inverse_matches.c[3];
            bc.o[4] = badchar[st[text_offset.o[4]]] * inverse_matches.c[4];
            bc.o[5] = badchar[st[text_offset.o[5]]] * inverse_matches.c[5];
            bc.o[6] = badchar[st[text_offset.o[6]]] * inverse_matches.c[6];
            bc.o[7] = badchar[st[text_offset.o[7]]] * inverse_matches.c[7];

            //increment text_offset by badchar value
            //text_offset.i += bc.i;

            text_offset.o[0] += bc.o[0];
            text_offset.o[1] += bc.o[1];
            text_offset.o[2] += bc.o[2];
            text_offset.o[3] += bc.o[3];
            text_offset.o[4] += bc.o[4];
            text_offset.o[5] += bc.o[5];
            text_offset.o[6] += bc.o[6];
            text_offset.o[7] += bc.o[7];


            //reset query_offset for mismatches
            //query_offset.i = bc.i;
            /*
            query_offset.o[0] = bc.o[0];
            query_offset.o[1] = bc.o[1];
            query_offset.o[2] = bc.o[2];
            query_offset.o[3] = bc.o[3];
            query_offset.o[4] = bc.o[4];
            query_offset.o[5] = bc.o[5];
            query_offset.o[6] = bc.o[6];
            query_offset.o[7] = bc.o[7];
            */
        }
        //if index < 0 for any matches
        //if((bool)(found_queries = reduce(~((query_offset.i) ^ (0 * LAST_BITS_ON))))) {
        if(query_offset.o[0] == -1 || query_offset.o[1] == -1 || query_offset.o[2] == -1 || query_offset.o[3] == -1 ||
           query_offset.o[4] == -1 || query_offset.o[5] == -1 || query_offset.o[6] == -1 || query_offset.o[7] == -1){
            //shift by m+1
            if(query_offset.o[0] == -1){
                result++;
                query_offset.o[0] = query_len;
                text_offset.o[0] += query_len;
            }
            if(query_offset.o[1] == -1){
                result++;
                query_offset.o[1] = query_len;
                text_offset.o[1] += query_len;
            }
            if(query_offset.o[2] == -1){
                result++;
                query_offset.o[2] = query_len;
                text_offset.o[2] += query_len;
            }
            if(query_offset.o[3] == -1){
                result++;
                query_offset.o[3] = query_len;
                text_offset.o[3] += query_len;
            }
            if(query_offset.o[4] == -1){
                result++;
                query_offset.o[4] = query_len;
                text_offset.o[4] += query_len;
            }
            if(query_offset.o[5] == -1){
                result++;
                query_offset.o[5] = query_len;
                text_offset.o[5] += query_len;
            }
            if(query_offset.o[6] == -1){
                result++;
                query_offset.o[6] = query_len;
                text_offset.o[6] += query_len;
            }
            if(query_offset.o[7] == -1){
                result++;
                query_offset.o[7] = query_len;
                text_offset.o[7] += query_len;
            }

            //result += ((uint8_t) (found_queries + found_queries/255));

            //found_queries = found_queries * (query_len + 1);

            //text_offset.i += found_queries;

           //query_offset.i += found_queries;
        }


/*

        //if match is found
        if((bool)(*query_matches.i = reduce(value))) {
            //if pattern is present at current shift
            finished_queries = (*query_offset.i - *query_matches.i) & ~((LAST_BITS_ON * -1) - 1);
            if (finished_queries) {
                //s += (s+m < n)? m-badchar[txt[s+m]] : 1;
                finsihed_qeuries_offset = *text_offset.i & (finished_queries * 255);
                finished_queries_offset_plus_query_len = finsihed_qeuries_offset + (finished_queries * query_len);
                while (finished_queries_offset_plus_query_len > 0) {

                }
            } else {
                //s += max(1, j - badchar[txt[s+j]]);
            }
        } else {
            //s += max(1, j - badchar[txt[s+j]]);
        }
        */
    }
    //We return False if the text is searched and nothing is found.
    return result;

}


