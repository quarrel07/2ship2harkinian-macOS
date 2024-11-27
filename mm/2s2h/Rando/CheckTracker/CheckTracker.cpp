
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/Rando/StaticData/StaticData.h"
#include <sstream>

bool showLogic = false;
bool hideCollected = false;
bool expandHeaders = true;
bool expandedheaderState = true;

std::map<SceneId, std::vector<RandoCheckId>> sceneChecks;

std::unordered_map<RandoCheckId, std::string> readableCheckNames;

uint32_t getSumOfObtainedChecks(std::vector<RandoCheckId>& checks) {
    uint32_t collected = 0;
    for (RandoCheckId checkId : checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
        if (randoSaveCheck.obtained) {
            collected++;
        }
    }
    return collected;
}

void initializeSceneChecks() {
    sceneChecks.clear();
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
        if (!randoSaveCheck.shuffled) {
            continue;
        }

        SceneId sceneId = randoStaticCheck.sceneId;
        sceneChecks[sceneId].push_back(randoStaticCheck.randoCheckId);
    }
}

bool checkTrackerShouldShowRow(bool obtained) {
    bool showCheck = true;
    if (hideCollected && obtained) {
        showCheck = false;
    }
    return showCheck;
}

namespace Rando {

namespace CheckTracker {

void Window::DrawElement() {
    if (showLogic) {
        std::set<RandoRegionId> reachableRegions = {};
        // Get connected entrances from starting & warp points
        Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions);
        // Get connected regions from current entrance (TODO: Make this optional)
        Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                           reachableRegions);

        for (RandoRegionId regionId : reachableRegions) {
            auto& randoRegion = Rando::Logic::Regions[regionId];
            std::vector<std::pair<RandoCheckId, std::string>> availableChecks;
            uint32_t obtainedCheckSum = 0;

            for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
                auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                if (randoSaveCheck.shuffled && accessLogicFunc.first()) {
                    availableChecks.push_back({ randoCheckId, accessLogicFunc.second });
                    if (randoSaveCheck.obtained) {
                        obtainedCheckSum++;
                    }
                }
            }

            if (availableChecks.size() > 0) {
                std::string regionName = Ship_GetSceneName(randoRegion.sceneId);
                if (randoRegion.name != "") {
                    regionName += " - ";
                    regionName += randoRegion.name;
                }

                regionName += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(availableChecks.size()) + ")";

                ImGui::PushID(regionId);
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
                if (expandedheaderState != expandHeaders) {
                    ImGui::SetNextItemOpen(expandHeaders);
                }
                if (ImGui::CollapsingHeader(regionName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Indent(20.0f);
                    for (auto& [checkId, accessLogicString] : availableChecks) {
                        auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
                        auto& randoSaveCheck = RANDO_SAVE_CHECKS[checkId];
                        ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained ? UIWidgets::Colors::LightGreen : UIWidgets::Colors::White);
                        if (checkTrackerShouldShowRow(randoSaveCheck.obtained)) {
                            ImGui::Text("%s", readableCheckNames[checkId].c_str());
                            if (accessLogicString != "") {
                                UIWidgets::Tooltip(accessLogicString.c_str());
                            }
                            if (randoSaveCheck.obtained) {
                                ImGui::SameLine(0, 50.0f);
                                ImGui::Text("%s", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                            }
                        }
                        ImGui::PopStyleColor();
                    }
                    ImGui::Unindent(20.0f);
                }
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }
        expandedheaderState = expandHeaders;
    } else {
        for (auto& [sceneId, checks] : sceneChecks) {
            if (sceneId == SCENE_MAX) {
                continue;
            }

            ImGui::PushID(sceneId);
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
            uint32_t obtainedCheckSum = getSumOfObtainedChecks(checks);
            std::string headerText = Ship_GetSceneName(sceneId);
            headerText += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(checks.size()) + ")";

            ImGui::PushStyleColor(ImGuiCol_Text, obtainedCheckSum == checks.size() ? 
                UIWidgets::Colors::LightGreen : UIWidgets::Colors::White);
            
            if (expandedheaderState != expandHeaders) {
                ImGui::SetNextItemOpen(expandHeaders);
            }
            if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Indent(20.0f);
                for (auto& randoCheckId : checks) {
                    Rando::StaticData::RandoStaticCheck& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    ImGui::PushStyleColor(ImGuiCol_Text, randoSaveCheck.obtained ? UIWidgets::Colors::LightGreen : UIWidgets::Colors::White);
                    if (checkTrackerShouldShowRow(randoSaveCheck.obtained)) {
                        ImGui::Text("%s", readableCheckNames[randoCheckId].c_str());
                        if (randoSaveCheck.obtained) {
                            ImGui::SameLine(0, 50.0f);
                            ImGui::Text("%s", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                        }
                    }
                    ImGui::PopStyleColor();
                }
                ImGui::Unindent(20.0f);
            }
            ImGui::PopStyleColor(2);
            ImGui::PopID();
        }
        expandedheaderState = expandHeaders;
    }
}

void SettingsWindow::DrawElement() {
    ImGui::SeparatorText("Check Tracker Settings");
    UIWidgets::Checkbox("Show Logic", &showLogic);
    UIWidgets::Checkbox("Hide Collected", &hideCollected);
    if (UIWidgets::Button("Expand/Collapse All")) {
        expandHeaders = !expandHeaders;
    }
}

void Init() {
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        readableCheckNames[randoCheckId] = convertEnumToReadableName(randoStaticCheck.name);
    }
}

void OnFileLoad() {
    if (!IS_RANDO) {
        return;
    }

    initializeSceneChecks();
}

} // namespace CheckTracker

} // namespace Rando
