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
#define EXIT(toEntranceId, condition)                         \
    {                                                         \
        toEntranceId, {                                       \
            [] { return condition; }, LogicString(#condition) \
        }                                                     \
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
    { RR_CLOCK_TOWN_SOUTH, { RR_CLOCK_TOWN_SOUTH, "", SCENE_CLOCKTOWER,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
            CHECK(RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) && (CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY)),
        },
        { // Entrances
            ENTRANCE(SOUTH_CLOCK_TOWN, 0), // From clock tower interior
            ENTRANCE(SOUTH_CLOCK_TOWN, 1), // From termina field
            ENTRANCE(SOUTH_CLOCK_TOWN, 2), // From east upper
            ENTRANCE(SOUTH_CLOCK_TOWN, 3), // From west upper
            ENTRANCE(SOUTH_CLOCK_TOWN, 4), // From north
            ENTRANCE(SOUTH_CLOCK_TOWN, 5), // From west lower
            ENTRANCE(SOUTH_CLOCK_TOWN, 6), // From laundry
            ENTRANCE(SOUTH_CLOCK_TOWN, 7), // From east lower
        },
        { // Exits
            EXIT(ENTRANCE(CLOCK_TOWER_INTERIOR, 1), true),
            EXIT(ENTRANCE(TERMINA_FIELD, 6), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 3), true), // To upper
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
            EXIT(ENTRANCE(WEST_CLOCK_TOWN, 1), true), // To lower
            EXIT(ENTRANCE(LAUNDRY_POOL, 0), true),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 1), true), // To lower
        },
        { // Connections
            CONNECTION(RR_CLOCK_TOWN_SOUTH_PLATFORM, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR))),
            CONNECTION(RR_MAX, true), // TODO: We'll remove this eventually
        },
    } },
    { RR_CLOCK_TOWER_INTERIOR, { RR_CLOCK_TOWER_INTERIOR, "", SCENE_INSIDETOWER,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
            CHECK(RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, HAS_ITEM(ITEM_OCARINA_OF_TIME)),
        },
        { // Entrances
            ENTRANCE(CLOCK_TOWER_INTERIOR, 1), // From south clock town
        },
        { // Exits
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 0), true),
        },
        { // Connections
        },
    } },
    { RR_CLOCK_TOWN_SOUTH_PLATFORM, { RR_CLOCK_TOWN_SOUTH_PLATFORM, "Clock Tower Platform", SCENE_CLOCKTOWER,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, true),
        },
        { // Entrances
        },
        { // Exits
            EXIT(ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), true),
        },
        { // Connections
            CONNECTION(RR_CLOCK_TOWN_SOUTH, true),
        },
    } },
    { RR_CLOCK_TOWER_ROOF, { RR_CLOCK_TOWER_ROOF, "", SCENE_OKUJOU,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, HAS_MAGIC && CAN_BE_DEKU),
            CHECK(RC_CLOCK_TOWER_ROOF_OCARINA, HAS_MAGIC && CAN_BE_DEKU),
        },
        { // Entrances
            ENTRANCE(CLOCK_TOWER_ROOFTOP, 0), // From clock tower platform
        },
        { // Exits
        },
        { // Connections
        },
    } },
    { RR_CLOCK_TOWN_LAUNDRY, { RR_CLOCK_TOWN_LAUNDRY, "", SCENE_ALLEY,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, true),
            CHECK(RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        { // Entrances
            ENTRANCE(LAUNDRY_POOL, 0), // From south clock town
        },
        { // Exits
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 6), true),
        },
        { // Connections
        },
    } },
    { RR_CLOCK_TOWN_NORTH, { RR_CLOCK_TOWN_NORTH, "", SCENE_BACKTOWN,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_NORTH_TREE_HP, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA),
            CHECK(RC_CLOCK_TOWN_NORTH_BOMB_LADY, CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
        },
        { // Entrances
            ENTRANCE(NORTH_CLOCK_TOWN, 0), // From termina field
            ENTRANCE(NORTH_CLOCK_TOWN, 1), // From east clock town
            ENTRANCE(NORTH_CLOCK_TOWN, 2), // From south clock town
            ENTRANCE(NORTH_CLOCK_TOWN, 3), // From great fairy
            ENTRANCE(NORTH_CLOCK_TOWN, 4), // From scrub playground
        },
        { // Exits
            EXIT(ENTRANCE(TERMINA_FIELD, 8), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 5), true),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 4), true),
            EXIT(ENTRANCE(FAIRY_FOUNTAIN, 0), true),
            EXIT(ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), CAN_BE_DEKU),
        },
        { // Connections
        },
    } },
    { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, "Clock Town", SCENE_YOUSEI_IZUMI,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)),
            CHECK(RC_CLOCK_TOWN_GREAT_FAIRY_ALT, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN),
        },
        { // Entrances
            ENTRANCE(DEKU_SCRUB_PLAYGROUND, 0), // From north clock town
        },
        { // Exits
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 3), true),
        },
        { // Connections
        },
    } },
    { RR_CLOCK_TOWN_EAST, { RR_CLOCK_TOWN_EAST, "", SCENE_TOWN,
        { // Events
        },
        { // Checks
            CHECK(RC_CLOCK_TOWN_STRAY_FAIRY, CAN_BE_DEKU), // Same check in two places, is this okay?
        },
        { // Entrances
            ENTRANCE(EAST_CLOCK_TOWN, 0), // From termina field
            ENTRANCE(EAST_CLOCK_TOWN, 1), // From lower south clock town
            ENTRANCE(EAST_CLOCK_TOWN, 2), // From astral observatory passage
            ENTRANCE(EAST_CLOCK_TOWN, 3), // From upper south clock town
            ENTRANCE(EAST_CLOCK_TOWN, 4), // From treasure chest shop
            ENTRANCE(EAST_CLOCK_TOWN, 5), // From north clock town
            ENTRANCE(EAST_CLOCK_TOWN, 6), // From honey and darling's shop
            ENTRANCE(EAST_CLOCK_TOWN, 7), // From mayor's residence
            ENTRANCE(EAST_CLOCK_TOWN, 8), // From town shooting gallery
            ENTRANCE(EAST_CLOCK_TOWN, 9), // From stock pot inn lobby
            ENTRANCE(EAST_CLOCK_TOWN, 10), // From stock pot inn upstairs
            ENTRANCE(EAST_CLOCK_TOWN, 11), // From milk bar
        },
        { // Exits
            EXIT(ENTRANCE(TERMINA_FIELD, 7), CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON),
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 7), true), // To lower
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 0), true), // TODO: Bombers Code req
            EXIT(ENTRANCE(SOUTH_CLOCK_TOWN, 2), true), // To upper
            EXIT(ENTRANCE(TREASURE_CHEST_SHOP, 0), true),
            EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 1), true),
            EXIT(ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0), true),
            EXIT(ENTRANCE(MAYORS_RESIDENCE, 0), true),
            EXIT(ENTRANCE(TOWN_SHOOTING_GALLERY, 0), true),
            EXIT(ENTRANCE(STOCK_POT_INN, 0), true), // To lobby
            EXIT(ENTRANCE(STOCK_POT_INN, 1), CAN_BE_DEKU), // To upstairs
            EXIT(ENTRANCE(MILK_BAR, 0), HAS_ITEM(ITEM_MASK_ROMANI)),
        },
        { // Connections
        },
    } },
    { RR_ASTRAL_OBSERVATORY_PASSAGE, { RR_ASTRAL_OBSERVATORY_PASSAGE, "Passage", SCENE_TENMON_DAI,
        { // Events
        },
        { // Checks
            CHECK(RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, (CAN_BE_HUMAN || CAN_BE_DIETY || CAN_BE_ZORA) && CAN_USE_EXPLOSIVE),
        },
        { // Entrances
            ENTRANCE(ASTRAL_OBSERVATORY, 0), // From east clock town
        },
        { // Exits
            EXIT(ENTRANCE(EAST_CLOCK_TOWN, 2), true),
        },
        { // Connections
            CONNECTION(RR_ASTRAL_OBSERVATORY, CAN_BE_DEKU && HAS_MAGIC),
        },
    } },
    { RR_ASTRAL_OBSERVATORY, { RR_ASTRAL_OBSERVATORY, "", SCENE_TENMON_DAI,
        { // Events
        },
        { // Checks
        },
        { // Entrances
            ENTRANCE(ASTRAL_OBSERVATORY, 1), // From outside
        },
        { // Exits
            EXIT(ENTRANCE(TERMINA_FIELD, 9), true),
        },
        { // Connections
            CONNECTION(RR_ASTRAL_OBSERVATORY_PASSAGE, CAN_BE_DEKU && HAS_MAGIC), // TODO: Trick to backflip over balloon
        },
    } },
    { RR_ASTRAL_OBSERVATORY_OUTSIDE, { RR_ASTRAL_OBSERVATORY_OUTSIDE, "Outside Astral Observatory", SCENE_00KEIKOKU,
        { // Events
        },
        { // Checks
            CHECK(RC_ASTRAL_OBSERVATORY_MOON_TEAR, true),
        },
        { // Entrances
            ENTRANCE(TERMINA_FIELD, 9), // From inside
        },
        { // Exits
            EXIT(ENTRANCE(ASTRAL_OBSERVATORY, 1), true),
        },
        { // Connections
        },
    } },
    // TODO: We'll remove this eventually
    { RR_MAX, { RR_MAX, "", SCENE_MAX,
        { // Events
            // EVENT(Flags_SetRandoInf(0), Flags_ClearRandoInf(0), true),
        },
        { // Checks
            // CHECK(RC_CLOCK_TOWN_SCRUB_DEED, Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)),
        },
        { // Entrances
            // { ENTRANCE(SOUTH_CLOCK_TOWN, 0) }, // From clock tower interior
        },
        { // Exits
            // EXIT(ENTRANCE(NORTH_CLOCK_TOWN, 2), true),
        },
        { // Connections
            // CONNECTION(RR_CLOCK_TOWN_SOUTH_PLATFORM, CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR))),
        },
    } },
};
// clang-format on

RandoRegionId GetRegionIdFromEntrance(s32 entrance) {
    static std::unordered_map<s32, RandoRegionId> entranceToRegionId;
    if (entranceToRegionId.empty()) {
        for (auto& [randoRegionId, randoRegion] : Regions) {
            for (auto& entrance : randoRegion.entrances) {
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

    for (auto& [exitId, condition] : randoRegion.exits) {
        RandoRegionId connectedRegionId = GetRegionIdFromEntrance(exitId);
        // Check if the region is accessible and hasn’t been visited yet
        if (reachableRegions.count(connectedRegionId) == 0 && condition.first()) {
            reachableRegions.insert(connectedRegionId);                // Mark region as visited
            FindReachableRegions(connectedRegionId, reachableRegions); // Recursively visit neighbors
        }
    }
}

} // namespace Logic

} // namespace Rando
