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

unzip -o a1-358s16

# ensure binaries exist
make clean ${EXECS[*]}
for i in ${EXECS[*]};
do
  if [ ! -f $i ]; then
    echo >&2 "ERROR: \"$i\" not found "
    exit 1
  fi
done


# -----------------------------
# - TEST 0                    -
# - Based on test_example1.sh -
# -----------------------------
(set +e; pkill -f peer)

(set -x ;

TMPFILE=temp.txt ;
./addpeer > $TMPFILE ;
PEER1=`sed -n '1p' $TMPFILE` ;
rm $TMPFILE ;

C1KEY=$(./addcontent $PEER1 'hello world') ;

./lookupcontent $PEER1 $C1KEY ;
./removecontent $PEER1 $C1KEY ;

./removepeer $PEER1 ;)
