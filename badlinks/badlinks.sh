#!/bin/bash

for file in `find -L "$1" -type l -mmin +10080`
do
	echo $file
done

