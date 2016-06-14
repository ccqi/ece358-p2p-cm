TEST="test_content_distributed: single add/lookup/remove across nodes"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

C1VALUE=$(./lookupcontent $PEER2 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C1VALUE'"
fi

./removecontent $PEER2 $C1KEY
./removepeer $PEER2
./removepeer $PEER1

ok "${TEST}"
