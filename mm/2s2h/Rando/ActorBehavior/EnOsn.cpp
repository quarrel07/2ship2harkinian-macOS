#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnOsnBehavior() {
    COND_VB_SHOULD(VB_OSN_CONSIDER_ELIGIBLE_FOR_SONG_OF_HEALING, IS_RANDO, {
        *should = INV_CONTENT(ITEM_OCARINA_OF_TIME) == ITEM_OCARINA_OF_TIME &&
                  !RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING].obtained;
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OSN, IS_RANDO, {
        *should = false;

        RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_INTERIOR_DEKU_MASK].eligible = true;
        RANDO_SAVE_CHECKS[RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING].eligible = true;
    });
}
