cminor : parser.tab.o scanner.o param_list.o decl.o expr.o stmt.o token.o type.o main.o
	gcc decl.o expr.o stmt.o token.o type.o parser.tab.o scanner.o param_list.o main.o -o $@ -ll -lc


%.o: %.c *.h
	gcc -Wall -c $< -o $@

scanner.c : scanner.flex parser.tab.h
	flex -oscanner.c scanner.flex

parser.tab.c parser.tab.h : parser.bison
	yacc -d -bparser -v parser.bison

clean:
	rm -f parser.tab.* parser.output scanner.c *.o cminor
