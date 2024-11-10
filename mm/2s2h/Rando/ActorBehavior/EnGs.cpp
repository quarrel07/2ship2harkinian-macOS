#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

#include <vector>

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define FIRST_GS_MESSAGE 0x20D1
#define SECOND_GS_MESSAGE 0x20C0

// A simple dynamic hinting system, cost doubles each time it's used.
// This isn't currently in use. Committed for reference, may iterate on it later
void Rando::ActorBehavior::InitEnGsBehavior() {
    static int cost = 10;

    COND_ID_HOOK(OnOpenText, FIRST_GS_MESSAGE, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        entry.msg = "Trade {{rupees}} Rupees for a hint?\n\xC2Yes\nNo";

        CustomMessage::Replace(&entry.msg, "{{rupees}}", std::to_string(cost));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, SECOND_GS_MESSAGE, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        MessageContext* msgCtx = &gPlayState->msgCtx;

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (msgCtx->choiceIndex == 0) {
            if (gSaveContext.save.saveInfo.playerData.rupees < cost) {
                entry.msg = "Foolish... You don't have enough rupees...";
            } else {
                std::vector<RandoCheckId> availableChecks;
                for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
                    RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    if (!saveCheck.obtained && saveCheck.shuffled) {
                        availableChecks.push_back(randoCheckId);
                    }
                }
                RandoCheckId checkId = availableChecks[rand() % availableChecks.size()];
                RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[checkId];

                entry.msg = "Wise choice... They say there {{item}} is hidden at {{location}}.";

                CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::Items[saveCheck.randoItemId].name);
                CustomMessage::Replace(&entry.msg, "{{location}}", Rando::StaticData::Checks[checkId].name);

                gSaveContext.rupeeAccumulator -= cost;
                cost *= 2;
            }
        } else {
            entry.msg = "Foolish... Come back later when you have more sense.";
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
