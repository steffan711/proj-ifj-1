<?php
$a = 1;
$b = 1;
$c = 1;

function dummy( $param )
{
    return $param + 1;
}

$foo = dummy( $a, dummy($b), dummy($c));
$foo = dummy( $foo ); 
$foo = $foo;
$p = put_string($foo, $a, $b, $c, "\n");
$p = put_string("Jou\n");