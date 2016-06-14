TEST="test_peers_many: add four to random nodes, remove in random order"

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
    fail "${TEST}" "first ./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER2 > $TMPFILE; sleep 1

PEER3=$(cat $TMPFILE)
if [ "empty" == "${PEER3:-empty}" ] ; then
    fail "${TEST}" "second ./addpeer ${PEER2} failed to print a port (got '${PEER3}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER1 > $TMPFILE; sleep 1

PEER4=$(cat $TMPFILE)
if [ "empty" == "${PEER4:-empty}" ] ; then
    fail "${TEST}" "third ./addpeer ${PEER1} failed to print a port (got '${PEER4}'): see $TMPFILE"
fi
rm $TMPFILE

./removepeer $PEER3
./removepeer $PEER1
./removepeer $PEER2
./removepeer $PEER4

ok "${TEST}"
