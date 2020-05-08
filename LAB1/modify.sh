# !/bin/bash



function ChangeName()   # function uppercasing(^) or lowercasing(,) given file if the previously read -u or -l options were detected 
{
directory=$(dirname "$1")/$(basename "$1");
base=$(basename "$1")
if [[ ${parameters[1]} == u ]]
then
newBase=$(echo ${base^});             #uppercase
fi
if [[ ${parameters[2]} == l ]]
then
newBase=$(echo ${base,});             #lowercase
fi
newDirectory=$(dirname "$1")/$newBase;
mv "$directory" "$newDirectory";
echo "changed name from $directory to $newDirectory";
}

# 4 following functions responsible for finding and dealing with handing over a task of changing name of a particular file if it was found (for cases of changing names with or without recursion and using or not using sed) 
function ChangeNames()  
{ 
find $1 -maxdepth 1 -type f |while read i 
do
ChangeName "$i";
done
}

function ChangeNamesRec() 
{ 
find $1 -type f |while read i
do
ChangeName "$i";
done
}

function ChangeNamesSed()
{ 
find $2 -maxdepth 1 -type f |while read i
do
sed -s  $1;
done
}

function ChangeNamesSedRec()
{ 
find $2 -type f |while read i
do
sed -s $1;
done
}


function ArgumentRecognition() #function run firstly in order to recognize which elements from within arguments are path or sed
{ 
#looping through the arguments
for ((i=0;i<${#Arg[*]};i++))  
do
if [ -f ${Arg[i]} ] || [ -d ${Arg[i]} ]  #recognize a file/directory
then 
input[i+1]=${Arg[i]}; #store paths
elif  [[ -n ${Arg[i]} ]] && [ ${Arg[i]} != '-r' ] && [ ${Arg[i]} != '-l' ] && [ ${Arg[i]} != '-u' ] #if it's not parameters then sed
then 
input[0]=${Arg[i]};  #store sed pattern
fi
done
}

function Execute()  #final function managing distribution of proper cases and calling relevant functions for changing names or displaying errors
{
if [ ${parameters[0]} != 1 ] && [ ${parameters[1]} != 1 ] && [ ${parameters[2]} != 1 ]  #mistake of three options
then
echo "Don't use -l -u -r at the same time";
exit;
fi

if  [ ${parameters[1]} != 1 ] && [ ${parameters[2]} != 1 ] #mistake of both -l and -u
then 
echo "Don't use -l -u at the same time";
exit;
fi



if [ ${parameters[0]} != 1 ] && ([ ${parameters[1]} != 1 ] || [ ${parameters[2]} != 1 ]) #in other words if two parameters (-r -l, -r -u) were detected
then
if [[ ${#input[*]} == 0 ]]  #no directory, so assume current directory
then
ChangeNamesRec $PWD;
else
#looping to change names of all given paths
for ((i=0;i<${#Arg[*]};i++)) 
do
if test "${input[i+1]+isset}"   #if it exists take the path given as an argument
then
ChangeNamesRec ${input[i+1]};
fi
done
fi





elif [ ${parameters[0]} == 1 ] && [ ${parameters[1]} == 1 ] && [ ${parameters[2]} == 1 ]  #if no parameters were detected (so it's sed)
then
if [[ ${#input[*]} == 0 ]] #both no parameters and no path/sed - probably a mistake
then
echo $help;
exit;
fi
if [[ ${#input[*]} == 1 ]]   
then 
if test "${input[0]+isset}"   #checking the sed
then
ChangeNamesSed $PWD ${args[1]};
else
echo "${input[0]} is not a proper sed pattern";
exit;
fi
fi
if [[ ${#input[*]} == 2 ]]  #both path and sed present 
then 
ChangeNamesSed ${input[0]} ${input[1]};
fi


else
if [ ${parameters[0]} == r ]  #if -r option was detected (and not -l, -u, so it's sed)
then
if [[ ${#input[*]} == 0 ]]
then
echo $help;
exit;
fi
if [[ ${#input[*]} == 1 ]]
then 
if test "${input[1]+isset}" 
then
ChangeNamesSedRec $PWD ${input[1]};
else
echo "${input[0]} is not a proper sed pattern";
exit;
fi
fi
if [[ ${#input[*]} == 2 ]]
then
ChangeNamesSedRec ${input[0]} ${input[1]};
fi


else
if [[ ${#input[*]} == 0 ]]    #last remaining case (either -l or -u)
then
ChangeNames $PWD;
else
#looping to change names of all given paths
for ((i=0;i<${#Arg[*]};i++)) 
do
if test "${input[i+1]+isset}" 
then
ChangeNames ${input[i+1]};
fi
done
fi
fi
fi
}

#description for -h
help="
Syntax:

1) modify [-r] [-l|-u] <dir/file names...>
2) modify [-r] <sed pattern> <dir/file names...>
3) modify [-h]

The script is dedicated to lowerizing (-l) file names, uppercasing (-u) file names or internally calling sed
command with the given sed pattern which will operate on file names.
Changes may be done either with recursion (-r) or without it. -r doesn't take arguments so it's a standalone option.

Options:
(-u) - uppercasing all file names and directories
(-l) - lowerizing all file names and directories
(-r) - recursively change file and directory names with given options -u or -l
(-h) - help"


input=(); #initialize empty input
parameters=( 1 1 1 );  #initialize hardcoded initial parameter fillers (for detection purposes)

while getopts "ulrh" options  #using getopts to find and recognize arguments being options (looking for -u, -l, -r, -h)
do
case "${options}" in
u)
parameters[1]='u';
;;
l)
parameters[2]='l';
;;
r)
parameters[0]='r';
;;
h)
echo $help
exit
;;
\?)
echo $help  # (other parameter, probably a mistake)
exit
;;
esac
done

Arg=($*);  #place here all the arguments
ArgumentRecognition;   
Execute;





