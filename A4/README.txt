1. The implementation is non-contigues.

to test: ./mykernel < ../TESTFILE.txt
My testfile demonstates the non-contigues allocation for p2.
Run the test file, and after that see the p2 in the partition,
you can see that hello1234 (value for cat x1p2) is allocated non-contiguesly.

2. Don't use '*' in the data section.

I am using '*' as the default char in the data section and not '0',
to be able to recognize numbers correctly.
So my implementation will not work correctly for strings with '*' in them.

4. Note that due to the implementation of the parser in the base code,
you will not be able to provide a data with more than 8 space separated
sections to write

3. Please provide complete feedback it I loose any marks.