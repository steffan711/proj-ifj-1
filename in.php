<?php

$x = put_string("Zadejte cislo pro vypocet faktorialu\n");
$a = 1;

while ($a !== 0)
{
    if($a === 10000000)
    {
        $b = "hodnota \$a je ". $a ;
        $x = put_string ( $b );
        return 5;
    }
    $a = $a + 1;
}
