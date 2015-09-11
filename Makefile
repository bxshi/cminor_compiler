SCANNER_PATH = src/scanner

SCANNER_FILES = $(SCANNER_PATH)/token.c $(SCANNER_PATH)/token.h

OSX_FLAG = -ll
LINUX_FLAG = -lgl

flex: $(SCANNER_FILES) $(SCANNER_PATH)/scanner.l
	flex -o $(SCANNER_PATH)/lex.yy.c $(SCANNER_PATH)/scanner.l

osxbuild: flex
	gcc -c $(SCANNER_PATH)/lex.yy.c
	gcc -c ${SCANNER_FILES}
	gcc -o cminor lex.yy.o token.o $(OSX_FLAG)

clean:
	rm lex.yy.o cminor token.o $(SCANNER_PATH)/lex.yy.c $(SCANNER_PATH)/token.h.gch
