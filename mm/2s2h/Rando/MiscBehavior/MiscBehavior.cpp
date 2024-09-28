#include "MiscBehavior.h"

extern "C" {
#include "variables.h"
}

// Entry point for the module, run once on game boot
void Rando::MiscBehavior::Init() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(Rando::MiscBehavior::OnFileCreate);
}

void Rando::MiscBehavior::OnFileLoad() {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onPlayerUpdateHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHook);

    onFlagSetHook = 0;
    onSceneFlagSetHook = 0;
    onPlayerUpdateHook = 0;

    Rando::MiscBehavior::CheckQueueReset();

    if (!IS_RANDO) {
        return;
    }

    onFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(Rando::MiscBehavior::OnFlagSet);
    onSceneFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(Rando::MiscBehavior::OnSceneFlagSet);
    onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_PLAYER, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });
}
