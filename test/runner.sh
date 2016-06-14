#!/usr/bin/env bash
set -eu

export GIT_ROOT=$(git rev-parse --show-toplevel)
cd ${GIT_ROOT}

# create workdir
export TEST_DIR="${GIT_ROOT}/test/work"
mkdir -p "${TEST_DIR}"
cp a1-358s16.zip "${TEST_DIR}/a1-358s16.zip"
cd "${TEST_DIR}"

# setup environment
EXECS=(addpeer removepeer addcontent removecontent lookupcontent)

unzip -q -o a1-358s16

# ensure binaries exist
echo
make -s clean ${EXECS[*]}
for i in ${EXECS[*]} ; do
    if [ ! -f $i ] ; then
        echo -e "\033[31mfail: \033[0mBINARIES"
        echo "'$i' not found"
        exit 1
    fi
done

echo -e "\033[32mok: \033[0mBINARIES"


# "-----------------------------"
# "- TEST 0                    -"
# "- Based on test_example1.sh -"
# "-----------------------------"
TMPFILE="${TEST_DIR}/temp.txt"
./addpeer > $TMPFILE

PEER1=$(cat $TMPFILE)
if [ "empty" == "${PEER1:-empty}" ] ; then
    echo -e "\033[31mfail: \033[0mTEST 0"
    echo "./addpeer failed to print a port (got '${PEER1}'): see $TMPFILE"
    exit 1
fi
rm $TMPFILE

C1KEY=$(./addcontent $PEER1 'hello world')
C1VALUE=$(./lookupcontent $PEER1 $C1KEY)
if [ "hello world" != "$C1VALUE" ] ; then
    echo -e "\033[31mfail: \033[0mTEST 0"
    echo "./lookupcontent returned '$C1VALUE'"
    exit 1
fi

./removecontent $PEER1 $C1KEY
./removepeer $PEER1

echo -e "\033[32mok: \033[0mTEST 0"
