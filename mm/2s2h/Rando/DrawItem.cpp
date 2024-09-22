#include "Rando/Rando.h"
#include <libultraship/libultraship.h>
#include "Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "objects/gameplay_keep/gameplay_keep.h"
SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime);
}

void DrawStrayFairy(RandoItem item) {
    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Xlu(gPlayState->state.gfxCtx);

    switch (item) {
        case RI_WOODFALL_STRAY_FAIRY:
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyWoodfallTexAnim);
            break;

            // case STRAY_FAIRY_AREA_SNOWHEAD:
            //     AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairySnowheadTexAnim));
            //     break;

            // case STRAY_FAIRY_AREA_GREAT_BAY:
            //     AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyGreatBayTexAnim));
            //     break;

            // case STRAY_FAIRY_AREA_STONE_TOWER:
            //     AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyStoneTowerTexAnim));
            //     break;

        default: // STRAY_FAIRY_AREA_CLOCK_TOWN
            AnimatedMat_Draw(gPlayState, (AnimatedMaterial*)&gStrayFairyClockTownTexAnim);
            break;
    }

    Matrix_ReplaceRotation(&gPlayState->billboardMtxF);
    Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);

    Matrix_Push();
    Matrix_Translate(96.2f, 898.3f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(4556, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftWingDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(-80.2f, -465.2f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(37148, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftArmDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(-80.2f, 914.4f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(28212, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftWingDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(48.1f, -481.2f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(59928, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftArmDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(-128.3f, -754.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(47487, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftArmDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(112.2f, -770.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(51342, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftArmDL); // Renders
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(0.0f, -465.2f, 20.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyTorsoDL); // Renders
    Matrix_Pop();

    Matrix_Translate(0.0f, 0.0f, 25.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyLeftFacingHeadDL);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)&gStrayFairyGlowDL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::DrawItem(RandoItem item) {
    switch (item) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
        case RI_WOODFALL_STRAY_FAIRY:
            DrawStrayFairy(item);
            break;
        default:
            GetItem_Draw(gPlayState, Rando::StaticData::Items[item].drawId);
            break;
    }
}
