
export CHAR=¡
echo -n $CHAR | ./bigfont $CHAR | ./bigfont $CHAR | ./bigfont $CHAR | ./t2i -col 0xff00 -ts 1 -w 10000 -h 10000 && xviewer t2i.png
