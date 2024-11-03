#include "MiscBehavior.h"
#include <libultraship/libultraship.h>
#include <fstream>
#include "BenPort.h"
#include "Rando/Spoiler/Spoiler.h"
#include <boost_custom/container_hash/hash_32.hpp>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

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

struct RandoPoolEntry {
    bool shuffled;
    RandoItemId vanillaItemId;
    RandoItemId placedItemId;
    bool itemPlaced;
    bool checkFilled;
    bool inPool;
};

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void Rando::MiscBehavior::OnFileCreate(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;
        memset(&gSaveContext.save.shipSaveInfo.rando, 0, sizeof(gSaveContext.save.shipSaveInfo.rando));
        memcpy(&gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys,
               &gSaveContext.save.saveInfo.inventory.dungeonKeys,
               sizeof(gSaveContext.save.saveInfo.inventory.dungeonKeys));

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

                for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                    RANDO_SAVE_OPTIONS[randoOptionId] = CVarGetInteger(randoStaticOption.cvar, 0);
                }

                // Begin by placing all vanilla items in their vanilla locations
                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
                    }
                }

                if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_NO_LOGIC) {
                    std::vector<RandoCheckId> checkPool;
                    std::vector<RandoItemId> itemPool;
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_POT &&
                            RANDO_SAVE_OPTIONS[RO_SHUFFLE_POTS] == RO_GENERIC_NO) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                            if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO) {
                                continue;
                            } else {
                                RANDO_SAVE_CHECKS[randoCheckId].price = Ship_Random(0, 200);
                            }
                        }

                        checkPool.push_back(randoCheckId);
                        itemPool.push_back(randoStaticCheck.randoItemId);
                    }

                    for (size_t i = 0; i < itemPool.size(); i++) {
                        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
                    }

                    for (size_t i = 0; i < checkPool.size(); i++) {
                        RANDO_SAVE_CHECKS[checkPool[i]].shuffled = true;
                        RANDO_SAVE_CHECKS[checkPool[i]].randoItemId = itemPool[i];
                    }
                } else if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_GLITCHLESS) {
                    std::unordered_map<RandoCheckId, RandoPoolEntry> randoCheckPool;
                    SaveContext copiedSaveContext;
                    memcpy(&copiedSaveContext, &gSaveContext, sizeof(SaveContext));

                    // As far as logic is concerned, we start in Deku form
                    gSaveContext.save.playerForm = PLAYER_FORM_DEKU;

                    // First loop through all regions and add checks/items to the pool
                    for (auto& [randoRegionId, randoStaticRegion] : Rando::StaticData::Regions) {
                        for (auto& [randoCheckId, _] : randoStaticRegion.checks) {
                            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

                            if (randoStaticCheck.randoCheckType == RCTYPE_SKULL_TOKEN &&
                                RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS] == RO_GENERIC_NO) {
                                continue;
                            }

                            if (randoStaticCheck.randoCheckType == RCTYPE_POT &&
                                RANDO_SAVE_OPTIONS[RO_SHUFFLE_POTS] == RO_GENERIC_NO) {
                                continue;
                            }

                            if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                                if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS] == RO_GENERIC_NO) {
                                    continue;
                                } else {
                                    RANDO_SAVE_CHECKS[randoCheckId].price = Ship_Random(0, 200);
                                }
                            }

                            randoCheckPool[randoCheckId] = { true,       randoStaticCheck.randoItemId,
                                                             RI_UNKNOWN, false,
                                                             false,      false };
                        }
                    }

                    // Recursive lambda for backtracking placement
                    std::function<bool(std::set<RandoRegionId>)> PlaceItems =
                        [&](std::set<RandoRegionId> reachableRegions) -> bool {
                        // Crawl through all reachable regions and add any new reachable regions
                        std::set<RandoRegionId> currentReachableRegions = reachableRegions;
                        for (RandoRegionId regionId : reachableRegions) {
                            FindReachableRegions(regionId, currentReachableRegions);
                        }

                        // Mark newly accessible checks
                        std::vector<RandoCheckId> newChecksInPool;
                        for (RandoRegionId regionId : currentReachableRegions) {
                            auto& randoStaticRegion = Rando::StaticData::Regions[regionId];
                            for (auto& [randoCheckId, accessLogicFunc] : randoStaticRegion.checks) {
                                if (
                                    // Check is shuffled
                                    randoCheckPool[randoCheckId].shuffled &&
                                    // Check is not already in the pool
                                    randoCheckPool[randoCheckId].inPool == false) {
                                    // Check is accessible
                                    if (accessLogicFunc()) {
                                        randoCheckPool[randoCheckId].inPool = true;
                                        newChecksInPool.push_back(randoCheckId);
                                    } else {
                                        SPDLOG_INFO("Check {} is not accessible",
                                                    Rando::StaticData::Checks[randoCheckId].name);
                                    }
                                }
                            }
                        }

                        int checksLeft = 0;
                        std::vector<RandoCheckId> currentCheckPool;
                        std::vector<std::pair<RandoItemId, RandoCheckId>> currentItemPool;
                        for (auto& [randoCheckId, randoPoolEntry] : randoCheckPool) {
                            if (randoPoolEntry.inPool) {
                                if (!randoPoolEntry.itemPlaced) {
                                    currentItemPool.push_back({ randoPoolEntry.vanillaItemId, randoCheckId });
                                }
                                if (!randoPoolEntry.checkFilled) {
                                    currentCheckPool.push_back(randoCheckId);
                                }
                            }
                            if (randoPoolEntry.shuffled && !randoPoolEntry.checkFilled) {
                                checksLeft++;
                            }
                        }
                        if (checksLeft == 0) {
                            return true; // All items placed
                        }

                        if (currentItemPool.size() == 0) {
                            for (RandoCheckId randoCheckId : newChecksInPool) {
                                randoCheckPool[randoCheckId].inPool = false;
                            }
                            return false; // No more items to place
                        }

                        // Shuffle the pool
                        if (currentItemPool.size() > 1) {
                            for (size_t i = 0; i < currentItemPool.size(); i++) {
                                std::swap(currentItemPool[i],
                                          currentItemPool[Ship_Random(0, currentItemPool.size() - 1)]);
                            }
                        }

                        for (size_t i = 0; i < currentItemPool.size(); i++) {
                            // Place the item in the check
                            RandoCheckId randoCheckId = currentCheckPool[i];
                            auto [randoItemId, randoCheckIdFromItem] = currentItemPool[i];
                            SPDLOG_INFO("Placing item {} in check {}",
                                        Rando::StaticData::Items[randoItemId].spoilerName,
                                        Rando::StaticData::Checks[randoCheckId].name);
                            randoCheckPool[randoCheckId].checkFilled = true;
                            randoCheckPool[randoCheckId].placedItemId = randoItemId;
                            randoCheckPool[randoCheckIdFromItem].itemPlaced = true;
                            RandoItemId convertedItemId = ConvertItem(randoItemId);
                            GiveItem(convertedItemId);

                            // Recurse to place the next item
                            if (PlaceItems(currentReachableRegions)) {
                                return true; // Found a solution
                            }

                            SPDLOG_INFO("Failed to place item {} in check {}",
                                        Rando::StaticData::Items[randoItemId].spoilerName,
                                        Rando::StaticData::Checks[randoCheckId].name);
                            // Backtrack: remove the item and try another check
                            randoCheckPool[randoCheckId].checkFilled = false;
                            randoCheckPool[randoCheckId].placedItemId = RI_UNKNOWN;
                            randoCheckPool[randoCheckIdFromItem].itemPlaced = false;
                            RemoveItem(convertedItemId);
                        }

                        for (RandoCheckId randoCheckId : newChecksInPool) {
                            randoCheckPool[randoCheckId].inPool = false;
                        }

                        return false; // No valid placements for this item
                    };

                    if (!PlaceItems({ RR_CLOCK_TOWN_SOUTH })) {
                        memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));
                        throw std::runtime_error("Failed to place all items in glitchless logic, input seed: " +
                                                 inputSeed);
                    } else {
                        memcpy(&gSaveContext, &copiedSaveContext, sizeof(SaveContext));

                        for (auto& [randoCheckId, randoPoolEntry] : randoCheckPool) {
                            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoPoolEntry.placedItemId;
                            RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
                        }

                        SPDLOG_INFO("Successfully placed all items in glitchless logic, input seed: {}", inputSeed);
                    }
                }

                if (CVarGetInteger("gRando.GenerateSpoiler", 0)) {
                    nlohmann::json j;
                    j["type"] = "2S2H_RANDO_SPOILER";
                    j["inputSeed"] = inputSeed;
                    j["finalSeed"] = finalSeed;

                    j["options"] = nlohmann::json::object();
                    for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                        j["options"][randoStaticOption.name] = RANDO_SAVE_OPTIONS[randoOptionId];
                    }

                    j["checks"] = nlohmann::json::object();
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                            continue;
                        }

                        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
                            continue;
                        }

                        if (randoStaticCheck.randoCheckType == RCTYPE_SHOP) {
                            j["checks"][randoStaticCheck.name] = nlohmann::json::object();
                            j["checks"][randoStaticCheck.name]["randoItemId"] =
                                Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId].spoilerName;
                            j["checks"][randoStaticCheck.name]["price"] = RANDO_SAVE_CHECKS[randoCheckId].price;
                        } else {
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
                    Rando::Spoiler::RefreshOptions();
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

                for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                    RANDO_SAVE_OPTIONS[randoOptionId] = j["options"][randoStaticOption.name].get<int>();
                }

                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
                        continue;
                    }

                    if (!j["checks"].contains(randoStaticCheck.name)) {
                        continue;
                    }

                    // Check if it's an object or a string
                    if (j["checks"][randoStaticCheck.name].is_object()) {
                        std::string itemName = j["checks"][randoStaticCheck.name]["randoItemId"].get<std::string>();
                        RandoItemId randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());

                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;
                        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;

                        // If it has a price, set it
                        if (j["checks"][randoStaticCheck.name].contains("price")) {
                            RANDO_SAVE_CHECKS[randoCheckId].price =
                                j["checks"][randoStaticCheck.name]["price"].get<uint16_t>();
                        }
                    } else {
                        std::string itemName = j["checks"][randoStaticCheck.name].get<std::string>();
                        RandoItemId randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());

                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;
                        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
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
