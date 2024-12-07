#include "Rando/Rando.h"
#include <libultraship/libultraship.h>
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_gi_melody/object_gi_melody.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
}

void DrawStrayFairy(RandoItemId randoItemId) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);

    switch (randoItemId) {
        case RI_WOODFALL_STRAY_FAIRY:
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyWoodfallTexAnim);
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairySnowheadTexAnim);
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyGreatBayTexAnim);
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyStoneTowerTexAnim);
            break;
        default: // STRAY_FAIRY_AREA_CLOCK_TOWN
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyClockTownTexAnim);
            break;
    }

    Matrix_ReplaceRotation(&gPlayState->billboardMtxF);
    Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);

    // Kind of a hack to draw the stray fairy, the drawback of this is that all stray fairies in the scene will animate
    // together, but worse is that the more there are the faster their animation will play (because of the
    // SkelAnime_Update below). This is still better than the previous solution which hand drew the fairy with DL
    // calls...
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[STRAY_FAIRY_LIMB_MAX];
    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(gPlayState, &skelAnime, (FlexSkeletonHeader*)&gStrayFairySkel,
                           (AnimationHeader*)&gStrayFairyFlyingAnim, jointTable, jointTable, STRAY_FAIRY_LIMB_MAX);
    }
    SkelAnime_Update(&skelAnime);
    POLY_XLU_DISP = SkelAnime_DrawFlex(gPlayState, skelAnime.skeleton, skelAnime.jointTable, skelAnime.dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void DrawSong(RandoItemId randoItemId) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch (randoItemId) {
        case RI_SONG_SUN:
            gDPSetEnvColor(POLY_XLU_DISP++, 237, 231, 62, 255);
            break;
        case RI_SONG_TIME:
            gDPSetEnvColor(POLY_XLU_DISP++, 98, 177, 211, 255);
            break;
        case RI_SONG_HEALING:
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 192, 203, 255);
            break;
        case RI_SONG_SONATA:
            gDPSetEnvColor(POLY_XLU_DISP++, 98, 255, 98, 255);
            break;
        case RI_SONG_ELEGY:
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 98, 0, 255);
            break;
        case RI_SONG_OATH:
            gDPSetEnvColor(POLY_XLU_DISP++, 98, 0, 98, 255);
            break;
        default:
            break;
    }

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gGiSongNoteDL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void DrawDoubleDefense() {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetGrayscaleColor(POLY_XLU_DISP++, 255, 255, 255, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gGiHeartBorderDL);
    gDPSetGrayscaleColor(POLY_XLU_DISP++, 255, 0, 0, 100);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gGiHeartContainerDL);
    gSPGrayscale(POLY_XLU_DISP++, false);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::DrawItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_SONG_TIME:
        case RI_SONG_SUN:
        case RI_SONG_HEALING:
        case RI_SONG_SONATA:
        case RI_SONG_ELEGY:
        case RI_SONG_OATH:
            DrawSong(randoItemId);
            break;
        case RI_CLOCK_TOWN_STRAY_FAIRY:
        case RI_WOODFALL_STRAY_FAIRY:
        case RI_SNOWHEAD_STRAY_FAIRY:
        case RI_GREAT_BAY_STRAY_FAIRY:
        case RI_STONE_TOWER_STRAY_FAIRY:
            DrawStrayFairy(randoItemId);
            break;
        case RI_DOUBLE_DEFENSE:
            DrawDoubleDefense();
            break;
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
            Rando::DrawItem(Rando::ConvertItem(randoItemId));
            break;
        case RI_NONE:
        case RI_UNKNOWN:
            break;
        default:
            GetItem_Draw(gPlayState, Rando::StaticData::Items[randoItemId].drawId);
            break;
    }
}
