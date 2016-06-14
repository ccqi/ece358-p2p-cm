#!/usr/bin/env bash
set -eu

function fail {
    echo -e "\033[31mfail: \033[0m${1}"
    echo $2
    exit 1
}
function ok {
    echo -e "\033[32mok: \033[0m${1}"
}

export EXECS=(addpeer removepeer addcontent removecontent lookupcontent)
export GIT_ROOT=$(git rev-parse --show-toplevel)
export TEST_DIR="${GIT_ROOT}/test"
export WORK_DIR="${TEST_DIR}/work"

# create workdir
mkdir -p "${WORK_DIR}"
cp "${GIT_ROOT}/a1-358s16.zip" "${WORK_DIR}/a1-358s16.zip"
cd "${WORK_DIR}"

unzip -q -o a1-358s16
make -s clean ${EXECS[*]}


echo

. "${TEST_DIR}/test_binaries.sh"
. "${TEST_DIR}/test_content_basic.sh"
. "${TEST_DIR}/test_peers_basic.sh"
. "${TEST_DIR}/test_peers_many.sh"
