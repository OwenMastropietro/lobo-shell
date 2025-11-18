TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/shouldnt-exist

./lobo_shell.x >$OUT <<EOF
echo "A B C > $OUTDIR/shouldnt-exist"
EOF

echo "A B C > $OUTDIR/shouldnt-exist" >$EOUT

if [ -f $OUTDIR/shouldnt-exist* ]; then
    echo "TEST FAILED $0"
    rm -f $OUTDIR/shouldnt-exist*
    exit
fi

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
