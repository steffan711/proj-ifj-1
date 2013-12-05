<?php
$s = doubleval("1e2 ew");
$x = put_string($s, " = 100 \n");

$s = doubleval("e2 ew");
$x = put_string($s, " = 0 \n");

$s = doubleval("1.1e2 ew");
$x = put_string($s, " = 110 \n");

$s = doubleval("1.1.e2 ew");
$x = put_string($s, " = 1.1 \n");

$s = doubleval("1.1e2ew");
$x = put_string($s, " = 110 \n");

$s = doubleval(".e2 ew");
$x = put_string($s, " = 0 \n");

$s = doubleval("1.1e1.2w2 ew");
$x = put_string($s, " = 11 \n");

$s = doubleval("1.1e");