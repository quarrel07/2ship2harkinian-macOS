#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Dm_Char02/z_dm_char02.h"

void DmChar02_PlaySfxForCutscenes(DmChar02* dmChar02, PlayState* play);
}

void DmChar02_UpdateCustom(Actor* actor, PlayState* play) {
    DmChar02* dmChar02 = (DmChar02*)actor;

    SkelAnime_Update(&dmChar02->skelAnime);
    dmChar02->unk_2F0 = dmChar02->unk_2F0;
    dmChar02->actionFunc(dmChar02, play);
    if ((actor->xzDistToPlayer <= 30.0f) && (fabsf(actor->playerHeightRel) <= fabsf(80.0f))) {
        Actor_Kill(&dmChar02->actor);

        auto& randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        randoSaveCheck.eligible = true;
        auto& randoSaveCheck2 = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_SONG_OF_TIME];
        randoSaveCheck2.eligible = true;
    }

    DmChar02_PlaySfxForCutscenes(dmChar02, play);
}

// This handles the two checks for the Clock Tower Roof, the Ocarina and Song of Time checks. It also handles
// overriding the drawing of the Ocarina in the hand of the Skull Kid.
void Rando::ActorBehavior::InitDmStkBehavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_DM_CHAR02, IS_RANDO,
                 [](Actor* actor, bool* should) { actor->update = DmChar02_UpdateCustom; });

    COND_VB_SHOULD(VB_DRAW_OCARINA_IN_STK_HAND, IS_RANDO, {
        if (*should) {
            *should = false;

            Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
            Vec3s rot;
            rot.x = -19737;
            rot.y = 31100;
            rot.z = 8674;
            Vec3f pos;
            pos.x = -26.5f;
            pos.y = -50.8f;
            pos.z = 10.9f;
            Matrix_TranslateRotateZYX(&pos, &rot);

            auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_CLOCK_TOWER_ROOF_OCARINA));
        }
    });

    COND_VB_SHOULD(VB_OVERRIDE_CHAR02_LIMB, IS_RANDO, {
        Gfx** dList = va_arg(args, Gfx**);

        *dList = NULL;
    });

    COND_VB_SHOULD(VB_POST_CHAR02_LIMB, IS_RANDO, {
        Matrix_Scale(15.0f, 15.0f, 15.0f, MTXMODE_APPLY);
        Vec3s rot;
        rot.x = -11554;
        Vec3f pos;
        pos.x = 10.4f;
        pos.y = -26.5f;
        pos.z = 13.2f;
        Matrix_TranslateRotateZYX(&pos, &rot);

        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_CLOCK_TOWER_ROOF_OCARINA));
    });

    COND_VB_SHOULD(VB_STK_HAVE_OCARINA, IS_RANDO, {
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_ROOF_OCARINA];
        *should = !randoSaveCheck.obtained;
    });
}
