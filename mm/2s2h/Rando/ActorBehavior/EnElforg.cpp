#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"

void EnElforg_SpawnSparkles(EnElforg* thisx, PlayState* play, s32 life);
}

void EnElforg_DrawCustom(Actor* thisx, PlayState* play) {
    EnElforg* enElforg = (EnElforg*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    RandoCheck check = RC_UNKNOWN;

    if (STRAY_FAIRY_TYPE(thisx) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
        check = RC_CLOCK_TOWN_STRAY_FAIRY;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_COLLECTIBLE) {
        auto checkData = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                             STRAY_FAIRY_GET_FLAG(&enElforg->actor), play->sceneId);
        check = checkData.check;
    } else if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_FREE_FLOATING) {
        auto checkData = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_SWITCH,
                                                             STRAY_FAIRY_GET_FLAG(&enElforg->actor), play->sceneId);
        check = checkData.check;
    }

    if (check == RC_UNKNOWN) {
        return;
    }

    auto checkSaveData = gSaveContext.save.shipSaveInfo.rando.checks[check];

    EnElforg_SpawnSparkles(enElforg, play, 16);
    thisx->shape.rot.y = thisx->shape.rot.y + 960;

    GetItem_Draw(play, Rando::StaticData::Items[checkSaveData.item].drawId);
}

void Rando::ActorBehavior::InitEnElforgBehavior(bool isRando) {
    static uint32_t onActorInitHookId = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInitHookId);

    onActorInitHookId = 0;

    if (!isRando) {
        return;
    }

    onActorInitHookId =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_EN_ELFORG, [](Actor* actor) {
            EnElforg* enElforg = (EnElforg*)actor;

            // Currently this will override the draw of all stray fairies, but we should probably add a check for the
            // ones we support until we capture all of them in the Checks static data.
            if (STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN ||
                STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_FREE_FLOATING ||
                STRAY_FAIRY_TYPE(&enElforg->actor) == STRAY_FAIRY_TYPE_COLLECTIBLE) {
                enElforg->actor.draw = EnElforg_DrawCustom;
            }
        });
}
