#Compiler
CC        = gcc

#Flags and includes for compiler
CFLAGS    = -Wall -Wundef -O2

#Targets
all: gbk_to_ibm1388.exe ibm1388_to_gbk.exe

gbk_to_ibm1388.exe: gbk_to_ibm1388.c
	@echo "==>Compile & Link gbk_to_ibm1388.c ..."
	$(CC) $(CFLAGS) -o $@ $<

ibm1388_to_gbk.exe: ibm1388_to_gbk.c
	@echo "==>Compile & Link ibm1388_to_gbk.c ..."
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f gbk_to_ibm1388.exe ibm1388_to_gbk.exe
