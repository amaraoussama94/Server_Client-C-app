#!/bin/sh

output=~/Desktop/test-dev/stdout.txt
rm $output
cd $1
echo -These are the directories > $output
#ls -d $1*/ >> $output
ls -l | grep ^d |awk '{print""substr($0,index($0,$11))}' >> $output
echo -These are the files  >> $output
#ls -f >> $output
ls -l | grep ^- |awk '{print""substr($0, index($0,$11))}' >> $output