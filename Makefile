CC=cc
CFLAGS=-Wall -Werror -std=c99
NAME = mergetars
DEPS = mergetars.h
OTHER = fileop.c tarop.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(NAME).c $(OTHER)
	$(CC) $(CFLAGS) $(NAME).c -o $@

clean:
	rm -f $(NAME) f?