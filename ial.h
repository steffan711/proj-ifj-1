/**
 * @file ial.h
 *
 * @brief Hlavickovy subor pre funkcie zadane z predmetu IAL
 * @author Filip
 */

#include "types.h"
#include <stdbool.h>

void quicksort( char *input, int left, int right );
int kmpmatch( const char *text, int text_size, const char *pattern, int pattern_size );