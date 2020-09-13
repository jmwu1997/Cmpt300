#  Filename: Makefile
#  Created by Ivan Jonathan Hoo
#  Copyright (c) 2016 Ivan Jonathan Hoo. All rights reserved.

CC = gcc
CCFLAGS = -std=gnu99 -pthread -lrt -D_POSIX_C_SOURCE=199309L -g -Wall -Werror

SRC = candykids.c
EXE = candykids

# all: candykids

# candykids: $(SRC)
# 	$(CC) $(CCFLAGS) $(SRC) -o $(EXE)

# clean:
# 	rm -f candykids *.o

all: candykids

# Compile all .c files to .o files
%.o : %.c
	$(CC) -c $(CCFLAGS) $<

candykids: candykids.o bbuff.o stats.o
	$(CC) $(CCFLAGS) -o $(EXE) candykids.o bbuff.o stats.o

clean:
	rm -f core *.o candykids
#  Filename: Makefile
#  Created by Ivan Jonathan Hoo
#  Copyright (c) 2016 Ivan Jonathan Hoo. All rights reserved.

CC = gcc
CCFLAGS = -std=gnu99 -pthread -lrt -D_POSIX_C_SOURCE=199309L -g -Wall -Werror

SRC = candykids.c
EXE = candykids

# all: candykids

# candykids: $(SRC)
# 	$(CC) $(CCFLAGS) $(SRC) -o $(EXE)

# clean:
# 	rm -f candykids *.o

all: candykids

# Compile all .c files to .o files
%.o : %.c
	$(CC) -c $(CCFLAGS) $<

candykids: candykids.o bbuff.o stats.o
	$(CC) $(CCFLAGS) -o $(EXE) candykids.o bbuff.o stats.o

clean:
	rm -f core *.o candykids
