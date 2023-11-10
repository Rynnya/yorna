#!/bin/bash

if [ -z "$1" ]
then
	echo "Filepath not provided!"
	exit 1
fi

glslc $1 -o $1.spv
return_code=$?

if [ $return_code -eq 0 ]
then
	echo "Successfully compiled" $1
else
	echo "Failed to compile" $1
	exit $return_code
fi
