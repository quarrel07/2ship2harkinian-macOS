#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

void Rando::ActorBehavior::InitEnItem00Behavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor, bool* should) {
        EnItem00* item00 = (EnItem00*)actor;

        // If it's one of our items ignore it
        if (item00->actor.params == ITEM00_NOTHING) {
            return;
        }

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                                    ENITEM00_GET_7F00(actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        // Pots handle their own items, ignore them
        if (randoStaticCheck.randoCheckType == RCTYPE_POT) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled) {
            return;
        }

        // Prevent the original item from spawning
        *should = false;

        // If it hasn't been collected yet, spawn a dummy item
        if (!randoSaveCheck.eligible) {
            CustomItem::Spawn(
                actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0, CustomItem::KILL_ON_TOUCH,
                randoStaticCheck.randoCheckId,
                [](Actor* actor, PlayState* play) {
                    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                    randoSaveCheck.eligible = true;
                },
                [](Actor* actor, PlayState* play) {
                    auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                    RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
                    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                    Rando::DrawItem(randoItemId);
                });
        }
    });
}
