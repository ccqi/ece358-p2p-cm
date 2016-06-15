TEST="test_peer_content_peer: test peers->content->peer case"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "P1 ./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "P2 ./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'a')
C2KEY=$(./addcontent $PEER1 'b')
C3KEY=$(./addcontent $PEER1 'c')
C4KEY=$(./addcontent $PEER1 'd')
C5KEY=$(./addcontent $PEER1 'e')
C6KEY=$(./addcontent $PEER1 'f')

./addpeer $PEER1 > $TMPFILE; sleep 1 # crashes here
PEER3=$(cat $TMPFILE)
if [ "empty" == "${PEER3:-empty}" ] ; then
    fail "${TEST}" "P3 ./addpeer ${PEER1} failed to print a port (got '${PEER3}'): see $TMPFILE"
fi
rm $TMPFILE

C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "a" != "$C1VALUE" ] ; then
    fail "${TEST}" "./C1 lookupcontent returned '$C1VALUE'"
fi
C2VALUE=$(./lookupcontent $PEER1 $C2KEY)
if [ "b" != "$C2VALUE" ] ; then
    fail "${TEST}" "./C2 lookupcontent returned '$C2VALUE'"
fi
C3VALUE=$(./lookupcontent $PEER1 $C3KEY)
if [ "c" != "$C3VALUE" ] ; then
    fail "${TEST}" "./C3 lookupcontent returned '$C3VALUE'"
fi
C4VALUE=$(./lookupcontent $PEER1 $C4KEY)
if [ "d" != "$C4VALUE" ] ; then
    fail "${TEST}" "./C4 lookupcontent returned '$C4VALUE'"
fi
C5VALUE=$(./lookupcontent $PEER1 $C5KEY)
if [ "e" != "$C5VALUE" ] ; then
    fail "${TEST}" "./C5 lookupcontent returned '$C5VALUE'"
fi
C6VALUE=$(./lookupcontent $PEER1 $C6KEY)
if [ "f" != "$C6VALUE" ] ; then
    fail "${TEST}" "./C6 lookupcontent returned '$C6VALUE'"
fi

./removecontent $PEER1 $C1KEY
./removecontent $PEER1 $C2KEY
./removecontent $PEER1 $C3KEY
./removecontent $PEER1 $C4KEY
./removecontent $PEER1 $C5KEY
./removecontent $PEER1 $C6KEY
./removepeer $PEER1
./removepeer $PEER2
./removepeer $PEER3

ok "${TEST}"
