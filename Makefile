gotodie: board.c board.h gtp.c gtp.h uct.c uct.h gotodie.c gotodie.h interface.c
	gcc -lm board.c gtp.c uct.c gotodie.c interface.c -o gotodie

