<?php

$x = put_string("Zadejte cislo pro vypocet faktorialu\n");
$a = get_string();
$a = intval($a);
if ($a < 0)
{
    $x = put_string("Faktorial nelze spocitat\n");
}
else
{
    $vysl = 1;
    while ($a > 0)
    {
        $vysl = $vysl * $a;
        $a = $a - 1;
    }
    $x = put_string("Vysledek je: ", $vysl, "-\n");
}
