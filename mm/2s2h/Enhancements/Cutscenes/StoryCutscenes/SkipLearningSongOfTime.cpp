#include <libultraship/bridge.h>
#include <spdlog/spdlog.h>
#include "Enhancements/GameInteractor/GameInteractor.h"
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "assets/objects/object_gi_melody/object_gi_melody.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

void RegisterSkipLearningSongOfTime() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(
        GI_VB_PLAY_TRANSITION_CS, [](GIVanillaBehavior _, bool* should, void* opt) {
            // Cutscene that plays after picking up the Ocarina of Time.
            if (gSaveContext.save.entrance == ENTRANCE(CUTSCENE, 0) && gSaveContext.save.cutsceneIndex == 0xFFF2 &&
                CVarGetInteger("gEnhancements.Cutscenes.SkipStoryCutscenes", 0)) {
                gSaveContext.save.playerForm = PLAYER_FORM_DEKU;
                gSaveContext.save.entrance = ENTRANCE(CLOCK_TOWER_ROOFTOP, 1);
                gSaveContext.save.cutsceneIndex = 0;
                GameInteractor::Instance->events.emplace_back(
                    GIEventGiveItem{ .showGetItemCutscene = true,
                                     .getItemText = "You recieved the Song of Time",
                                     .drawItem =
                                         []() {
                                             OPEN_DISPS(gPlayState->state.gfxCtx);

                                             Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);

                                             gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx),
                                                       G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                                             gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 255);
                                             gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiSongNoteDL);

                                             CLOSE_DISPS(gPlayState->state.gfxCtx);
                                         },
                                     .giveItem = []() { SET_QUEST_ITEM(QUEST_SONG_TIME); } });
            }
        });
}
