TEST="test_content_balances_many: larger test_content_balances (segfault case)"

TMPFILE="${WORK_DIR}/temp.txt"
./addpeer > $TMPFILE; sleep 1

PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'a')
C2KEY=$(./addcontent $PEER1 'b')
C3KEY=$(./addcontent $PEER1 'c')
C4KEY=$(./addcontent $PEER1 'd')
C5KEY=$(./addcontent $PEER1 'e')

./addpeer $PEER1 > $TMPFILE; sleep 1
PEER2=$(cat $TMPFILE)
if [ "empty" == "${PEER2:-empty}" ] ; then
    fail "${TEST}" "./addpeer ${PEER1} failed to print a port (got '${PEER2}'): see $TMPFILE"
fi
rm $TMPFILE

C6KEY=$(./addcontent $PEER1 'f')

./removepeer $PEER2  # segfault originally occured here

C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "a" != "$C1VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C1VALUE'"
fi
C2VALUE=$(./lookupcontent $PEER1 $C2KEY)
if [ "b" != "$C2VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C2VALUE'"
fi
C3VALUE=$(./lookupcontent $PEER1 $C3KEY)
if [ "c" != "$C3VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C3VALUE'"
fi
C4VALUE=$(./lookupcontent $PEER1 $C4KEY)
if [ "d" != "$C4VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C4VALUE'"
fi
C5VALUE=$(./lookupcontent $PEER1 $C5KEY)
if [ "e" != "$C5VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C5VALUE'"
fi
C6VALUE=$(./lookupcontent $PEER1 $C6KEY)
if [ "f" != "$C6VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C6VALUE'"
fi

./removecontent $PEER1 $C1KEY
./removecontent $PEER1 $C2KEY
./removecontent $PEER1 $C3KEY
./removecontent $PEER1 $C4KEY
./removecontent $PEER1 $C5KEY
./removecontent $PEER1 $C6KEY
./removepeer $PEER1

ok "${TEST}"
