TEST="test_content_balances: added content balances to new nodes, flees dead nodes"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')
C2KEY=$(./addcontent $PEER1 'hello again world')

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

./removepeer $PEER1

C1VALUE=$(./lookupcontent $PEER2 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C1VALUE'"
fi
C2VALUE=$(./lookupcontent $PEER2 $C2KEY)
if [ "hello again world" != "$C2VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C2VALUE'"
fi

./removecontent $PEER2 $C1KEY
./removecontent $PEER2 $C2KEY
./removepeer $PEER2

ok "${TEST}"
