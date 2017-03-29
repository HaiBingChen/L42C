#!/bin/sh

dir=$(find ${PWD} -name "*.c")
for d in $dir
do
    cat $d > tmp
	rm $d -rf
	mv tmp $d
done   

dir=$(find ${PWD} -name "*.h")
for d in $dir
do
    cat $d > tmp
    rm $d -rf
    mv tmp $d
done

dir=$(find ${PWD} -name "*.s")
for d in $dir
do
    cat $d > tmp
    rm $d -rf
    mv tmp $d
done


dir=$(find ${PWD} -name "*.S")
for d in $dir
do
    cat $d > tmp
    rm $d -rf
    mv tmp $d
done

