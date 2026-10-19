all: tinyvm tinyvm-as

tinyvm:
	$(CC) src/vm.c -o tinyvm

tinyvm-as:
	$(CC) src/as.c -o tinyvm-as

.PHONY: clean

clean:
	rm -f tinyvm tinyvm-as