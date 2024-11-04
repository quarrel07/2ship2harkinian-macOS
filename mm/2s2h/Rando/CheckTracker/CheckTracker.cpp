
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"

namespace Rando {

namespace CheckTracker {

void Window::DrawElement() {
    std::set<RandoRegionId> reachableRegions = { RR_CLOCK_TOWN_SOUTH };
    Rando::Logic::FindReachableRegions(RR_CLOCK_TOWN_SOUTH, reachableRegions);

    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        std::vector<RandoCheckId> availableChecks;

        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc()) {
                availableChecks.push_back(randoCheckId);
            }
        }

        if (availableChecks.size() > 0) {
            std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
            if (randoRegion.name != "") {
                regionName += " - ";
                regionName += randoRegion.name;
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
