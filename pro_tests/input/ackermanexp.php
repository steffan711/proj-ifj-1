<?php
function ackermann( $m , $n )
{
    $x = 4;
    $y = 2;
    $z = 14;
    $foo = "lajflajf";
    $boo = "afljadjfj";
    
    if ( $m===0 )
    {
        return $n + $y * $x + $x * $y - 4 - $x - $x - 3;
    }
    elseif ( $n===0 )
    {
        return ackermann( $m - $y * $x + $x * $y - 4 - $x - $x - 3 + $z , 1 );
    }
    return ackermann( $m - $y * $x + $x * $y - 4 - $x - $x - 3 + $z, ackermann( $m , $n - $y * $x + $x * $y - 4 - $x - $x - 3 + $z ) );
}

$p = put_string(ackermann( 3, 10 ), "\n");





