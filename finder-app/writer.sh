#!/bin/sh

# check the input parameters : 
# $1 shall be a directory name
# $2 shall be a string to search

if [ $# -ne 2 ]
then
	echo "usage : writer.sh <file name> <string to write>"
	exit 1
else
	writefile=$1
	writestr=$2
	
	# retrieve folder name from full path and check if the folder exists
	folder=`dirname $writefile`
	echo "taget folder is $folder"
	if [ $? -ne 0 ]
	then
		echo "could not extract folder name from file path $writefile"
	fi

	#if the folder does not exist, create it 
	if [ ! -d $folder ]
	then
		echo "create folder $folder"
		mkdir -p $folder
	fi

	# write the input string in the file, and create the file if it does not exist
	echo $writestr > $writefile
	# $? contains the result of the last command
	if [ $? -ne 0 ]
	then
		echo "could not create file $writefile"
		exit 1
	fi
	exit 0
fi
