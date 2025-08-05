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

assert 3 "a=1; b=2; a+b;"
assert 10 "foo=10; foo;"
assert 5 "a=3; b=2; a=a+b; a;"
assert 2 "foo=1; bar=foo+1; bar;"
assert 21 "a=1; b=2; c=3; d=4; e=5; f=6; a+b+c+d+e+f;"

assert 14 "a=3; b=5*6-8; a+b/2;"
assert 14 "a=2; b=3; c=4; a+b*c;"
assert 20 "a=2; b=3; c=4; (a+b)*c;"
assert 2 "a=8; b=2; a/b/2;"
assert 2 "a=8; b=2; a/(b*2);"

assert 10 "-10+20;"
assert 10 "+10;"
assert 251 "a=10; -a+5;"

# 等于 (==)
assert 1 "1==1;"
assert 0 "1==2;"

# 不等于 (!=)
assert 1 "1!=2;"
assert 0 "1!=1;"

# 小于 (<)
assert 1 "1<2;"
assert 0 "2<1;"

# 小于等于 (<=)
assert 1 "1<=1;"
assert 1 "1<=2;"
assert 0 "2<=1;"

# 大于 (>)
assert 1 "2>1;"
assert 0 "1>2;"

# 大于等于 (>=)
assert 1 "2>=2;"
assert 1 "2>=1;"
assert 0 "1>=2;"

assert 1 "a=1; b=1; a==b;"
assert 0 "a=1; b=2; a==b;"
assert 1 "foo=10; bar=5; foo>bar;"
assert 1 "foo=5; bar=10; foo<=bar;"
assert 1 "a=1; b=2; c=a+b; c==3;"
assert 0 "(1+2)==(2+2);"

assert 6 "foo=1; bar=2; foo=foo+bar; bar=bar*foo; bar;"
assert 1 "a=2; b=3; a=b-a; a==1;"


assert 2 "a=2+1;b=a*2/3;c=b+1;return b;"
echo OK