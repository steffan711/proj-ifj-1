<?php

function f($a, $b)
{
    $c = $a * $b;
    return $c;
}

function g()
{
    $x = put_string("g()\n");
    $c = $x / 5;
}
$b = !!false;
$a = true;
$d = 10;
$c = 22.5;
$e = null;
if ( !$b )
{
    $foo = put_string("negacia nam funguje\n");
} 
else
{
    $foo = put_string("negacia nam nefunguje\n");
}
if ( ! ( true && true ) || true and false || false or true )
{
    $c = $b and $a or $b && $a || !!$b;
}
else 
{
    $c = $d and $c;
}
//$z = put_string( f(5,6, 9, 4, g() ), "\n");
$z = put_string( $c, $b, $a,  "\n");
return 5;