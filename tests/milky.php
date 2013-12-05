<?php  
 
function printAllPermutations($str, $prefix) {
        $strLen = strlen($str);
        if ($strLen === 1) {
                $res = put_string($prefix, $str, "\n");
        } else {
                $n = 0;
                while ($n < $strLen) {
                        // We have to check case so that $n != $strLen because of get_substring
                        if ($n < $strLen - 1) {
                                $rest = get_substring($str, 0, $n) . get_substring($str, $n + 1, $strLen);
                        } else {
                                $rest = get_substring($str, 0, $n);
                        }
                        $pref = $prefix . get_substring($str, $n, $n + 1);
                        $res = printAllPermutations($rest, $pref);
                        $n = $n + 1;
                }
        }
}
 
$res = put_string("String to permute:\n");
$str = get_string();
$res = printAllPermutations($str, "");