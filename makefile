.IGNORE:
all:
	gcc draft.c declare.c cards.c -o draft -lSDL2 -lSDL2_ttf -lSDL2_gfx -lm
