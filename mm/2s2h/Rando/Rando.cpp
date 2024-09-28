#include "Rando.h"
#include "Enhancements/GameInteractor/GameInteractor.h"
#include <libultraship/libultraship.h>
#include <random>
#include <fstream>
#include <filesystem>
#include "Rando/ActorBehavior/ActorBehavior.h"
#include <boost_custom/container_hash/hash_32.hpp>
#include "BenPort.h"

extern "C" {
#include "ShipUtils.h"
#include "variables.h"
#include "functions.h"
}

static bool queued = false;
void RandomizerQueueChecks(Actor* actor) {
    if (queued) {
        return;
    }

    for (auto& randoSaveCheck : RANDO_SAVE_CHECKS) {
        if (randoSaveCheck.eligible && !randoSaveCheck.obtained) {
            queued = true;

            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = !CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0),
                .getItemText = Rando::StaticData::Items[randoItemId].name,
                .drawItem = [randoItemId]() { Rando::DrawItem(randoItemId); },
                .giveItem =
                    [&randoSaveCheck, randoItemId]() {
                        Rando::GiveItem(randoItemId);
                        randoSaveCheck.obtained = true;
                        queued = false;
                    } });
            return;
        }
    }
}

void RandomizerOnFlagSetHandler(FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    randoSaveCheck.eligible = true;
}

void RandomizerOnSceneFlagSetHandler(s16 sceneId, FlagType flagType, u32 flag) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(flagType, flag, sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
    randoSaveCheck.eligible = true;
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case GI_VB_GIVE_ITEM_FROM_GURUGURU:
        case GI_VB_GIVE_ITEM_FROM_SCRIPT:
        case GI_VB_GIVE_ITEM_FROM_SWORDSMAN:
            *should = false;
            break;
    }
}

std::vector<std::string> Rando::spoilerFileOptions;
const std::filesystem::path randomizerFolderPath(Ship::Context::GetPathRelativeToAppDirectory("randomizer",
                                                                                              appShortName));

void refreshSpoilerFileOptions() {
    Rando::spoilerFileOptions.clear();

    Rando::spoilerFileOptions.push_back("Generate New Seed");
    s32 spoilerFileIndex = -1;

    // ensure the randomizer folder exists
    if (!std::filesystem::exists(randomizerFolderPath)) {
        std::filesystem::create_directory(randomizerFolderPath);
    }
    for (const auto& entry : std::filesystem::directory_iterator(randomizerFolderPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            Rando::spoilerFileOptions.push_back(fileName);

            if (fileName == CVarGetString("gRando.SpoilerFile", "")) {
                spoilerFileIndex = Rando::spoilerFileOptions.size() - 1;
            }
        }
    }

    if (spoilerFileIndex == -1) {
        CVarSetInteger("gRando.SpoilerFileIndex", 0);
        CVarSetString("gRando.SpoilerFile", "");
    } else {
        CVarSetInteger("gRando.SpoilerFileIndex", spoilerFileIndex);
    }
}

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void OnSaveInitHandler(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;
        memset(&gSaveContext.save.shipSaveInfo.rando, 0, sizeof(gSaveContext.save.shipSaveInfo.rando));

        try {
            if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
                bool hadInputSeed = true;
                std::string inputSeed = CVarGetString("gRando.InputSeed", "");
                if (inputSeed.empty()) {
                    inputSeed = std::to_string(Ship_Random(0, 1000000));
                    hadInputSeed = false;
                }

                uint32_t finalSeed = boost::hash_32<std::string>{}(inputSeed);
                Ship_Random_Seed(finalSeed);

                std::vector<RandoItemId> itemPool;
                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                        itemPool.push_back(randoStaticCheck.randoItemId);
                    }
                }

                for (size_t i = 0; i < itemPool.size(); i++) {
                    std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
                }

                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
                        itemPool.pop_back();
                    }
                }

                if (CVarGetInteger("gRando.GenerateSpoiler", 0)) {
                    nlohmann::json j;
                    j["type"] = "2S2H_RANDO_SPOILER";
                    j["inputSeed"] = inputSeed;
                    j["finalSeed"] = finalSeed;
                    j["checks"] = nlohmann::json::object();
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            j["checks"][randoStaticCheck.name] =
                                Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId].spoilerName;
                        }
                    }

                    std::string spoilerFile = inputSeed + ".json";
                    std::string spoilerFilePath =
                        Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
                    std::ofstream fileStream(spoilerFilePath);
                    if (!fileStream.is_open()) {
                        throw std::runtime_error("Failed to open spoiler file");
                    }

                    fileStream << j.dump(4);

                    if (hadInputSeed) {
                        CVarSetString("gRando.SpoilerFile", spoilerFile.c_str());
                    }
                    refreshSpoilerFileOptions();
                }

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            } else {
                std::string spoilerFile = CVarGetString("gRando.SpoilerFile", "");
                if (spoilerFile.empty()) {
                    throw std::runtime_error("Attempted to load spoiler file with no file selected");
                }

                std::string spoilerFilePath =
                    Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
                std::ifstream fileStream(spoilerFilePath);
                if (!fileStream.is_open()) {
                    throw std::runtime_error("Failed to open spoiler file");
                }

                nlohmann::json j;
                try {
                    fileStream >> j;
                } catch (nlohmann::json::exception& e) {
                    throw std::runtime_error("Failed to parse spoiler file: " + std::string(e.what()));
                }

                if (!j.contains("type") || j["type"] != "2S2H_RANDO_SPOILER") {
                    throw std::runtime_error("Spoiler file is not a valid spoiler file");
                }

                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = Rando::StaticData::GetItemIdFromName(
                            j["checks"][randoStaticCheck.name].get<std::string>().c_str());
                    }
                }

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error with randomizer save creation: {}", e.what());
            Audio_PlaySfx(NA_SE_SY_QUIZ_INCORRECT);
            gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_VANILLA;
            char invalidName[8] = { 18, 23, 31, 10, 21, 18, 13, 62 };
            memcpy(gSaveContext.save.saveInfo.playerData.playerName, invalidName, sizeof(invalidName));
            gSaveContext.save.saveInfo.playerData.newf[0] = '\0';
        }
    }
}

bool Rando::HandleFileDropped(std::string filePath) {
    try {
        std::ifstream fileStream(filePath);

        if (!fileStream.is_open()) {
            return false;
        }

        // Check if first byte is "{"
        if (fileStream.peek() != '{') {
            return false;
        }

        nlohmann::json j;
        try {
            fileStream >> j;
        } catch (nlohmann::json::exception& e) { return false; }

        if (!j.contains("type") || j["type"] != "2S2H_RANDO_SPOILER") {
            return false;
        }

        // Save the spoiler file to the randomizer folder
        std::string spoilerFile = std::filesystem::path(filePath).filename().string();
        std::string spoilerFilePath =
            Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
        std::filesystem::copy_file(filePath, spoilerFilePath, std::filesystem::copy_options::overwrite_existing);

        // Set the spoiler file to the new file
        CVarSetString("gRando.SpoilerFile", spoilerFile.c_str());
        // Update the spoiler file options
        refreshSpoilerFileOptions();

        Audio_PlaySfx(NA_SE_SY_QUIZ_CORRECT);
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Spoiler file loaded");
        return true;
    } catch (std::exception& e) {
        SPDLOG_ERROR("Failed to load file: {}", e.what());
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Failed to load file");
        return false;
    } catch (...) {
        SPDLOG_ERROR("Failed to load file");
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Failed to load file");
        return false;
    }
}

// When a save is loaded, we want to unregister all hooks and re-register them if it's a rando save
void OnSaveLoadHandler(s16 fileNum) {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;
    static uint32_t onPlayerUpdateHook = 0;

    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::ShouldVanillaBehavior>(onVanillaBehaviorHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onPlayerUpdateHook);

    onFlagSetHook = 0;
    onSceneFlagSetHook = 0;
    onVanillaBehaviorHook = 0;
    onPlayerUpdateHook = 0;

    Rando::InitActorBehavior(gSaveContext.save.shipSaveInfo.saveType == SAVETYPE_RANDO);

    if (gSaveContext.save.shipSaveInfo.saveType != SAVETYPE_RANDO) {
        return;
    }

    onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
    onSceneFlagSetHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
    onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldVanillaBehavior>(
        RandomizerOnVanillaBehaviorHandler);
    onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
        ACTOR_PLAYER, RandomizerQueueChecks);
}

// Entry point for the rando module
void Rando::Init() {
    refreshSpoilerFileOptions();
    queued = false;
    GameInteractor::Instance->currentEvent = GIEventNone{};
    GameInteractor::Instance->events.clear();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(OnSaveInitHandler);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(OnSaveLoadHandler);
}
