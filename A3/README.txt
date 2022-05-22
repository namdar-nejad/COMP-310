1. While executing the files passed to exec:
	If there is a fatal error, for example file problem or an invalid command, the executin stopes for all files.
	If the command is not invalid, for example calling print with a variable that doesn't exsit then the error massage shows and exec will continue. 

	NOTE: Printing a variable that doesn't exist is not a fatal error and the exec prosess will continue after showing the error.

2. The files created in the backing store only stay there until the next exec command and then get removed and replaiced with the new files.

3. Files that are longer than 40 line of code or 10 pages are not allowed to be passed to exec.

4. I occasionally got an error when compiling mykernel with make in mimi (error below), this can be solved if you just "make clean" and "make" again.

	make: *** [mykernel] Error 1

5. I geit a warning (added below) when compling the code, I couldn't find a solution to get rid of the warning, please avoid it.

kernel.c:262:30: warning: '/*' within block comment [-Wcomment]
        rm -rf ./BackingStore/* > /dev/null 2>&1
                             ^

6. The last line of the script passed to exec can be either empty or full (a command).

7. To run my test do:
	./mykernel < ./TESTDIR/TESTFILE.txt