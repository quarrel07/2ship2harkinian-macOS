#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnOsnBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OSN, IS_RANDO, {
        *should = false;

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_DEKU_MASK].eligible = true;
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_SONG_OF_HEALING].eligible = true;
    });
}
