TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp $OUTDIR/etmp

./lobo_shell.x >$OUT <<'EOF'
ls /usr/bin | head -10
EOF

ls /usr/bin | head -10 >$EOUT

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
