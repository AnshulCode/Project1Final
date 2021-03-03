ww: ww.o
	gcc -g -std=c99 -Wvla -Wall -o ww ww.o

ww.o: ww.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall ww.c -o  ww.o

# separate version of ww.o with debugging text enabled
dww: dww.o
	gcc -g -std=c99 -Wvla -Wall -o dww dww.o

dww.o: ww.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -DDEBUG -o dww.o ww.c

clean:
	rm -f *.o ww
