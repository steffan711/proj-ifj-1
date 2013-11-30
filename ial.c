/**
 * @file ial.c
 *
 * @brief Funkcie algoritmov pre predmet IAL
 * @author Filip
 */


#include "ial.h"
#include <stdlib.h>
#include <string.h>


/**
 * Pomocna funkcia, implementacia quicksortu
 *
 * @param retazec
 * @param leva medz
 * @param prava medz
 * @return void
 */
void quicksort( char *input, int beg, int end )
{
	int i = beg, j = end;
	char pom, PM = input[( beg + end ) / 2];
	
	do
	{
		while( input[i] < PM )
			i++;
		while( input[j] > PM )
			j--;
		if( i <= j )
		{
			pom = input[i];
			input[i] = input[j];
			input[j] = pom;
			i++;
			j--;
		}
		
	} while( i <= j );
	
	if( beg < j ) quicksort( input, beg, j );
	if( i < end ) quicksort( input, i, end );
}


/**
 * Pomocna funkcia, implementacia Knuth Morris Prattovho algoritmu
 *
 * @param retazec v ktorom sa hlada
 * @param hladany retazec
 * @return pozicia, -1 -> nenajdene, ina chyba nemoze nastat
 */
int kmpmatch( const char *text, int text_size, const char *pattern, int pattern_size )
{
    int Fail[pattern_size + 1];
    int i, j, result = -1;

    if( pattern_size == 0 )
        return 0;

    Fail[0] = -1;
    for ( i = 0; i != ( pattern_size - 1 ); i++ )
	{
        Fail[i + 1] = Fail[i] + 1;
        while( Fail[i + 1] > 0 && pattern[i] != pattern[Fail[i + 1] - 1] )
            Fail[i + 1] = Fail[Fail[i + 1] - 1] + 1;
    }

	i = j = 0;
    while( i != text_size )
	{
        if( j < 0 || text[i] == pattern[j] )
		{
            ++i, ++j;
            if( j == ( pattern_size - 1 ) )
			{
				result = i - j;
                break;
            }
        }
        else
			j = Fail[j];
    }

    return result;
}