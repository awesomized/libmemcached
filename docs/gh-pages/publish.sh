#!/bin/bash

set -eu
cd "$(dirname $0)"

if test -d source/.git
then
  cd source
  git pull -r
  cd ..
else
  git clone -b gh-pages github.com:m6w6/libmemcached source
fi

mkdir -p build
cd build
cmake ../..
make html
rsync -va --delete --exclude=.git/ docs/html/ ../source/

cd ../source
touch .nojekyll
git add -A
git ci -m "update docs"
git push

