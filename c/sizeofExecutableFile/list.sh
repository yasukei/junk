#!/bin/sh

RESULT=result.txt

files=''
for file in *; do
	if [ -x ${file} ]; then
		files=${files}' '${file}
	fi
done

ls -l ${files} >${RESULT}
size ${files} >>${RESULT}
cat ${RESULT}
