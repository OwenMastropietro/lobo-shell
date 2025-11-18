TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp $OUTDIR/etmp

./lobo_shell.x >$OUT <<EOF
cat < /usr/share/dict/words | grep ish > $OUTDIR/tmp
cat $OUTDIR/tmp
EOF

cat </usr/share/dict/words | grep ish >$OUTDIR/etmp
cat $OUTDIR/etmp >>$EOUT

if [ ! -f $OUTDIR/tmp ] || [ ! -s $OUTDIR/tmp ]; then
    echo "TEST FAILED $0"
    exit 1
fi
rm -f $OUTDIR/tmp $OUTDIR/etmp

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
