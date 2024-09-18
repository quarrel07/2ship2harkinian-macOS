#ifndef RANDO_STATIC_DATA_H
#define RANDO_STATIC_DATA_H

#include <unordered_map>
#include "Rando/Types.h"
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64item.h"
#include "z64scene.h"
}

namespace Rando {

namespace StaticData {

struct RandoCheckData {
    RandoCheck check;
    RandoCheckType type;
    SceneId scene;
    FlagType flagType;
    s32 flag;
    RandoItem item;
};

extern std::unordered_map<RandoCheck, RandoCheckData> Checks;

struct RandoItemData {
    const char* name;
    ItemId itemId;
    GetItemId getItemId;
    GetItemDrawId drawId;
};

extern std::unordered_map<RandoItem, RandoItemData> Items;

RandoCheckData GetCheckFromFlag(FlagType flagType, s32 flag, s16 scene = SCENE_MAX);

} // namespace StaticData

} // namespace Rando

#endif // RANDO_STATIC_DATA_H
