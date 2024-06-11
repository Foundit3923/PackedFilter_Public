#include "stringzilla.h"
int simd_search (char* init_query, int query_len, char* init_term, int text_len) {
    sz_string_view_t haystack = {init_term, text_len};
    sz_string_view_t needle = {init_query, query_len};

    //sz_size_t substring_position = (long long unsigned int) sz_find(haystack.start, haystack.length, needle.start, needle.length);

    if(sz_find(haystack.start, haystack.length, needle.start, needle.length)){
      return 1;
    }

    return 0;
}