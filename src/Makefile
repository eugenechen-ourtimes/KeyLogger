.PHONY: clean
TARGET=server client.exe
all: $(TARGET)

INCLUDE=-I../include
BIN=../bin
CLIENT_OBJS=Client.o option.o
SERVER_OBJS=Server.o option.o
CPPFLAG=-std=c++11
LIB=-lws2_32

%.o: %.c
	@echo "making: " $@
	@gcc $< -c $(CFLAG) $(INCLUDE)
%.o: %.cpp
	@echo "making: " $@
	@g++ $< -c $(CPPFLAG) $(INCLUDE) $(LIB)

server: $(SERVER_OBJS)
	@echo "making: " $@
	@g++ -o $(BIN)/$@ $(CPPFLAG) $(INCLUDE) $(SERVER_OBJS)

client.exe:
	@echo "making: " $@
	@(echo 'gcc -c option.c $(INCLUDE) && g++ -c Client.cpp $(INCLUDE) && g++ -o $(BIN)/$@ $(CLIENT_OBJS) $(LIB) -m64  -Ofast -static' && echo exit) | /mnt/c/Windows/System32/cmd.exe

clean:
	rm -f *.o
	@for bin in $(TARGET); do \
		echo "rm -f $$bin"; \
		rm -f $(BIN)/$$bin; \
	done

