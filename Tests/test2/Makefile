SRC = bump_alloc.c
OUT = bump_alloc.out

CC = cc

CFLAGS = -g -O0 -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm
ARGS ?=

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

docker:
	docker run --platform linux/amd64 -tiv "$(CURDIR):/valgrind" karek/valgrind:latest

memcheck: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUT) $(ARGS)

clean:
	rm -rf $(OUT) *.dSYM *.o