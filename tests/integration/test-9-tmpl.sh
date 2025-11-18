TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp $OUTDIR/etmp

# under this umask, we would expect to see 664
umask 002

./lobo_shell.x >$OUT <<EOF
echo First > $OUTDIR/tmp
echo Follows > $OUTDIR/tmp
cat $OUTDIR/tmp
EOF

echo First >$OUTDIR/etmp
echo Follows >$OUTDIR/etmp
cat $OUTDIR/etmp >$EOUT

if [ ! -f $OUTDIR/tmp ] || [ ! -s $OUTDIR/tmp ]; then
    echo "TEST FAILED $0"
    exit 1
fi

if [ -z $(find $OUTDIR/tmp -perm 664) ]; then
    echo "TEST FAILED $0"
    exit 2
fi
rm -f $OUTDIR/tmp $OUTDIR/etmp

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
