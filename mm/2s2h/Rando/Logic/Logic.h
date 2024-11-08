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
RandoRegionId GetRegionIdFromEntrance(s32 entrance);
void ApplyGlitchlessLogicToSaveContext();
void ApplyNoLogicToSaveContext();

struct RandoEvent {
    std::function<void()> onApply;
    std::function<void()> onRemove;
    std::function<bool()> condition;
    std::string conditionString;
};

struct RandoRegion {
    RandoRegionId randoRegionId;
    const char* name;
    SceneId sceneId;
    std::vector<RandoEvent> events;
    std::unordered_map<RandoCheckId, std::pair<std::function<bool()>, std::string>> checks;
    std::set<s32> entrances;
    std::unordered_map<s32, std::pair<std::function<bool()>, std::string>> exits;
    std::unordered_map<RandoRegionId, std::pair<std::function<bool()>, std::string>> connections;
};

extern std::map<RandoRegionId, RandoRegion> Regions;

} // namespace Logic

} // namespace Rando

#endif // RANDO_LOGIC_H
