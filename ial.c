/**
 * @file ial.c
 *
 * @brief Funkcie algoritmov pre predmet IAL
 * @author Filip
 */


#include "ial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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
	if( pattern_size == 0 )
        return 0;

	int m = 0, i = 0, Fail[pattern_size + 1], pos = 2, cnd = 0;	
	Fail[0] = -1;
	Fail[1] = 0;
	
	while( pos < pattern_size )
	{
		if( pattern[pos - 1] == pattern[cnd] )
			Fail[pos++] = ++cnd;
		else if( cnd > 0 )
			cnd = Fail[cnd];
		else
			Fail[pos++] = 0;
	}
	
	while( ( m + i ) < text_size )
	{
		if( pattern[i] == text[m + i] )
		{
			if( i == ( pattern_size - 1 ) )
				return m;
			i++;
		}
		else
		{
			m += i - Fail[i];
			if( Fail[i] > -1 )
				i = Fail[i];
			else
				i = 0;
		}
	}
	
	return -1;
}