<?php
function a($param1)
{
    if ($param1 > 10)
    {
        $p = put_string("funkcia a() vola funkciu b()\n");
        $p = b($param1);
    }
    elseif ($param1 < 10)
    {
        $p = put_string("funkcia a() vola funkciu c()\n");
        $p = c($param1);
    }
}

function b($param1)
{
    if ($param1 > 10)
    {
        $p = put_string("funkcia b() vola funkciu c()\n");
        $p = c($param1);
    }
    elseif ($param1 < 10)
    {
        $p = put_string("funkcia a() vola funkciu c()\n");
        $p = a($param1);
    }
}

function c($param1)
{
    if ($param1 > 10)
    {
        $p = put_string("funkcia c() vola funkciu a()\n");
        $p = a($param1 - 1);
    }
    elseif ($param1 < 10)
    {
        return 0;
    }
}

$p = a(5);