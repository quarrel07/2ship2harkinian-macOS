
#include "CheckTracker.h"
#include "2s2h/ShipUtils.h"

namespace Rando {

namespace CheckTracker {

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions) {
    auto& randoStaticRegion = Rando::StaticData::Regions[currentRegion];

    for (auto& [neighborRegionId, accessLogicFunc] : randoStaticRegion.regions) {
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(neighborRegionId) == 0 && accessLogicFunc()) {
            reachableRegions.insert(neighborRegionId);                // Mark region as visited
            FindReachableRegions(neighborRegionId, reachableRegions); // Recursively visit neighbors
        }
    }
}

void Window::DrawElement() {
    std::set<RandoRegionId> reachableRegions = { RR_CLOCK_TOWN_SOUTH };
    FindReachableRegions(RR_CLOCK_TOWN_SOUTH, reachableRegions);

    for (RandoRegionId regionId : reachableRegions) {
        auto& randoStaticRegion = Rando::StaticData::Regions[regionId];
        std::vector<RandoCheckId> availableChecks;

        for (auto& [randoCheckId, accessLogicFunc] : randoStaticRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc()) {
                availableChecks.push_back(randoCheckId);
            }
        }

        if (availableChecks.size() > 0) {
            std::string regionName = Ship_GetSceneName(randoStaticRegion.sceneId);
            if (randoStaticRegion.name != "") {
                regionName += " - ";
                regionName += randoStaticRegion.name;
            }
            ImGui::SeparatorText(regionName.c_str());

            for (RandoCheckId checkId : availableChecks) {
                auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                ImGui::Text("%s", randoStaticCheck.name);
            }
        }
    }
}

void SettingsWindow::DrawElement() {
    ImGui::Text("Check Tracker Settings");
}

} // namespace CheckTracker

} // namespace Rando
