ww1: ww1.o
	gcc -g -std=c99 -Wvla -Wall -o ww1 ww1.o

ww1.o: ww1.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall ww1.c -o  ww1.o

# separate version of ww1.o with debugging text enabled
dww1: dww1.o
	gcc -g -std=c99 -Wvla -Wall -o dww1 dww1.o

dww1.o: ww1.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -DDEBUG -o dww1.o ww1.c

clean:
	rm -f *.o ww1
