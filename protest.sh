#!/bin/sh

RCol='\e[0m'    # Text Reset

# Regular           Bold                Underline           High Intensity      BoldHigh Intens     Background          High Intensity Backgrounds
Bla='\e[0;30m';     BBla='\e[1;30m';    UBla='\e[4;30m';    IBla='\e[0;90m';    BIBla='\e[1;90m';   On_Bla='\e[40m';    On_IBla='\e[0;100m';
Red='\e[0;31m';     BRed='\e[1;31m';    URed='\e[4;31m';    IRed='\e[0;91m';    BIRed='\e[1;91m';   On_Red='\e[41m';    On_IRed='\e[0;101m';
Gre='\e[0;32m';     BGre='\e[1;32m';    UGre='\e[4;32m';    IGre='\e[0;92m';    BIGre='\e[1;92m';   On_Gre='\e[42m';    On_IGre='\e[0;102m';
Yel='\e[0;33m';     BYel='\e[1;33m';    UYel='\e[4;33m';    IYel='\e[0;93m';    BIYel='\e[1;93m';   On_Yel='\e[43m';    On_IYel='\e[0;103m';
Blu='\e[0;34m';     BBlu='\e[1;34m';    UBlu='\e[4;34m';    IBlu='\e[0;94m';    BIBlu='\e[1;94m';   On_Blu='\e[44m';    On_IBlu='\e[0;104m';
Pur='\e[0;35m';     BPur='\e[1;35m';    UPur='\e[4;35m';    IPur='\e[0;95m';    BIPur='\e[1;95m';   On_Pur='\e[45m';    On_IPur='\e[0;105m';
Cya='\e[0;36m';     BCya='\e[1;36m';    UCya='\e[4;36m';    ICya='\e[0;96m';    BICya='\e[1;96m';   On_Cya='\e[46m';    On_ICya='\e[0;106m';
Whi='\e[0;37m';     BWhi='\e[1;37m';    UWhi='\e[4;37m';    IWhi='\e[0;97m';    BIWhi='\e[1;97m';   On_Whi='\e[47m';    On_IWhi='\e[0;107m';

bin="./main"
dir="pro_tests/"						#where to look for test folders and setting file
settings=$dir"settings.php"			#file with input/output file names and //description echoed while running this script
N="/dev/null"							#simplifying code
input=$dir"input/"						#input files directory for interpret
output=$dir"output/"					#folder to store output files
expected_output=$dir"expected_output/"	#output files directory to check difference
ext=".php"								#extension of input files
failfile=$dir"fails.txt"				#output file for fails that occured in tests
ifjfile=$dir"ifj13.php"					#definitions for php to cooperate with our interpret

# clear fail files
echo "" >$failfile

# cat $settings | tr '\t' ' ' | grep -v "^//" > "$dir.temp"

result=0
clear
DONE=false
until $DONE; do
	read line || DONE=true
	if [ -n "`echo $line | grep -v '^//'`" ]; then
		name=`echo $line | sed -e 's_//.*$__' | tr '\t' ' ' | sed -e 's_ *$__'`
		echo ""
		printf "$name || "
		echo "$line	<== BEGIN" | tr '\t' ' ' | sed -e 's_.*// *__'
		echo "===================================================="
		if [ -f "$input$name$ext" ]
		then
			$bin $input$name$ext </dev/stdin >$output$name 2>>$output$name"-fails"
			[ -f "$expected_output$name" ] || php -d open_basedir="" $ifjfile $input$name$ext </dev/stdin >$expected_output$name 2>$expected_output$name"-fails"
			if [ -z "`diff $expected_output$name $output$name`" ]; then
				echo -e ${Gre}"TEST SUCCESSFULL"${RCol}
			else
				echo -e ${Red}"TEST FAILED"${RCol}
				result=$((result+1))
				temp=`echo "$line" | tr '\t' ' ' | sed -e 's_.*// *__'`
				echo "!!!!!!!!!!!!!!!!!!!!!!!! BEGIN ==> filename: $name // $temp" >>$failfile
				diff $output$name $expected_output$name >>$failfile
			fi
			echo "" >>$failfile
			
		else
			[ -f "$input$name$ext" ] || echo -e ${Red}"Input file specified in config file: \"$name$ext\" doesn't exist!"${RCol}
		fi
		echo "===================================================="
		echo "$line	<== END" | tr '\t' ' ' | sed -e 's_.*// *__'
		echo ""
	fi
done < $settings

if [ $result -eq 0 ];
then
	echo -e ${Gre}"ALL RAN TESTS PASSED!!"${RCol}
else
	echo -e ${Red}"TOTALLY $result TESTS FAILED! CHECK \"$failfile\" for more info"${RCol}
fi
echo "";
# rm "$dir.temp"