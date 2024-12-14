#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "functions.h"
}

void RegisterSkipTingleTime() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_EN_BAL, [](Actor* actor, bool* should) {
            if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_TINGLE);
            }
        });
}
