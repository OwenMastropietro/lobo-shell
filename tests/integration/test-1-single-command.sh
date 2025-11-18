TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp $OUTDIR/etmp

./lobo_shell.x > $OUT << 'EOF'
whoami
EOF

whoami > $EOUT

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
