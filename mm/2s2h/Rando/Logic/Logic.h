#ifndef RANDO_LOGIC_H
#define RANDO_LOGIC_H

#include <unordered_map>
#include <map>
#include <set>
#include "Rando/Rando.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64item.h"
#include "z64scene.h"
}

namespace Rando {

namespace Logic {

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions);
void ApplyGlitchlessLogicToSaveContext();
void ApplyNoLogicToSaveContext();

struct RandoRegion {
    RandoRegionId randoRegionId;
    const char* name;
    SceneId sceneId;
    std::unordered_map<RandoCheckId, std::function<bool()>> checks;
    std::unordered_map<RandoRegionId, std::function<bool()>> regions;
};

extern std::map<RandoRegionId, RandoRegion> Regions;

} // namespace Logic

} // namespace Rando

#endif // RANDO_LOGIC_H
