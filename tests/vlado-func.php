<?php
 
function increment_to_milion( $x )
{
    if ($x === 0)
    {
        $t = put_string( "Done\n", $x );
        return 0;
    }
    else
    {
        $t = increment_to_milion( $x - 1 );
    }
}

$a = 20000000;
$x = increment_to_milion($a);