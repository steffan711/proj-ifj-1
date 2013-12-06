<?php
$i = 1;
while ( 1 )
{ 
    if ( $i < 200000000 )
    {
        $i = $i + 1;
    }
    else
    {
        $p = put_string("hodnota i je :", $i);
        return $i;
    }
}