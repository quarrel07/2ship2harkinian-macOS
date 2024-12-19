#include "ActorBehavior.h"

extern "C" {
void Flags_SetRandoInf(s32 flag);
}

// When EnTsn is initialized this means the player should now be considered to have access to the Seahorse. This flag
// is purely to drive the live updating check tracker, it has no effect on actual gameplay.
void Rando::ActorBehavior::InitEnTsnBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_TSN, IS_RANDO, [](Actor* actor) {
        if (!RANDO_ACCESS[RANDO_ACCESS_SEAHORSE] && RANDO_ACCESS[RANDO_ACCESS_ZORA_EGG]) {
            RANDO_ACCESS[RANDO_ACCESS_SEAHORSE]++;
        }
    });
}
