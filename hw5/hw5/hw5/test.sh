#!/bin/bash
n="$(echo $1 | cut -d \. -f 1)"
il="$n.il"
res="$n.res"
./hw5 < $1 >& $il
./spim -file t1.il > $res

