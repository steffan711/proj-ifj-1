/**
 * @file built_in.c
 *
 * @brief Vsetky vstavane funkcie, ktore musi interpret podporovat
 * @author Filip
 */


#include "ial.h"


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