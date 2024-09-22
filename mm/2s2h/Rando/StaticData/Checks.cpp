#include "StaticData.h"

namespace Rando {

namespace StaticData {

// clang-format off
std::unordered_map<RandoCheck, RandoCheckData> Checks = {
    { RC_UNKNOWN,                                 { RC_UNKNOWN,                                  RCTYPE_UNKNOWN,      SCENE_MAX,          FLAG_NONE,                   0x00,                                                                RI_UNKNOWN } },
    { RC_S_CLOCK_TOWN_POH,                        { RC_S_CLOCK_TOWN_POH,                         RCTYPE_FREESTANDING, SCENE_CLOCKTOWER,   FLAG_CYCL_SCENE_COLLECTIBLE, 0x0A,                                                                RI_PIECE_OF_HEART } },
    { RC_S_CLOCK_TOWN_UPPER_CHEST,                { RC_S_CLOCK_TOWN_UPPER_CHEST,                 RCTYPE_CHEST,        SCENE_CLOCKTOWER,   FLAG_CYCL_SCENE_CHEST,       0x00,                                                                RI_RED_RUPEE } },
    { RC_S_CLOCK_TOWN_CARPENTER_CHEST,            { RC_S_CLOCK_TOWN_CARPENTER_CHEST,             RCTYPE_CHEST,        SCENE_CLOCKTOWER,   FLAG_CYCL_SCENE_CHEST,       0x01,                                                                RI_PURPLE_RUPEE } },
    { RC_N_CLOCK_TOWN_POH,                        { RC_N_CLOCK_TOWN_POH,                         RCTYPE_FREESTANDING, SCENE_BACKTOWN,     FLAG_CYCL_SCENE_COLLECTIBLE, 0x0A,                                                                RI_PIECE_OF_HEART } },
    { RC_N_CLOCK_TOWN_POH,                        { RC_N_CLOCK_TOWN_POH,                         RCTYPE_FREESTANDING, SCENE_BACKTOWN,     FLAG_CYCL_SCENE_COLLECTIBLE, 0x0A,                                                                RI_PIECE_OF_HEART } },
    { RC_E_CLOCK_TOWN_UPPER_CHEST,                { RC_E_CLOCK_TOWN_UPPER_CHEST,                 RCTYPE_CHEST,        SCENE_TOWN,         FLAG_CYCL_SCENE_CHEST,       0x0A,                                                                RI_SILVER_RUPEE } },
    { RC_LAUNDRY_DUDE,                            { RC_LAUNDRY_DUDE,                             RCTYPE_UNKNOWN,      SCENE_ALLEY,        FLAG_WEEK_EVENT_REG,         WEEKEVENTREG_38_40,                                                  RI_MASK_BREMEN } },
    { RC_CLOCK_TOWN_STRAY_FAIRY,                  { RC_CLOCK_TOWN_STRAY_FAIRY,                   RCTYPE_STRAY_FAIRY,  SCENE_ALLEY,        FLAG_RANDO_INF,              RANDO_INF_CLOCK_TOWN_STRAY_FAIRY_COLLECTED,                          RI_CLOCK_TOWN_STRAY_FAIRY } },
 // { RC_CLOCK_TOWN_GREAT_FAIRY,                  { RC_CLOCK_TOWN_GREAT_FAIRY,                   RCTYPE_UNKNOWN,      SCENE_YOUSEI_IZUMI, FLAG_CYCL_SCENE_SWITCH,      0x0A,                                                                RI_PROGRESSIVE_MAGIC } },
    { RC_STOCK_POT_INN_GRANDMA_SHORT_STORY,       { RC_STOCK_POT_INN_GRANDMA_SHORT_STORY,        RCTYPE_UNKNOWN,      SCENE_YADOYA,       FLAG_WEEK_EVENT_REG,         WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP, RI_PIECE_OF_HEART } },
    { RC_STOCK_POT_INN_GRANDMA_LONG_STORY,        { RC_STOCK_POT_INN_GRANDMA_LONG_STORY,         RCTYPE_UNKNOWN,      SCENE_YADOYA,       FLAG_WEEK_EVENT_REG,         WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP,  RI_PIECE_OF_HEART } },
    { RC_STOCK_POT_INN_ROOM_KEY,                  { RC_STOCK_POT_INN_ROOM_KEY,                   RCTYPE_UNKNOWN,      SCENE_YADOYA,       FLAG_WEEK_EVENT_REG,         WEEKEVENTREG_RECEIVED_ROOM_KEY,                                      RI_ROOM_KEY } },
    { RC_WOODFALL_LOBBY_STRAY_FAIRY,              { RC_WOODFALL_LOBBY_STRAY_FAIRY,               RCTYPE_STRAY_FAIRY,  SCENE_MITURIN,      FLAG_CYCL_SCENE_SWITCH,      0x2B,                                                                RI_WOODFALL_STRAY_FAIRY } },
    { RC_WOODFALL_LOBBY_CHEST_STRAY_FAIRY,        { RC_WOODFALL_LOBBY_CHEST_STRAY_FAIRY,         RCTYPE_STRAY_FAIRY,  SCENE_MITURIN,      FLAG_CYCL_SCENE_CHEST,       0x18,                                                                RI_WOODFALL_STRAY_FAIRY } },
    { RC_WOODFALL_SECOND_ROOM_BUBBLE_STRAY_FAIRY, { RC_WOODFALL_SECOND_ROOM_BUBBLE_STRAY_FAIRY,  RCTYPE_STRAY_FAIRY,  SCENE_MITURIN,      FLAG_CYCL_SCENE_SWITCH,      0x30,                                                                RI_WOODFALL_STRAY_FAIRY } },
    { RC_MAX,                                     { RC_UNKNOWN,                                  RCTYPE_UNKNOWN,      SCENE_MAX,          FLAG_NONE,                   0x00,                                                                RI_UNKNOWN } },
};
// clang-format on

RandoCheckData GetCheckFromFlag(FlagType flagType, s32 flag, s16 scene) {
    for (auto& [check, data] : Checks) {
        if (data.flagType == flagType && data.flag == flag && (scene == SCENE_MAX || data.scene == scene)) {
            return data;
        }
    }
    return Checks[RC_UNKNOWN];
}

} // namespace StaticData

} // namespace Rando
