#!/bin/bash

set -eu
cd "$(dirname $0)"

if test -d gh-pages/.git
then
  cd gh-pages
  git pull -r
  cd ..
else
  git clone -b gh-pages github.com:m6w6/libmemcached gh-pages
fi

mkdir -p gh-pages-build
cd gh-pages-build
cmake ..
make html
rsync -va --delete --exclude=.git/ docs/html/ ../gh-pages/

cd ../gh-pages
touch .nojekyll
git add -A
git ci -m "update docs"
git push

