.PHONY: all

all: sall-vm sall-as

sall-vm: src/vm/main.c src/vm/vm.c src/vm/utils.c src/shared.c include/sall.h include/vm.h
	$(CC) src/vm/main.c src/vm/vm.c src/vm/utils.c src/shared.c -o $@

sall-as: src/as.c src/shared.c include/sall.h
	$(CC) src/as.c src/shared.c -o $@

.PHONY: clean

clean:
	rm -f sall-*
	rm -f demo/*.bin	

.PHONY: demo

demo: demo/demo.bin demo/crash.bin demo/loop.bin

demo/demo.bin: demo/demo.sal sall-as
	./sall-as demo/demo.sal demo/demo.bin

demo/crash.bin: demo/crash.sal sall-as
	./sall-as demo/crash.sal demo/crash.bin

demo/loop.bin: demo/loop.sal sall-as
	./sall-as demo/loop.sal demo/loop.bin