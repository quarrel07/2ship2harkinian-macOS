#include "Logic.h"

extern "C" {
s32 Flags_GetRandoInf(s32 flag);
}

namespace Rando {

namespace Logic {

#define IS_DEKU (GET_PLAYER_FORM == PLAYER_FORM_DEKU)
#define IS_ZORA (GET_PLAYER_FORM == PLAYER_FORM_ZORA)
#define IS_DEITY (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY)
#define IS_GORON (GET_PLAYER_FORM == PLAYER_FORM_GORON)
#define IS_HUMAN (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)
#define HAS_ITEM(item) (INV_CONTENT(item) == item)
#define CAN_BE_DEKU (IS_DEKU || HAS_ITEM(ITEM_MASK_DEKU))
#define CAN_BE_ZORA (IS_ZORA || HAS_ITEM(ITEM_MASK_ZORA))
#define CAN_BE_DIETY (IS_DEITY || HAS_ITEM(ITEM_MASK_FIERCE_DEITY))
#define CAN_BE_GORON (IS_GORON || HAS_ITEM(ITEM_MASK_GORON))
#define CAN_BE_HUMAN                                                                                        \
    (IS_HUMAN || (IS_DEITY && HAS_ITEM(ITEM_MASK_FIERCE_DEITY)) || (IS_ZORA && HAS_ITEM(ITEM_MASK_ZORA)) || \
     (IS_DEKU && HAS_ITEM(ITEM_MASK_DEKU)) || (IS_GORON && HAS_ITEM(ITEM_MASK_GORON)))
#define HAS_MAGIC (gSaveContext.save.saveInfo.playerData.isMagicAcquired)
#define CAN_USE_EXPLOSIVE (HAS_ITEM(ITEM_BOMB) || HAS_ITEM(ITEM_BOMBCHU) || (HAS_ITEM(ITEM_MASK_BLAST) && CAN_BE_HUMAN))
#define ONE_WAY_EXIT 0

std::string LogicString(std::string condition) {
    if (condition == "true")
        return "";

    return condition;
}

#define EVENT(onApply, onRemove, condition)                                                                    \
    {                                                                                                          \
        {                                                                                                      \
            [] { return onApply; }, [] { return onRemove; }, [] { return condition; }, LogicString(#condition) \
        }                                                                                                      \
    }
#define EXIT(toEntrance, fromEntrance, condition)                           \
    {                                                                       \
        toEntrance, {                                                       \
            fromEntrance, [] { return condition; }, LogicString(#condition) \
        }                                                                   \
    }
#define CONNECTION(region, condition)                         \
    {                                                         \
        region, {                                             \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }
#define CHECK(check, condition)                               \
    {                                                         \
        check, {                                              \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
    }

// clang-format off
std::map<RandoRegionId, RandoRegion> Regions = {
    { RR_CLOCK_TOWN_SOUTH, RandoRegion{ .name = "", .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) && (CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY)),
        },
        .exits = {
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1), ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6),        ENTRANCE(SOUTH_CLOCK_TOWN, 1), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3),      ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2),      ENTRANCE(SOUTH_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2),     ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1),      ENTRANCE(SOUTH_CLOCK_TOWN, 5), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0),         ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1),      ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
        },
        .connections = {
            CONNECTION(RR_CLOCK_TOWN_SOUTH_PLATFORM, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR))),
            CONNECTION(RR_MAX, true), // TODO: We'll remove this eventually
        },
    } },
    { RR_CLOCK_TOWER_INTERIOR, RandoRegion{ .name = "", .sceneId = SCENE_INSIDETOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        .exits = {
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0), ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
        },
    } },
    { RR_CLOCK_TOWN_SOUTH_PLATFORM, RandoRegion{ .name = "Clock Tower Platform", .sceneId = SCENE_CLOCKTOWER,
        .checks = {
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, true),
        },
        .exits = {
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), ONE_WAY_EXIT, true),
        },
        .connections = {
            CONNECTION(RR_CLOCK_TOWN_SOUTH, true),
        },
    } },
    { RR_CLOCK_TOWER_ROOF, RandoRegion{ .name = "", .sceneId = SCENE_OKUJOU,
        .checks = {
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, HAS_MAGIC && CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, HAS_MAGIC && CAN_BE_DEKU),
        },
        .oneWayEntrances = {
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
    } },
    { RR_CLOCK_TOWN_LAUNDRY, RandoRegion{ .name = "", .sceneId = SCENE_ALLEY,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = {
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6), ENTRANCE(LAUNDRY_POOL, 0), true),
        },
    } },
    { RR_CLOCK_TOWN_NORTH, RandoRegion{ .name = "", .sceneId = SCENE_BACKTOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_HP, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        .exits = {
            EXIT(ENTRANCE(TERMINA_FIELD, 8),         ENTRANCE(NORTH_CLOCK_TOWN, 0), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5),       ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4),      ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0),        ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), ENTRANCE(NORTH_CLOCK_TOWN, 4), CAN_BE_DEKU),
        },
    } },
    { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, RandoRegion{ .name = "Clock Town", .sceneId = SCENE_YOUSEI_IZUMI,
        .checks = {
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN),
        },
        .exits = {
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3), ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), true),
        },
    } },
    { RR_CLOCK_TOWN_EAST, RandoRegion{ .name = "", .sceneId = SCENE_TOWN,
        .checks = {
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, CAN_BE_DEKU), // Same check in two places, is this okay?
        },
        .exits = {
            EXIT(ENTRANCE(TERMINA_FIELD, 7),           ENTRANCE(EAST_CLOCK_TOWN, 0), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7),        ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0),      ENTRANCE(EAST_CLOCK_TOWN, 2), true), // TODO: Bombers Code req
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2),        ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0),     ENTRANCE(EAST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1),        ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), ENTRANCE(EAST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0),        ENTRANCE(EAST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0),   ENTRANCE(EAST_CLOCK_TOWN, 8), true),
            EXIT(ENTRANCE(STOCK_POT_INN, 0),           ENTRANCE(EAST_CLOCK_TOWN, 9), true), // To lobby
            EXIT(ENTRANCE(STOCK_POT_INN, 1),           ENTRANCE(EAST_CLOCK_TOWN, 10), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0),                ENTRANCE(EAST_CLOCK_TOWN, 11), HAS_ITEM(ITEM_MASK_ROMANI)),
        },
    } },
    { RR_CLOCK_TOWN_WEST, RandoRegion{ .name = "", .sceneId = SCENE_ICHIBA,
        .exits = {
            EXIT(ENTRANCE(TERMINA_FIELD, 0),    ENTRANCE(WEST_CLOCK_TOWN, 0), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 5), ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 3), ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(SWORDMANS_SCHOOL, 0), ENTRANCE(WEST_CLOCK_TOWN, 3), true),
            EXIT(ENTRANCE(CURIOSITY_SHOP, 0),   ENTRANCE(WEST_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(TRADING_POST, 0),     ENTRANCE(WEST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(BOMB_SHOP, 0),        ENTRANCE(WEST_CLOCK_TOWN, 6), true),
            EXIT(ENTRANCE(POST_OFFICE, 0),      ENTRANCE(WEST_CLOCK_TOWN, 7), true),
            EXIT(ENTRANCE(LOTTERY_SHOP, 0),     ENTRANCE(WEST_CLOCK_TOWN, 8), true),
        },
    } },
    { RR_ASTRAL_OBSERVATORY_PASSAGE, RandoRegion{ .name = "Passage", .sceneId = SCENE_TENMON_DAI,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, (CAN_BE_HUMAN || CAN_BE_DIETY || CAN_BE_ZORA) && CAN_USE_EXPLOSIVE),
        },
        .exits = {
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2), ENTRANCE(ASTRAL_OBSERVATORY, 0), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY, CAN_BE_DEKU && HAS_MAGIC),
        },
    } },
    { RR_ASTRAL_OBSERVATORY, RandoRegion{ .name = "", .sceneId = SCENE_TENMON_DAI,
        .exits = {
            EXIT(ENTRANCE(TERMINA_FIELD, 9), ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        .connections = {
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, CAN_BE_DEKU && HAS_MAGIC), // TODO: Trick to backflip over balloon
        },
    } },
    { RR_ASTRAL_OBSERVATORY_OUTSIDE, RandoRegion{ .name = "Outside Astral Observatory", .sceneId = SCENE_00KEIKOKU,
        .checks = {
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        .exits = {
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1), ENTRANCE(TERMINA_FIELD, 9), true),
        },
    } },
    { RR_TERMINA_FIELD, RandoRegion{ .name = "", .sceneId = SCENE_00KEIKOKU,
        .exits = {
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 0),          ENTRANCE(TERMINA_FIELD, 0), true),
            EXIT(ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 0),   ENTRANCE(TERMINA_FIELD, 1), true),
            EXIT(ENTRANCE(GREAT_BAY_COAST, 0),          ENTRANCE(TERMINA_FIELD, 2), true),
            EXIT(ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 0), ENTRANCE(TERMINA_FIELD, 3), true),
            EXIT(ENTRANCE(ROAD_TO_IKANA, 0),            ENTRANCE(TERMINA_FIELD, 4), true),
            EXIT(ENTRANCE(MILK_ROAD, 0),                ENTRANCE(TERMINA_FIELD, 5), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 1),         ENTRANCE(TERMINA_FIELD, 6), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 0),          ENTRANCE(TERMINA_FIELD, 7), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 0),         ENTRANCE(TERMINA_FIELD, 8), true),
        },
    } },
    // TODO: We'll remove this eventually
    { RR_MAX, RandoRegion{ .name = "", .sceneId = SCENE_MAX,
        .checks = {
            // CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
        },
        .exits = {
            // EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
        },
        .connections = {
            // CONNECTION(RR_CLOCK_TOWN_SOUTH_PLATFORM, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR))),
        },
        .events = {
            // EVENT(Flags_SetRandoInf(0), Flags_ClearRandoInf(0), true),
        },
        .oneWayEntrances = {
            // { ENTRANCE(SOUTH_CLOCK_TOWN, 0) }, // From clock tower interior
        },
    } },
};
// clang-format on

RandoRegionId GetRegionIdFromEntrance(s32 entrance) {
    static std::unordered_map<s32, RandoRegionId> entranceToRegionId;
    if (entranceToRegionId.empty()) {
        for (auto& [randoRegionId, randoRegion] : Regions) {
            for (auto& [_, regionExit] : randoRegion.exits) {
                if (regionExit.returnEntrance == ONE_WAY_EXIT) {
                    continue;
                }
                entranceToRegionId[regionExit.returnEntrance] = randoRegionId;
            }
            for (auto& entrance : randoRegion.oneWayEntrances) {
                entranceToRegionId[entrance] = randoRegionId;
            }
        }
    }

    if (entranceToRegionId.contains(entrance)) {
        return entranceToRegionId[entrance];
    }

    return RR_MAX;
}

void FindReachableRegions(RandoRegionId currentRegion, std::set<RandoRegionId>& reachableRegions) {
    auto& randoRegion = Rando::Logic::Regions[currentRegion];

    for (auto& [connectedRegionId, condition] : randoRegion.connections) {
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && condition.first()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }

    for (auto& [exitId, regionExit] : randoRegion.exits) {
        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && regionExit.condition()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }
}

} // namespace Logic

} // namespace Rando
