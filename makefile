# Philip Massouh

lab4.zip: makefile *.c *.h README
	zip $@ $^
	rm -rf install
	mkdir install
	unzip $@ -d install
	make -C install lab4

# -D_POSIX_C_SOURCE=199309L for better timekeepingm 
# simply using clock() measures CPU time which will be increased with multithreads
matrix: matrix.c
	gcc matrix.c -o exec_matrix -D_POSIX_C_SOURCE=199309L -O1 -Wall -ansi -pedantic -Wimplicit-function-declaration -Wreturn-type -lpthread





