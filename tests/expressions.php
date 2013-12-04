<?php

function foo ($a, $b)
{
    if (-(- -$a) > 0 ){ $p = foo( -$a, -$b ); }
    return -$b * -$a;
}
//php -d open_basedir="" ifj13.php ./tests/testovanySkript.php

$a = 5*4+22.38;
$a = put_string("5*4+22.38 = ", $a, "\n\n");
$a = 5.0*4+22.38 - 8;
$a = put_string("5.0*4+22.38 - 8 = ", $a, "\n\n");
$a = 5*4-22.38+64.2;
$a = put_string("5*4-22.38+64.2 = ", $a, "\n\n");
$b = 875.00;
$a = 5*4+22.38 - $b;
$a = put_string("\$b = 875.00", "5*4+22.38 - \$b = ", $a, "\n\n");
$a = 5*4+0.0-0.0+0e0+22.38;
$a = put_string("5*4+0.0-0.0+0e0+22.38 = ", $a, "\n\n");
$a = 5*4+22.38/22/33.5;
$a = put_string("5*4+22.38/22/33.5 = ", $a, "\n\n");
$b = 0.0;
$c = 0e-0;
$d = 0-0.0;
$e = 0e-22;
$a = 5*4+22.38 + $a - $c * $d * $e;
$a = put_string("\$b = 0.0", "\$c = 0e-0", "\$d = 0-0.0", "\$e = 0e-22", "5*4+22.38 + \$a - \$c * \$d * \$e = ", $a, "\n\n");
$a = false <= true;
$a = 5!=="0.0"===false<=true>false!==5.5==="ahoj";
$a = 5!=="0.0"===false<=true>false!==5.5==="ahoj";
$a = put_string("5 !== \"0.0\" === 4 <= true > false !== 5.5 === \"ahoj\" = ", $a, "\n\n");
$a = "54564646464".55;
$a = put_string("\"54564646464\".55 = ", $a, "\n\n");
$a = "".555.5;
$a = put_string("\"\".555.5 = ", $a, "\n\n");
$a = "".null;
$a = put_string("\"\".null = ", $a, "\n\n");
$a = "".true;
$a = put_string("\"\".true = ", $a, "\n\n");
$a = "".false;
$a = put_string("\"\".false = ", $a, "\n\n");
$a = ""."\x00"."xxx".55.22.33.true.false.null;
$a = put_string("\"\".\"\x00\".\"xxx\".55.22.33.true.false.null = ", $a, "\n\n");
$a = -5*-4+22.38*-foo(8,-5)+foo (-1, 4);
$a = put_string("-5*-4+22.38*-foo(8,-5)+foo (-1, 4) = ", $a, "\n\n");
$c = -(- -5*-1+- - -3- -(- -(-9- -(4*- - -(-2)*-(- -2)*-45)-(- -(-(- -3)+-1))-(-42))*-2));
$x = put_string("-(- -5*-1+- - -3- -(- -(-9- -(4*- - -(-2)*-(- -2)*-45)-(- -(-(- -3)+-1))-(-42))*-2)) = ", $c, "\n\n");