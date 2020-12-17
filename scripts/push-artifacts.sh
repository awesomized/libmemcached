#!/usr/bin/env bash
# push artifacts to https://artifacts.m6w6.name/libmemcached/

if test -n "$PUSH_ARTIFACTS_ID"
then
  echo "$PUSH_ARTIFACTS_ID" > push-artifacts.id
  chmod 0600 push-artifacts.id
fi

RSYNC_CONF=$(dirname $0)/push-artifacts.conf
rsync -RPae "ssh -F $RSYNC_CONF" --exclude "_CPack_Packages" "$@" m6w6.name::artifacts/libmemcached/
