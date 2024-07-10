#!/bin/bash

set -e

filename=$1
script_dir=$(realpath $(dirname $0))
split_dir=$(realpath $(dirname $1))


if [ -f ${filename} ]; then
    echo 'File exists.'

    split -l 50000 -d ${filename} file_
else
    echo 'File does not exist.'
    exit 1
fi

cd ${split_dir}

for i in $(find file_*); do mv $i "$i.csv"; done

for i in $(find . -type f -name "file_*.csv" -not -name "file_00.csv");
    do echo -e "$(head -1 file_00.csv)\n$(cat $i)" > $i;
done

src_line='\"'
target_line=' '

for i in $(find file_*); do sed -i -e 's/${src_line}/${target_line}/g' $i; done




cd ${script_dir}