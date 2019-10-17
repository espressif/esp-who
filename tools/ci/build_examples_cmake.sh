#!/bin/bash
#
# Build all examples from the examples directory, out of tree to
# ensure they can run when copied to a new directory.
#
# Runs as part of CI process.
#
# Assumes PWD is an out-of-tree build directory, and will copy examples
# to individual subdirectories, one by one.
#
#
# Without arguments it just builds all examples
#
# With one argument <JOB_NAME> it builds part of the examples. This is a useful for
#   parallel execution in CI.
#   <JOB_NAME> must look like this:
#               <some_text_label>_<num>
#   It scans .gitlab-ci.yaml to count number of jobs which have name "<some_text_label>_<num>"
#   It scans the filesystem to count all examples
#   Based on this, it decides to run qa set of examples.
#

# -----------------------------------------------------------------------------
# Safety settings (see https://gist.github.com/ilg-ul/383869cbb01f61a51c4d).

if [[ ! -z ${DEBUG_SHELL} ]]
then
  set -x # Activate the expand mode if DEBUG is anything but empty.
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.
set -o nounset # Exit if variable not set.

# Remove the initial space and instead use '\n'.
IFS=$'\n\t'

export PATH="$IDF_PATH/tools:$PATH"  # for idf.py

# -----------------------------------------------------------------------------

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${IDF_PATH} ] && die "IDF_PATH is not set"
[ -z ${LOG_PATH} ] && die "LOG_PATH is not set"
[ -d ${LOG_PATH} ] || mkdir -p ${LOG_PATH}

echo "build_examples running in ${PWD}"

export BATCH_BUILD=1
export V=0 # only build verbose if there's an error

shopt -s lastpipe # Workaround for Bash to use variables in loops (http://mywiki.wooledge.org/BashFAQ/024)

RESULT=0
FAILED_EXAMPLES=""
RESULT_ISSUES=22  # magic number result code for issues found
LOG_SUSPECTED=${LOG_PATH}/common_log.txt
touch ${LOG_SUSPECTED}
SDKCONFIG_DEFAULTS_CI=sdkconfig.ci

EXAMPLE_PATHS=$( find ${CI_PROJECT_DIR}/examples/ -type f -name CMakeLists.txt | grep -v "/components/" | grep -v "/main/" | sort )

if [ -z "${CI_NODE_TOTAL:-}" ]
then
    START_NUM=0
    END_NUM=999
else
	JOB_PATTERN=$( echo ${CI_JOB_NAME} | sed -n -r 's/^(.*)_[0-9]+$/\1/p' )
    [ -z ${JOB_PATTERN} ] && die "JOB_PATTERN is bad"
	
	JOB_NUM=$( echo ${CI_JOB_NAME} | sed -n -r 's/^.*_0*([0-9]+)$/\1/p' )
    [ -z ${JOB_NUM} ] && die "JOB_NUM is bad"
	
    # count number of the jobs
	NUM_OF_JOBS=$( grep -c -E "^${JOB_PATTERN}_[0-9]+:$" "${CI_PROJECT_DIR}/.gitlab-ci.yml" )
    [ -z ${NUM_OF_JOBS} ] && die "NUM_OF_JOBS is bad"

    # count number of examples
    NUM_OF_EXAMPLES=$( echo "${EXAMPLE_PATHS}" | wc -l )
    [ -z ${NUM_OF_EXAMPLES} ] && die "NUM_OF_EXAMPLES is bad"

    # separate intervals
    #57 / 5 == 12
    NUM_OF_EX_PER_JOB=$(( (${NUM_OF_EXAMPLES} + ${NUM_OF_JOBS} - 1) / ${NUM_OF_JOBS} ))
    [ -z ${NUM_OF_EX_PER_JOB} ] && die "NUM_OF_EX_PER_JOB is bad"

    # ex.: [0; 12); [12; 24); [24; 36); [36; 48); [48; 60)
    START_NUM=$(( ${JOB_NUM} * ${NUM_OF_EX_PER_JOB} ))
    [ -z ${START_NUM} ] && die "START_NUM is bad"

    END_NUM=$(( (${JOB_NUM} + 1) * ${NUM_OF_EX_PER_JOB} ))
    [ -z ${END_NUM} ] && die "END_NUM is bad"
fi

build_example () {
    local ID=$1
    shift
    local CMAKELISTS=$1
    shift

    local EXAMPLE_DIR=$(dirname "${CMAKELISTS}")
    local EXAMPLE_NAME=$(basename "${EXAMPLE_DIR}")

    echo "Building ${EXAMPLE_NAME} as ${ID}..."
    mkdir -p "${ID}"
    cp -r "${EXAMPLE_DIR}" "${ID}"
    pushd "${ID}/${EXAMPLE_NAME}"
        # be stricter in the CI build than the default IDF settings
        export EXTRA_CFLAGS="-Werror -Werror=deprecated-declarations"
        export EXTRA_CXXFLAGS=${EXTRA_CFLAGS}

        # build non-verbose first
        local BUILDLOG=${LOG_PATH}/ex_${ID}_log.txt
        touch ${BUILDLOG}


        idf.py fullclean >>${BUILDLOG} 2>&1 &&
		idf.py build >>${BUILDLOG} 2>&1 &&

        cat ${BUILDLOG}
    popd

    grep -i "error\|warning" "${BUILDLOG}" 2>&1 | grep -v "error.c.obj" >> "${LOG_SUSPECTED}" || :
}

EXAMPLE_NUM=0

echo "Current job will build example ${START_NUM} - ${END_NUM}"

for EXAMPLE_PATH in ${EXAMPLE_PATHS}
do
    if [[ $EXAMPLE_NUM -lt $START_NUM || $EXAMPLE_NUM -ge $END_NUM ]]
    then
        EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))
        continue
    fi
    echo ">>> example [ ${EXAMPLE_NUM} ] - $EXAMPLE_PATH"

    build_example "${EXAMPLE_NUM}" "${EXAMPLE_PATH}"

    EXAMPLE_NUM=$(( $EXAMPLE_NUM + 1 ))
done

# show warnings
echo -e "\nFound issues:"

#       Ignore the next messages:
# "error.o" or "-Werror" in compiler's command line
# "reassigning to symbol" or "changes choice state" in sdkconfig
# 'Compiler and toochain versions is not supported' from crosstool_version_check.cmake
IGNORE_WARNS="\
library/error\.o\
\|\ -Werror\
\|error\.d\
\|reassigning to symbol\
\|changes choice state\
\|crosstool_version_check\.cmake\
"

sort -u "${LOG_SUSPECTED}" | grep -v "${IGNORE_WARNS}" \
    && RESULT=$RESULT_ISSUES \
    || echo -e "\tNone"

[ -z ${FAILED_EXAMPLES} ] || echo -e "\nThere are errors in the next examples: $FAILED_EXAMPLES"
[ $RESULT -eq 0 ] || echo -e "\nFix all warnings and errors above to pass the test!"

echo -e "\nReturn code = $RESULT"

exit $RESULT
