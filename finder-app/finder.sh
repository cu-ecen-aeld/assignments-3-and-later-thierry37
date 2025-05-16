#!/bin/sh

# check the input parameters : 
# $1 shall be a directory name
# $2 shall be a string to search

if [ $# -ne 2 ]
then
	echo "usage : find.sh <directory name> <strinf to search>"
	exit 1
else
	filesdir=$1
	searchstr=$2
	
	if [ ! -d  $filesdir ] 
	then
  		echo "$filesdir does not exist."
		exit 1
	else
		# get the number of files in filesdir and its subdirectories
		# we use find command, starting from filesdir. type -t searches for all files
		# wc -l counts the number of lines in the response
  		nbFiles=`find $filesdir -type f | wc -l`
		#echo "directory $filesdir and its subdirectories contains $nbFiles files"

		# count how many lines contains the searched string
		nbLines=`grep -r $searchstr $filesdir | wc -l`
		echo "The number of files are $nbFiles and the number of matching lines are $nbLines"
	fi
	exit 0	
fi
