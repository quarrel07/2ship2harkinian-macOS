#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipGreatFairyCutscene();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();
    SkipClockTowerSkullKidEncounter();
    RegisterSkipLearningSongOfHealing();
    RegisterSkipLearningSongOfTime();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
    RegisterSkipToFileSelect();
}
