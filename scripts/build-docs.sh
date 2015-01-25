#!/bin/bash

cd "$TRAVIS_BUILD_DIR/docs/html/"

git config --global user.email "travis@leops.me"
git config --global user.name "Travis-CI"

git checkout gh-pages

git config credential.helper "store --file=gitCredentials"
echo "https://${GH_TOKEN}:@github.com" > gitCredentials

cd "$TRAVIS_BUILD_DIR"

echo "Generating docs with doxygen..."

doxygen

cd "$TRAVIS_BUILD_DIR/docs/html/"

git add -A

rm -f ~/docs_need_commit
git status
git status | perl -ne '/modified:\s+(.*)/ and print "$1\n"' | while read x; do
        echo "Checking for useful changes: $x"
        git diff --cached $x |
                perl -ne '/^[-+]/ and !/^([-+])\1\1 / and !/^[-+]Généré.*GameEditor.*/ and exit 1' &&
                git reset -q $x ||
                { echo "Useful change detected"; touch ~/docs_need_commit; }
done

if [[ ! -f ~/docs_need_commit ]]; then
        echo "Docs at gh-pages are up to date."
        exit
fi

git commit -a -F- <<EOF
Latest docs on successful travis build $TRAVIS_BUILD_NUMBER
Commit $TRAVIS_COMMIT
EOF

git branch
git push origin gh-pages

echo "Published docs to gh-pages."
