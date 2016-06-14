TEST="test_peers_many: add four to random nodes, remove in random order"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "P1-xx ./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "P2-P1 ./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER2 > $TMPFILE; sleep 1
PEER3=$(cat $TMPFILE)
if [ "empty" == "${PEER3:-empty}" ] ; then
    fail "${TEST}" "P3-P2 ./addpeer ${PEER2} failed to print a port (got '${PEER3}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER4=$(cat $TMPFILE)
if [ "empty" == "${PEER4:-empty}" ] ; then
    fail "${TEST}" "P4-P1 (2nd) ./addpeer ${PEER1} failed to print a port (got '${PEER4}'): see $TMPFILE"
fi
rm $TMPFILE

./removepeer $PEER3
./removepeer $PEER1
./removepeer $PEER2
./removepeer $PEER4

ok "${TEST}"
