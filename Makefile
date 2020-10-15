PROJECT =  mergetars
HEADERS =  $(PROJECT).h
OBJ     =  mergetars.o fileop.o tarop.o

CC	= cc
CFLAGS	= -std=c99 -Wall -Werror

$(PROJECT) : $(OBJ)
	$(CC) $(CFLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
       rm -f $(PROJECT) $(OBJ)