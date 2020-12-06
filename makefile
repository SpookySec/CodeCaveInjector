injectormake: code_cave_inject.c findcave.h
	gcc -o code_cave_inject code_cave_inject.c findcave.h
	gcc -o hello hello.c
	gcc -o find_code_cave find_code_cave.c

