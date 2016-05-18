#!/bin/sh
#convient for compiler expriment.

github='http://www.github.com/gybjm2016/Compiler.git'
if [ ! -x batch.sh ]; then
	chmod +x "batch.sh"
fi

if [ $# -lt 1 ]; then
	cat<<HELP
Illegal Use.
usage: sh batch.sh [ARGUMENTS]
   or: ./batch.sh [ARGUMENTS]
ARGUMENTS is one type of the followings:
    pull      pull all dates from "${github}" into master.
    commit    commit all changes to local git.
    submit    submit all local git to "${github}" by your account.
HELP
	exit 0
fi			

if [ $1 = "commit" ]; then
	cd Code
	make clean
	cd ..
	git add -A
	git commit -m $2
elif [ $1 = "submit" ]; then
	git push ${github} master
else
	if [ $1 = "pull" ]; then
		git pull ${github} master
	else
	  cat<<HELP
Illegal Use.
usage: sh batch.sh [ARGUMENTS]
   or: ./batch.sh [ARGUMENTS]
ARGUMENTS is one type of the followings:
    pull      pull all dates from "${github}" into master.
    commit    commit all changes to local git.
    submit    submit all local git to "${github}" by your account.
HELP
	fi
fi
