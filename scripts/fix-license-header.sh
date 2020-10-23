#!/bin/bash

PROGRAM_NAME=$(basename $0)
CWD=$(dirname $0)
FILES=()
INPLACE=false

help() {
	echo $2
	echo "Usage:"
	echo "	$PROGRAM_NAME [-hi] [--help|--inplace] <file> ..."
	echo
	exit $1
}

while test $# -gt 0; do
	case $1 in 
	-h|--help) help ;;
	-i|--inplace) INPLACE=true ;;
	*) 
		test -f $1 || help 1 "$1 does not exist.\n"
		FILES+=($1)
		;;
	esac
	
	shift
done

test "${#FILES[@]}" -eq 0 && help 1

for FILE in "${FILES[@]}"; do
	if $INPLACE; then
		TEMP=$(mktemp $FILE.XXXXXXXX)
		exec 1>$TEMP
	fi
	cat $CWD/../LICENSE.HEADER
	echo
	awk < $FILE '
	BEGIN { hash_seen = 0 }
	/^#/ { hash_seen = 1 }
	{ if (hash_seen) print }
	'
	if $INPLACE; then
		mv $TEMP $FILE
	fi
done
