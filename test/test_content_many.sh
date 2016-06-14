TEST="test_content_many: multiple add/lookup/remove contents, random order"

./addpeer > $TMPFILE; sleep 1
PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')
C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "C1 ./lookupcontent $C1KEY returned '$C1VALUE'"
fi

C2KEY=$(./addcontent $PEER1 'applesauce and beer')
C2VALUE=$(./lookupcontent $PEER1 $C2KEY)
if [ "applesauce and beer" != "$C2VALUE" ] ; then
    fail "${TEST}" "C2 ./lookupcontent $C2KEY returned '$C2VALUE'"
fi

./removecontent $PEER1 $C2KEY

C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "C1 (2nd) ./lookupcontent $C1KEY returned '$C1VALUE'"
fi

./removecontent $PEER1 $C1KEY

./removepeer $PEER1

ok "${TEST}"
