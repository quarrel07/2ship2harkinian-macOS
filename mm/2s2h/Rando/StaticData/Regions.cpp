#include "StaticData.h"

extern "C" {
s32 Flags_GetRandoInf(s32 flag);
}

namespace Rando {

namespace StaticData {

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

// clang-format off
std::map<RandoRegionId, RandoStaticRegion> Regions = {
    { RR_CLOCK_TOWN_SOUTH, { RR_CLOCK_TOWN_SOUTH, "", SCENE_CLOCKTOWER,
        { // Checks
            { RC_CLOCK_TOWN_SCRUB_DEED, []() { return Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR); } },
            { RC_CLOCK_TOWN_SOUTH_CHEST_UPPER, []() { return CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR); } },
            { RC_CLOCK_TOWN_SOUTH_CHEST_LOWER, []() { return CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) && (CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY); } },
        },
        { // Regions
            { RR_CLOCK_TOWN_SOUTH_PLATFORM, []() { return CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_DIETY || (CAN_BE_DEKU && Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)); } },
            { RR_CLOCK_TOWN_LAUNDRY, []() { return true; } },
            { RR_CLOCK_TOWN_NORTH, []() { return true; } },
            { RR_CLOCK_TOWN_EAST, []() { return true; } },
            { RR_CLOCK_TOWER_INTERIOR, []() { return true; } },
            { RR_CATCH_ALL, []() { return true; } }, // TODO: We'll remove this eventually
        },
    } },
    { RR_CLOCK_TOWER_INTERIOR, { RR_CLOCK_TOWER_INTERIOR, "", SCENE_INSIDETOWER,
        { // Checks
            { RC_CLOCK_TOWER_INTERIOR_SONG_OF_HEALING, []() { return HAS_ITEM(ITEM_OCARINA_OF_TIME); } },
            { RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, []() { return HAS_ITEM(ITEM_OCARINA_OF_TIME); } },
        },
        { // Regions
            { RR_CLOCK_TOWN_SOUTH, []() { return true; } },
        },
    } },
    { RR_CLOCK_TOWN_SOUTH_PLATFORM, { RR_CLOCK_TOWN_SOUTH_PLATFORM, "Clock Tower Platform", SCENE_CLOCKTOWER,
        { // Checks
            { RC_CLOCK_TOWN_SOUTH_PLATFORM_HP, []() { return true; } },
        },
        { // Regions
            { RR_CLOCK_TOWN_SOUTH, []() { return true; } },
            { RR_CLOCK_TOWER_ROOF, []() { return true; } },
        },
    } },
    { RR_CLOCK_TOWER_ROOF, { RR_CLOCK_TOWER_ROOF, "", SCENE_OKUJOU,
        { // Checks
            { RC_CLOCK_TOWER_ROOF_SONG_OF_TIME, []() { return HAS_MAGIC && CAN_BE_DEKU; } },
            { RC_CLOCK_TOWER_ROOF_OCARINA, []() { return HAS_MAGIC && CAN_BE_DEKU; } },
        },
        { // Regions
            // TODO: Moon?
        },
    } },
    { RR_CLOCK_TOWN_LAUNDRY, { RR_CLOCK_TOWN_LAUNDRY, "", SCENE_ALLEY,
        { // Checks
            { RC_CLOCK_TOWN_STRAY_FAIRY, []() { return true; } },
            { RC_CLOCK_TOWN_LAUNDRY_GURU_GURU, []() { return CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON; } },
        },
        { // Regions
            { RR_CLOCK_TOWN_SOUTH, []() { return true; } },
        },
    } },
    { RR_CLOCK_TOWN_NORTH, { RR_CLOCK_TOWN_NORTH, "", SCENE_BACKTOWN,
        { // Checks
            { RC_CLOCK_TOWN_NORTH_TREE_HP, []() { return CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA; } },
            { RC_CLOCK_TOWN_NORTH_BOMB_LADY, []() { return CAN_BE_DIETY || CAN_BE_HUMAN || CAN_BE_ZORA || CAN_BE_GORON; } },
        },
        { // Regions
            { RR_CLOCK_TOWN_SOUTH, []() { return true; } },
            { RR_CLOCK_TOWN_EAST, []() { return true; } },
            { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, []() { return true; } },
        },
    } },
    { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, { RR_CLOCK_TOWN_GREAT_FAIRY_FOUNTAIN, "Clock Town", SCENE_YOUSEI_IZUMI,
        { // Checks
            { RC_CLOCK_TOWN_GREAT_FAIRY, []() { return CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80); } },
            { RC_CLOCK_TOWN_GREAT_FAIRY_ALT, []() { return CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80) && CAN_BE_HUMAN; } },
        },
        { // Regions
            { RR_CLOCK_TOWN_NORTH, []() { return true; } },
        },
    } },
    { RR_CLOCK_TOWN_EAST, { RR_CLOCK_TOWN_EAST, "", SCENE_TOWN,
        { // Checks
            { RC_CLOCK_TOWN_STRAY_FAIRY, []() { return CAN_BE_DEKU; } }, // Same check in two places, is this okay?
        },
        { // Regions
            { RR_CLOCK_TOWN_NORTH, []() { return true; } },
            { RR_ASTRAL_OBSERVATORY_PASSAGE, []() { return true; } }, // TODO: Bombers Code req
        },
    } },
    { RR_ASTRAL_OBSERVATORY_PASSAGE, { RR_ASTRAL_OBSERVATORY_PASSAGE, "Passage", SCENE_TENMON_DAI,
        { // Checks
            { RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST, []() { return (CAN_BE_HUMAN || CAN_BE_DIETY || CAN_BE_ZORA) && CAN_USE_EXPLOSIVE; } },
        },
        { // Regions
            { RR_CLOCK_TOWN_EAST, []() { return true; } },
            { RR_ASTRAL_OBSERVATORY, []() { return CAN_BE_DEKU && HAS_MAGIC; } },
        },
    } },
    { RR_ASTRAL_OBSERVATORY, { RR_ASTRAL_OBSERVATORY, "", SCENE_TENMON_DAI,
        { // Checks
        },
        { // Regions
            { RR_ASTRAL_OBSERVATORY_PASSAGE, []() { return CAN_BE_DEKU && HAS_MAGIC; } }, // TODO: Trick to backflip over balloon
            { RR_ASTRAL_OBSERVATORY_OUTSIDE, []() { return true; } },
        },
    } },
    { RR_ASTRAL_OBSERVATORY_OUTSIDE, { RR_ASTRAL_OBSERVATORY_OUTSIDE, "Outside Astral Observatory", SCENE_00KEIKOKU,
        { // Checks
            { RC_ASTRAL_OBSERVATORY_MOON_TEAR, []() { return true; } },
        },
        { // Regions
            { RR_ASTRAL_OBSERVATORY, []() { return true; } },
        },
    } },
    // TODO: We'll remove this eventually
    { RR_CATCH_ALL, { RR_CATCH_ALL, "", SCENE_MAX,
        { // Checks
            // { RC_CLOCK_TOWER_INTERIOR_DEKU_MASK, []() { return true; } },
        },
        { // Regions
        },
    } },
};
// clang-format on

} // namespace StaticData

} // namespace Rando
