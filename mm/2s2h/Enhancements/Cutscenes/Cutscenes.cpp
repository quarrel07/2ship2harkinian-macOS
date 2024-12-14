#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipFallingMoonsTear();
    RegisterSkipGreatFairyCutscene();
    RegisterSkipKafeiReveal();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();
    RegisterSkipMayorsOfficeCutscene();
    RegisterSkipPushingMikau();
    RegisterSkipKamaroTeachDance();
    RegisterSkipTingleTime();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();
    SkipClockTowerSkullKidEncounter();
    RegisterSkipLearningElegyOfEmptiness();
    RegisterSkipLearningSongOfHealing();
    RegisterSkipLearningSongOfTime();
    RegisterSkipSongOfTimeCutscene();
    RegisterSkipMilkRunCutscenes();
    RegisterSkipBombBagTheftCutscene();
    RegisterSkipRosaSistersDance();
    RegisterSkipLearningSonataOfAwakening();
    RegisterSkipBossCutscenes();
    RegisterSkipGiantsChamber();
    RegisterSkipHealingDarmani();
    RegisterSkipHealingMikau();
    RegisterSkipHealingPamelasFather();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
    RegisterSkipToFileSelect();
}
