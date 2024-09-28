#include "MiscBehavior.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
}

static bool queued = false;
void Rando::MiscBehavior::CheckQueue() {
    if (queued) {
        return;
    }

    for (auto& randoSaveCheck : RANDO_SAVE_CHECKS) {
        if (randoSaveCheck.eligible && !randoSaveCheck.obtained) {
            queued = true;

            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .getItemText = Rando::StaticData::Items[randoItemId].name,
                .drawItem = [randoItemId]() { Rando::DrawItem(randoItemId); },
                .giveItem =
                    [&randoSaveCheck, randoItemId]() {
                        Rando::GiveItem(randoItemId);
                        randoSaveCheck.obtained = true;
                        queued = false;
                    } });
            return;
        }
    }
}

void Rando::MiscBehavior::CheckQueueReset() {
    queued = false;
    GameInteractor::Instance->currentEvent = GIEventNone{};
    GameInteractor::Instance->events.clear();
}
