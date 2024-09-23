#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipGreatFairyCutscene();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();
    RegisterSkipLearningSongOfTime();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
    RegisterSkipToFileSelect();
}
