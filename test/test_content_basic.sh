TEST="test_content_basic: single add/lookup/remove"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')
C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "./lookupcontent returned '$C1VALUE'"
fi

./removecontent $PEER1 $C1KEY
./removepeer $PEER1

ok "${TEST}"
