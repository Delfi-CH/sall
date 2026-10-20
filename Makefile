all: tvm tvm-as

tvm: src/vm.c src/shared.c
	$(CC) src/vm.c src/shared.c -o tvm

tvm-as: src/as.c src/shared.c
	$(CC) src/as.c src/shared.c -o tvm-as

.PHONY: clean

clean:
	rm -f tvm*