BGre='\e[1;32m';
BRed='\e[1;31m'; 
RCol='\e[0m'
retval=0


echo -e "========================================================\n"

echo "----------------- LEXIKALNE CHYBY --------------------"
for f in testy_HC/lexikalna_analyza/*.1; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 1 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi
    done;
echo 


echo "---------------- SYNTAKTICKE CHYBY -------------------"
for f in testy_HC/syntakticka/*.2; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 2 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi    
    done;
echo 


echo "---------------- NEDEF. / REDEF. FUNKCIA--------------"
for f in testy_HC/semanticka+interpret/*.3; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 3 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi    
    done;
echo 

    
echo "------ CHYBAJUCI PARAMETER PRI VOLANI FUNKCIE -------" 
for f in testy_HC/semanticka+interpret/*.4; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 4 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi    
    done;
echo


echo "------------- NEDKLAROVANE PREMENNE -----------------" 
for f in testy_HC/semanticka+interpret/*.5; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 5 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi    
done;
echo 



echo "----------------- DELENIE NULOU ---------------------"
for f in testy_HC/semanticka+interpret/*.10; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 10 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi    
    done;
echo 

echo "--------------- CHYBA PRI PRETYPOVANI ---------------"
for f in testy_HC/semanticka+interpret/*.11; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 11 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi
    done;
echo 

echo "------------- TYPOVA KOMPATIBILITA ------------------"
for f in testy_HC/semanticka+interpret/*.12; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 12 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi
    done;
 echo   

echo "------- OSTATNE SEMANTICKE / BEHOVE CHYBY -----------"
for f in testy_HC/semanticka+interpret/*.13; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 13 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi
    done;
 echo   
 
echo "---------------- VALIDNE PROGRAMY -------------------"
for f in testy_HC/validne_programy/*.0; do 
    echo -n "File: $f ... "; 
    ./main $f 2> /dev/null
    retval=$?
    if [ $retval = 0 ]; then
        echo -e "${BGre}\tOK"$RCol
    else
        echo -e "${BRed}\tFAIL"$RCol
    fi
    valgrind --quiet --leak-check=full --error-exitcode=100 ./main $f 2> /dev/null
    if [ $? = 100 ]; then
        echo -e "${BRed}\tMemory leak detected ! $?"$RCol
    fi
    done;
 echo 
    
echo "========================================================"