TESTDIR=tests/integration
OUTDIR=$TESTDIR/output

OUT=$OUTDIR/$(basename ${0%.sh}).out
EOUT=$OUTDIR/$(basename ${0%.sh}).eout
rm -f $OUT $EOUT $OUTDIR/tmp tests/output/etmp

./lobo_shell.x >$OUT <<'EOF'
cat /usr/share/dict/words | grep -i ^o | grep -i c$ | tail -100 | head | wc -c
EOF

cat /usr/share/dict/words | grep -i ^o | grep -i c$ | tail -100 | head | wc -c >$EOUT

diff $OUT $EOUT && echo "PASSED $0" || echo "TEST FAILED $0"
