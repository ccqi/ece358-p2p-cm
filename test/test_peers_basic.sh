TEST="test_peers_basic: add two peers, remove in opposite order"

TMPFILE="${WORK_DIR}/temp.txt"
./addpeer > $TMPFILE; sleep 1

PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER1 > $TMPFILE; sleep 1

PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

./removepeer $PEER2
./removepeer $PEER1

ok "${TEST}"
