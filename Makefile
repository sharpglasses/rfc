all:main

main:aggregate_master.o main.o lst_map.o rb_map.o  rfc.o rfc_debug.o
	gcc $^ -o $@
%.o:%.c
	gcc -c $^

clean:
	rm main *.o
