.IGNORE:
all:
	gcc draft.c alice.c kaguya.c red_hood.c match-girl.c mulan.c snow_white.c cards.c effect_placeholders.c -o draft -lSDL2 -lSDL2_ttf -lSDL2_gfx -lm
