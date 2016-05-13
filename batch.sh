cd Code
make clean
cd ..
git status
git add -A
echo "Finished adding all changes into your local git."
echo -n "Please input the summary for this submit:"
read summary
if [$summary = "\n"]
then
	echo "Because of empty input for submit summary, don't finish push work."
else
	git commit -m "$summary"
	git push "http://github.com/gybjm2016/Compiler.git" master
fi


