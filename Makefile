SCANNER_PATH = src/scanner

SCANNER_FILES = $(SCANNER_PATH)/token.c $(SCANNER_PATH)/token.h

RESULT_PREFIX = result/good

OSX_FLAG = -ll -lc
LINUX_FLAG = -lgl

flex: $(SCANNER_FILES) $(SCANNER_PATH)/scanner.l
	flex -o $(SCANNER_PATH)/lex.yy.c $(SCANNER_PATH)/scanner.l

osxbuild: flex
	gcc -c $(SCANNER_PATH)/lex.yy.c
	gcc -c ${SCANNER_FILES}
	gcc -o cminor lex.yy.o token.o $(OSX_FLAG)

test: osxbuild cminor
	./cminor -scan test/good0.cminor > good0.test
	diff $(RESULT_PREFIX)0.result good0.test
	./cminor -scan test/good1.cminor > good1.test
	diff $(RESULT_PREFIX)1.result good1.test
	./cminor -scan test/good2.cminor > good2.test
	diff $(RESULT_PREFIX)2.result good2.test
clean:
	rm lex.yy.o cminor token.o $(SCANNER_PATH)/lex.yy.c $(SCANNER_PATH)/token.h.gch
	rm *.test
