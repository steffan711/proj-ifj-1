<?php

function f($a, $b)
{
    $c = $a * $b;
    return $a;
}

function g()
{
    $x = put_string("g()\n");
    $c = $x / 0;
}

$z = put_string( f(5,6, 9, 4, g() ), "\n");