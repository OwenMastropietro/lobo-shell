TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp $OUTDIR/etmp

./lobo_shell.x >$OUT <<EOF
wc -l < /usr/share/dict/words
whoami > $OUTDIR/tmp
wc -w < /usr/share/dict/words >> $OUTDIR/tmp
cat $OUTDIR/tmp
EOF

wc -l </usr/share/dict/words >$EOUT
whoami >$OUTDIR/etmp
wc -w </usr/share/dict/words >>$OUTDIR/etmp
cat $OUTDIR/etmp >>$EOUT

rm -f $OUTDIR/tmp $OUTDIR/etmp

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
