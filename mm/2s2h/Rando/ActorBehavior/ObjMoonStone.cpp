#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_Obj_Moon_Stone/z_obj_moon_stone.h"
}

void ObjMoonstone_DrawCustom(Actor* thisx, PlayState* play) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_ASTRAL_OBSERVATORY_MOON_TEAR];

    Matrix_Translate(0.0f, 10.0f, 0.0f, MTXMODE_APPLY);

    thisx->shape.rot.y = thisx->shape.rot.y + 960;

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_ASTRAL_OBSERVATORY_MOON_TEAR));
}

void Rando::ActorBehavior::InitObjMoonStoneBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_MOON_STONE, IS_RANDO, [](Actor* actor) {
        actor->world.pos.z += 5.0f;
        actor->draw = ObjMoonstone_DrawCustom;
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_MOONS_TEAR, IS_RANDO, {
        ObjMoonStone* objMoonStone = va_arg(args, ObjMoonStone*);
        if (objMoonStone->actor.xzDistToPlayer < 25.0f) {
            *should = false;
            SET_WEEKEVENTREG(WEEKEVENTREG_74_40);
            Actor_Kill(&objMoonStone->actor);
        }
    });
}
