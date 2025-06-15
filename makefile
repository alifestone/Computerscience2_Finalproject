.IGNORE:
all:
	gcc draft.c alice.c kaguya.c red_hood.c match-girl.c mulan.c snow_white.c cards.c effect_placeholders.c -o draft -lSDL2 -lSDL2_ttf -lSDL2_gfx -lm
	gcc aivsai.c alice.c red_hood.c match-girl.c snow_white.c mulan.c kaguya.c cards.c aifables.c effect_placeholders.c -o aivsai -lSDL2 -lSDL2_ttf -lSDL2_gfx