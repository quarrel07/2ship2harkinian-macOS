#include "MiscBehavior.h"

extern "C" {
#include "variables.h"
}

// Entry point for the module, run once on game boot
void Rando::MiscBehavior::Init() {
    Rando::MiscBehavior::InitFileSelect();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(Rando::MiscBehavior::OnFileCreate);
}

void Rando::MiscBehavior::OnFileLoad() {
    Rando::MiscBehavior::CheckQueueReset();
    Rando::MiscBehavior::InitKaleidoItemPage();
    Rando::MiscBehavior::InitOfferGetItemBehavior();

    COND_HOOK(OnFlagSet, IS_RANDO, Rando::MiscBehavior::OnFlagSet);
    COND_HOOK(OnSceneFlagSet, IS_RANDO, Rando::MiscBehavior::OnSceneFlagSet);
    COND_HOOK(BeforeEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::BeforeEndOfCycleSave);
    COND_HOOK(AfterEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::AfterEndOfCycleSave);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });
}
