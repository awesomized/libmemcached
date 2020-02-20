#!/bin/bash

set -eu
cd "$(dirname $0)"

if test -d pages/.git
then
  cd pages
  git pull -r
  cd ..
else
  git clone -b gh-pages github.com:m6w6/libmemcached pages
fi

mkdir -p build
cd build
cmake -DBUILD_DOCSONLY=true ../../..
make html
rsync -va --delete --exclude=.git/ docs/html/ ../pages/

cd ../pages
touch .nojekyll
git add -A
git ci -m "update docs"
git push

