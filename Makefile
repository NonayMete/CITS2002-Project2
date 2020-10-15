CC=cc
CFLAGS=-Wall -Werror -std=c99
DEPS = mergetars.h
NAME = mergetars.c
OTHER = fileop.c tarop.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mergetars: $(NAME) $(OTHER)
	$(CC) $(CFLAGS) $(NAME) -o $@

clean:
	rm -f $(NAME) f?