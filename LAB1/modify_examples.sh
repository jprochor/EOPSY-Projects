#!/bin/bash
touch testfile.txt;
mkdir $PWD/extra
echo $'1. Calling without any extra arguments\n';
./modify.sh
echo $'2. Calling with uppercase\n';
./modify.sh -u testfile.txt;
echo $'3. Calling with lowercase\n';
./modify.sh -l Testfile.txt;
echo $'4. Calling help\n';
./modify.sh -h testfile.txt;
echo $'5. Calling with recursion and uppercase\n';
./modify.sh -r -u testfile.txt;
echo $'6. Calling with recursion and lowercase\n';
./modify.sh -r -l Testfile.txt;
touch $PWD/extra/testfile2.txt;
echo $'7. Calling with uppercase and with directory\n';
./modify.sh -u $PWD/extra
echo $'8. Calling with lowercase and with directory\n';
./modify.sh -l $PWD/extra
echo $'10. Calling help and with path\n';
./modify.sh -h $PWD/extra
echo $'11. Calling with recursion and uppercase and with directory\n';
./modify.sh -r -u $PWD/extra
echo $'12. Calling with recursion and lowercase and with directory\n';
./modify.sh -r -l $PWD/extra
echo $'13. Calling with uppercase and path\n';
./modify.sh -u $PWD/extra/testfile2.txt;
echo $'14. Calling with lowercase and path\n';
./modify.sh -l $PWD/extra/Testfile2.txt;
echo $'15. Calling help with path\n';
./modify.sh -h $PWD/extra/testfile2.txt;
echo $'16. Calling with recursion and uppercase with path\n';
./modify.sh -r -u $PWD/extra/testfile2.txt;
echo $'17. Calling with recursion and lowercase with path\n';
./modify.sh -r -l $PWD/extra/Testfile2.txt;
echo $'18. Calling sed\n';
#./modify.sh 's/\w\+/\L\u&/g';
echo $'19. Calling sed with directory\n';
#./modify.sh 's/\w\+/\L\u&/g' $PWD/extra;
echo $'20. Calling sed with path\n';
#./modify.sh 's/\w\+/\L\u&/g' $PWD/extra/Testfile2.txt;

#section of improper calling


echo $'21. Calling with an invalid parameter\n';
./modify.sh -z;
echo $'22. Calling with both uppercase and lowercase parameter\n';
./modify.sh -u -l $PWD/extra/testfile2.txt;
echo $'23. Calling with both uppercase and lowercase and recursion parameter\n';
./modify.sh -u -l -r;
echo $'24. Calling with both uppercase and recursion parameter and additional invalid parameter\n';
./modify.sh -u -b -r;
echo $'24. Calling with both uppercase and additional invalid parameter\n';
./modify.sh -z -u;
echo $'25. Calling directory with both uppercase and lowercase\n';
./modify.sh -l -u $PWD/extra;
echo $'26. Calling both sed and directory\n';
#./modify.sh 's/\w\+/\L\u&/g' $PWD/extra;
echo $'27. Calling sed and an additional sed\n';
#./modify.sh 's/\w\+/\L\u&/g' 's/\w\+/\L\u&/g';











