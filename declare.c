#include "declare.h"
Card potshot={"Pot\nShot",SKILL_ATK,0,1,0,0,1,true,NULL};
Card crackshot={"Crack\nShot",SKILL_ATK,2,2,0,0,2,true,NULL};
Card overdrive_burn={
    .name="Overdrive\nBurn",
    .type=SKILL_ATK,
    .cst=0,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card sniper_shot={
    .name="Sniper\nShot",
    .type=SKILL_ATK,
    .cst=4,
    .dmg=3,
    .defense=0,
    .mov=0,
    .rng=3,
    .link=true,
    .effect=NULL //請填入效果函式
};
Card onboard_cache={
    .name="Onboard\nCache",
    .type=SKILL_ATK,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card energy_shield={
    .name="Energy\nShield",
    .type=SKILL_DEF,
    .cst=1,
    .dmg=0,
    .defense=3,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card electrified_shield={
    .name="Electrified\nShield",
    .type=SKILL_DEF,
    .cst=2,
    .dmg=0,
    .defense=4,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card hooded_system={
    .name="Hooded\nSystem",
    .type=SKILL_DEF,
    .cst=1,
    .dmg=0,
    .defense=5,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card omega_shield={
    .name="Omega\nShield",
    .type=SKILL_DEF,
    .cst=3,
    .dmg=0,
    .defense=6,
    .mov=0,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_ballistics={
    .name="Unleashed\nBallistics",
    .type=SKILL_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=2,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_firepower={
    .name="Unleashed\nFirepower",
    .type=SKILL_MOV,
    .cst=2,
    .dmg=0,
    .defense=0,
    .mov=3,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card transformed_senses={
    .name="Transformed\nSenses",
    .type=SKILL_MOV,
    .cst=1,
    .dmg=0,
    .defense=0,
    .mov=1,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Card unleashed_fury={
    .name="Unleashed\nFury",
    .type=SKILL_MOV,
    .cst=3,
    .dmg=0,
    .defense=0,
    .mov=4,
    .rng=0,
    .link=false,
    .effect=NULL //請填入效果函式
};
Fable red_hood={
    .name="Red Riding Hood",
    .Piece={255, 0, 0, 255},
    .health=30,
    .energy=25,
    .defense=6,
    .epic_threshold=15,
    .lane=0,
    .skill[0]={
        .cards={&potshot, &crackshot, &overdrive_burn, &sniper_shot, &onboard_cache},
        .cnt=5
    },
    .skill[1]={
        .cards={&energy_shield, &electrified_shield, &hooded_system, &omega_shield, &onboard_cache},
        .cnt=5
    },
    .skill[2]={
        .cards={&unleashed_ballistics, &unleashed_firepower, &transformed_senses, &unleashed_fury, &onboard_cache},
        .cnt=5
    },
};
