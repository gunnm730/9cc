#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ] ; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected,but got $actual"
        exit 1
    fi
}


assert 42 42
assert 45 "23-56+78"
assert 21 " 5 + 20 - 4 "
assert  2 "1*2+3-6/2"
assert  9 "(2+3) * 4 /2-  1"
assert  3 "- 1 +2*(1+3/3)"
assert  0 "1>=2"
assert  1 "1+2<=3*1"

echo OK