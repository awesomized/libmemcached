#!/usr/bin/env bash
REPO=${GITHUB_REPOSITORY}

REF=$(basename ${GITHUB_REF})
REF_URL=https://github.com/${REPO}/commits/${REF}

BUILD_URL=https://github.com/${REPO}/actions/runs/${GITHUB_RUN_ID}
BUILD_ENV=${ImageOS}/${CC:-${CC_VND}-${CC_VER}}

case "$1" in
1|true|success)
	level=info
	status=success
	;;
*)
	level=error
	status=failure
	;;
esac
message="Github [${REPO}](${REF_URL}) (${REF}) [${status}](${BUILD_URL}) (${BUILD_ENV})"

curl -sS "${GITTER}" --data-urlencode "level=${level}" --data-urlencode "message=${message}"

