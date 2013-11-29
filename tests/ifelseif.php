<?php
$x = 1;

$c = put_string("only if:\n");

if($x > 0)
{
    $c = put_string("if\n");
}
elseif($x < 0)
{
    $c = put_string("elseif\n");
}
else
{
    $c = put_string("else\n");
}



$c = put_string("only if:\n");

if($x !== 0)
{
    $c = put_string("if\n");
}
elseif($x !== 0)
{
    $c = put_string("elseif\n");
}
else
{
    $c = put_string("else\n");
}


$c = put_string("only else:\n");
if($x === 0)
{
    $c = put_string("if\n");
}
elseif($x === 0)
{
    $c = put_string("elseif\n");
}
else
{
    $c = put_string("else\n");
}








$c = put_string("only elseif:\n");
if($x < 0)
{
    $c = put_string("if\n");
}
elseif($x > 0)
{
    $c = put_string("elseif\n");
}
else
{
    $c = put_string("else\n");
}

$c = put_string("only elseif:\n");
if($x < 0)
{
    $c = put_string("if\n");
}
elseif($x > 0)
{
    $c = put_string("elseif\n");
}

    $c = put_string("else\n");
