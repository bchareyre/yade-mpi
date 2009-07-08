#!/bin/sh
# run this to get nice text to paste into the openoffice sheet
TMPS=/tmp/a$$ /tmp/b$$ /tmp/c$$ /tmp/d$$
ls *.ser?.log
awk '/TOTAL/ { print $2}' *.ser?.log > /tmp/a$$
awk '/Collider/ { print $4}' *.ser?.log > /tmp/b$$
awk '/TOTAL/ { print $2}' *.par?.log > /tmp/c$$
awk '/Collider/ { print $4}' *.par?.log > /tmp/d$$
paste /tmp/a$$ /tmp/b$$ /tmp/c$$ /tmp/d$$ | sed 's/us//g' | sed 's/%//g' | sed 's/\./,/g'
rm -rf $TMPS
