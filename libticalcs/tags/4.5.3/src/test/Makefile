all: tnode.o test.o
	gcc tnode.o test.c


tnode.o: tnode.c
	gcc -c tnode.c

test.o: test.c
	gcc -c test.c

clean: *.o
	rm -f *.o *.lo a.out *~