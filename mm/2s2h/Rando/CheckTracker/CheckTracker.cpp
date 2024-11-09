
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"

namespace Rando {

namespace CheckTracker {

void Window::DrawElement() {
    std::set<RandoRegionId> reachableRegions = {};
    // Get connected entrances from starting & warp points
    Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
    // Get connected regions from current entrance (TODO: Make this optional)
    Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance), reachableRegions);

    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        std::vector<std::pair<RandoCheckId, std::string>> availableChecks;

        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc.first()) {
                availableChecks.push_back({ randoCheckId, accessLogicFunc.second });
            }
        }

        if (availableChecks.size() > 0) {
            std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
            if (randoRegion.name != "") {
                regionName += " - ";
                regionName += randoRegion.name;
            }
            ImGui::SeparatorText(regionName.c_str());

            for (auto& [checkId, accessLogicString] : availableChecks) {
                auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                ImGui::Text("%s", randoStaticCheck.name);
                if (accessLogicString != "") {
                    UIWidgets::Tooltip(accessLogicString.c_str());
                }
            }
        }
    }
}

void SettingsWindow::DrawElement() {
    ImGui::Text("Check Tracker Settings");
}

} // namespace CheckTracker

} // namespace Rando
