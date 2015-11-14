
PARSER_PATH = ./src/typechecker

C_FLAGS = -ll -lc

cminor : ${PARSER_PATH}/parser.tab.o ${PARSER_PATH}/scanner.o ${PARSER_PATH}/param_list.o ${PARSER_PATH}/decl.o ${PARSER_PATH}/expr.o ${PARSER_PATH}/stmt.o ${PARSER_PATH}/token.o ${PARSER_PATH}/type.o ${PARSER_PATH}/symbol.o ${PARSER_PATH}/scope.o ${PARSER_PATH}/hash_table.o ${PARSER_PATH}/main.o
	gcc ${PARSER_PATH}/parser.tab.o ${PARSER_PATH}/scanner.o ${PARSER_PATH}/param_list.o ${PARSER_PATH}/decl.o ${PARSER_PATH}/expr.o ${PARSER_PATH}/stmt.o ${PARSER_PATH}/token.o ${PARSER_PATH}/type.o ${PARSER_PATH}/symbol.o ${PARSER_PATH}/scope.o ${PARSER_PATH}/hash_table.o ${PARSER_PATH}/main.o -o $@ ${C_FLAGS}


${PARSER_PATH}/%.o: ${PARSER_PATH}/%.c ${PARSER_PATH}/*.h
	gcc -Wall -c $< -o $@ -g

${PARSER_PATH}/scanner.c : ${PARSER_PATH}/scanner.flex ${PARSER_PATH}/parser.tab.h
	flex -o${PARSER_PATH}/scanner.c ${PARSER_PATH}/scanner.flex

${PARSER_PATH}/parser.tab.c parser.tab.h : ${PARSER_PATH}/parser.bison
	yacc -d -b${PARSER_PATH}/parser -v ${PARSER_PATH}/parser.bison

clean:
	rm -f ${PARSER_PATH}/parser.tab.* ${PARSER_PATH}/parser.output ${PARSER_PATH}/scanner.c ${PARSER_PATH}/*.o cminor ./test/*.out

test: cminor ./test/* test.sh
	./test.sh

