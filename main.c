/**
 * @file main.c
 *
 * @brief Hlavna vetva
 * @author Vsetci
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"
#include "file_io.h"


// ERRNO.H ? **< edit: not yet, but soon

/**
 * Main funkcia programu
 *
 * @todo Core is missing, at least
 *
 * @param parameter info, expected to be exactly 2
 * @param parameter, expected to include name of file
 * @return Vrati uspesnost vykonania programu
 */
int main( int argc, char *argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Error: No input file specified !\n" );
        return E_OTHER;
    }
	
    E_ERROR_TYPE ret_val;
	char *subor;	/**< abstrakcia zdrojoveho suboru */
	
	ret_val = mmap_file( argv[1], &subor ); 
	
	if ( ret_val != E_OK )
	{
		printf("Nastala chyba\n");
		return ret_val;	
	}
	
	

    /** MAGIC WILL SOON BEGIN TO HAPPEN HERE **/
	
	
    
    
    /** END OF MAGIC **/

    free( subor );
    return ret_val;
}



