<?php
function stringy( $str )
{
    $ret = strlen( $str );
    if ( $ret > 100000 )
    {
        $str = $str . $str ;
    }
    else
    {
        $ret = stringy( $str . "a" );
    }
}

$str = "a";
$ret = stringy( $str );

