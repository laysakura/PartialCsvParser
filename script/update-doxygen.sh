#!/bin/sh
basedir="$(cd $(dirname $0) && pwd)"/..
(cd $basedir ; rm -rf doc/html/ ; doxygen Doxyfile && ~/git/ghp-import/ghp-import -p doc/html)
