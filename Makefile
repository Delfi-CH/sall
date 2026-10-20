all: sall-vm sall-as

sall-vm: src/vm.c src/shared.c
	$(CC) src/vm.c src/shared.c -o $@

sall-as: src/as.c src/shared.c
	$(CC) src/as.c src/shared.c -o $@

.PHONY: clean

clean:
	rm -f sall-*