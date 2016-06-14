TEST="test_content_many: multiple add/lookup/remove contents, random order"

TMPFILE="${WORK_DIR}/temp.txt"
./addpeer > $TMPFILE; sleep 1

PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    fail "${TEST}" "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')
C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "first ./lookupcontent $C1KEY returned '$C1VALUE'"
fi

C2KEY=$(./addcontent $PEER1 'applesauce and beer')
C2VALUE=$(./lookupcontent $PEER1 $C2KEY)
if [ "applesauce and beer" != "$C2VALUE" ] ; then
    fail "${TEST}" "second ./lookupcontent $C2KEY returned '$C2VALUE'"
fi

./removecontent $PEER1 $C2KEY

C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    fail "${TEST}" "third ./lookupcontent $C1KEY returned '$C1VALUE'"
fi

./removecontent $PEER1 $C1KEY

./removepeer $PEER1

ok "${TEST}"
