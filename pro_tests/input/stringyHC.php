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

function a($a)
{
    return $a;
}

$str = "a"."b";
$str = a($str);
$x = put_string($str);
$ret = stringy( $str );

