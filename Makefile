all: tinyvm tinyvm-as

tinyvm: src/vm.c src/shared.c
	$(CC) src/vm.c src/shared.c -o tinyvm

tinyvm-as: src/as.c src/shared.c
	$(CC) src/as.c src/shared.c -o tinyvm-as

.PHONY: clean

clean:
	rm -f tinyvm tinyvm-as