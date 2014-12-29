#!/bin/sh

seed_col=5
seed_line=1000
seed_n_col=$(perl -e "print ${seed_col} * ${seed_line}")

if [ $# -ne 1 ]; then
    echo "$0 [ *n* where output has ${seed_col} * ${seed_line} * 2^n ]"
    exit 1
fi
n=$1
n_col=$(perl -e "print ${seed_col} * ${seed_line} * 2**${n}")

basedir="$(cd $(dirname $0) && pwd)"/..
datadir="$basedir/benchmark/csv"

echo Creating "$datadir/${n_col}col.csv ..."

cp "$datadir/${seed_n_col}col.csv" "$datadir/in.csv"
for i in $(seq $n); do
    cat "$datadir/in.csv" "$datadir/in.csv" > "$datadir/out.csv"
    mv "$datadir/out.csv" "$datadir/in.csv"
done
mv "$datadir/in.csv" "$datadir/${n_col}col.csv"

echo done
