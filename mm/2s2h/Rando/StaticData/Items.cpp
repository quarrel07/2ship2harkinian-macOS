#include "StaticData.h"

namespace Rando {

namespace StaticData {

// clang-format off
std::unordered_map<RandoItem, RandoItemData> Items = {
    { RI_UNKNOWN,                { "Unknown",                 ITEM_NONE,          GI_NONE,         GID_NONE } },
    { RI_PIECE_OF_HEART,         { "Piece of Heart",          ITEM_HEART_PIECE,   GI_HEART_PIECE,  GID_HEART_PIECE } },
    { RI_CLOCK_TOWN_STRAY_FAIRY, { "Clock Town Stray Fairy",  ITEM_STRAY_FAIRIES, GI_STRAY_FAIRY,  GID_NONE } },
    { RI_PROGRESSIVE_MAGIC,      { "Progressive Magic Meter", ITEM_NONE,          GI_NONE,         GID_MAGIC_JAR_SMALL } },
    { RI_GREEN_RUPEE,            { "Green Rupee",             ITEM_RUPEE_GREEN,   GI_RUPEE_GREEN,  GID_RUPEE_GREEN } },
    { RI_BLUE_RUPEE,             { "Blue Rupee",              ITEM_RUPEE_BLUE,    GI_RUPEE_BLUE,   GID_RUPEE_BLUE } },
    { RI_RED_RUPEE,              { "Red Rupee",               ITEM_RUPEE_RED,     GI_RUPEE_RED,    GID_RUPEE_RED } },
    { RI_PURPLE_RUPEE,           { "Purple Rupee",            ITEM_RUPEE_PURPLE,  GI_RUPEE_PURPLE, GID_RUPEE_PURPLE } },
    { RI_SILVER_RUPEE,           { "Silver Rupee",            ITEM_RUPEE_SILVER,  GI_RUPEE_SILVER, GID_RUPEE_SILVER } },
    { RI_ROOM_KEY,               { "Room Key",                ITEM_ROOM_KEY,      GI_ROOM_KEY,     GID_ROOM_KEY } },
    { RI_MASK_BREMEN,            { "Bremen Mask",             ITEM_MASK_BREMEN,   GI_MASK_BREMEN,  GID_MASK_BREMEN } },
    { RI_WOODFALL_STRAY_FAIRY,   { "Woodfall Stray Fairy",    ITEM_STRAY_FAIRIES, GI_STRAY_FAIRY,  GID_NONE } },
    { RI_SUNS_SONG,              { "Sun's Song",              ITEM_SONG_SUN,      GI_NONE,         GID_NONE } },
    { RI_MAX,                    { "Unknown",                 ITEM_NONE,          GI_MAX,          GID_MAXIMUM } },
};
// clang-format on

} // namespace StaticData

} // namespace Rando
