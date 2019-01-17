.PHONY: clean
TARGET=server client.exe client
all: $(TARGET)

INCLUDE=-I../include
BIN=../bin
CLIENTWINDOWS_OBJS=ClientWindows.o option.o
CLIENTMAC_OBJS=ClientMac.o option.o
SERVER_OBJS=Server.o option.o
CPPFLAG=-std=c++11
WINLIB=-lws2_32

%.o: %.c
	@echo "making: " $@
	@gcc $< -c $(CFLAG) $(INCLUDE)
%.o: %.cpp
	@echo "making: " $@
	g++ $< -c $(CPPFLAG) $(INCLUDE)

server: $(SERVER_OBJS)
	@echo "making: " $@
	@g++ -o $(BIN)/$@ $(CPPFLAG) $(INCLUDE) $(SERVER_OBJS)

client.exe:
	@echo "making: " $@
	@(echo 'gcc -c option.c $(INCLUDE) && g++ -c ClientWindows.cpp $(INCLUDE) && g++ -o $(BIN)/$@ $(CLIENTWINDOWS_OBJS) $(WINLIB) -Ofast -static' && echo exit) | /mnt/c/Windows/System32/cmd.exe
client: $(CLIENTMAC_OBJS)
	@echo "making: " $@
	@gcc -c option.c $(INCLUDE)
	@g++ -o $(BIN)/$@ $(CPPFLAG) $(INCLUDE) $(CLIENTMAC_OBJS)

clean:
	rm -f *.o
	@for bin in $(TARGET); do \
		echo "rm -f $$bin"; \
		rm -f $(BIN)/$$bin; \
	done

