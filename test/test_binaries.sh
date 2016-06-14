TEST="test_binaries: test binaries exist"

for i in ${EXECS[*]} ; do
    if [ ! -f $i ] ; then
        fail "${TEST}" "'$i' not found"
    fi
done

ok "${TEST}"
