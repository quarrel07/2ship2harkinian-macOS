#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

// Copied from z_player.c, we could instead move this to a header file, idk
typedef struct GetItemEntry {
    /* 0x0 */ u8 itemId;
    /* 0x1 */ u8 field; // various bit-packed data
    /* 0x2 */ s8 gid;   // defines the draw id and chest opening animation
    /* 0x3 */ u8 textId;
    /* 0x4 */ u16 objectId;
} GetItemEntry; // size = 0x6
#define GIFIELD_20 (1 << 5)
#define GIFIELD_40 (1 << 6)

extern "C" {
#include "variables.h"
#include "functions.h"

extern GetItemEntry sGetItemTable[GI_MAX - 1];
}

// ALRIGHT BUCKLE UP. This function converts items to their "obtainable" form. This is used for various reasons, and you
// don't always want to convert items, depending on the context. For example, if you're rendering ammo in a shop for an
// item you don't have yet, you don't want to convert it, because you want to show the player what they're missing. But
// if you're rendering a Mask they already have, you want to convert it to a Rupee, because they can't get it again.
//
// Progressive Items:
// - If an RC was provided, and the player has already obtained the check, the item is converted to a Rupee.
// - If the player has obtained the highest level of the item, the item is converted to a Rupee.
// - Maybe this should convert to an associated ammo item if applicable? The main issue is we check if this returns a
// Rupee to determine if it's obtainable in some cases.
//
// Keys, Fairies and Skulltulas:
// - If the settings deem this item should be persistent (NOT IMPLEMENTED YET), and the player has already obtained the
// check, the item is converted to a Rupee.
//
// Ammo:
// - If the player does not have the associated item to use the ammo, the item is converted to a Rupee.
//
// Refills:
// - If the player has no empty bottles, the item is converted to a Rupee.
//
// Heart Pieces/Containers/One-Time Items:
// - If the player has already obtained the check, the item is converted to a Rupee.
//
// Everything Else:
// - We check if the item has an associated GetItemEntry, and if it does, we check if vanilla would allow the player to
// obtain it. If not, we convert it to a Rupee.
RandoItemId Rando::ConvertItem(RandoItemId randoItemId, RandoCheckId randoCheckId) {
    bool hasObtainedCheck = false;
    if (randoCheckId != RC_UNKNOWN) {
        hasObtainedCheck = RANDO_SAVE_CHECKS[randoCheckId].obtained;
    }

    u8 vanillaCantObtain = false;
    if (Rando::StaticData::Items[randoItemId].itemId != ITEM_NONE &&
        Rando::StaticData::Items[randoItemId].getItemId != GI_NONE) {
        GetItemEntry* giEntry = &sGetItemTable[Rando::StaticData::Items[randoItemId].getItemId - 1];
        vanillaCantObtain =
            ((Item_CheckObtainability(giEntry->itemId) != ITEM_NONE) && (giEntry->field & GIFIELD_20)) ||
            ((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) && (giEntry->field & GIFIELD_40));
    }

    switch (randoItemId) {
        case RI_UNKNOWN:
            return RI_RUPEE_BLUE;
        case RI_PROGRESSIVE_BOMB_BAG:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                return RI_BOMB_BAG_20;
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 1) {
                return RI_BOMB_BAG_30;
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 2) {
                return RI_BOMB_BAG_40;
            } else {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOMB_BAG_20:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 1) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOMB_BAG_30:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 2) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOMB_BAG_40:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_PROGRESSIVE_BOW:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                return RI_BOW;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 1) {
                return RI_QUIVER_40;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 2) {
                return RI_QUIVER_50;
            } else {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOW:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 1) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_QUIVER_40:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 2) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_QUIVER_50:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_PROGRESSIVE_MAGIC:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            } else if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                return RI_SINGLE_MAGIC;
            } else if (!gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                return RI_DOUBLE_MAGIC;
            } else {
                return RI_RUPEE_BLUE;
            }
            break;
        // TODO: These should be based on a persistent setting
        case RI_WOODFALL_SMALL_KEY:
        case RI_SNOWHEAD_SMALL_KEY:
        case RI_GREAT_BAY_SMALL_KEY:
        case RI_STONE_TOWER_SMALL_KEY:
        case RI_CLOCK_TOWN_STRAY_FAIRY:
        case RI_WOODFALL_STRAY_FAIRY:
        case RI_SNOWHEAD_STRAY_FAIRY:
        case RI_GREAT_BAY_STRAY_FAIRY:
        case RI_STONE_TOWER_STRAY_FAIRY:
        case RI_GS_TOKEN_SWAMP:
        case RI_GS_TOKEN_OCEAN:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            } else {
                return randoItemId;
            }
            break;
        case RI_HEART_PIECE:
        case RI_HEART_CONTAINER:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            } else {
                return randoItemId;
            }
            break;
        case RI_MILK_REFILL:
        case RI_FAIRY_REFILL:
        case RI_RED_POTION_REFILL:
        case RI_BLUE_POTION_REFILL:
        case RI_GREEN_POTION_REFILL:
            if (!Inventory_HasEmptyBottle()) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_ARROWS_10:
        case RI_ARROWS_30:
        case RI_ARROWS_50:
            if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOMBCHU:
        case RI_BOMBCHU_5:
        case RI_BOMBCHU_10:
        case RI_BOMBS_5:
        case RI_BOMBS_10:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_SHIELD_HERO:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_SHIELD_MIRROR:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_MIRROR) {
                return RI_RUPEE_BLUE;
            }
            break;
        case RI_BOTTLE_EMPTY:
            if (hasObtainedCheck) {
                return RI_RUPEE_BLUE;
            }

            for (s32 slot = SLOT_BOTTLE_1; slot <= SLOT_BOTTLE_6; slot++) {
                if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_NONE) {
                    return randoItemId;
                }
            }
            return RI_RUPEE_BLUE;
        case RI_MOONS_TEAR:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) ? RI_RUPEE_BLUE : randoItemId;
        case RI_DEED_LAND:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) ? RI_RUPEE_BLUE : randoItemId;
        case RI_DEED_SWAMP:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) ? RI_RUPEE_BLUE : randoItemId;
        case RI_DEED_MOUNTAIN:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) ? RI_RUPEE_BLUE : randoItemId;
        case RI_DEED_OCEAN:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) ? RI_RUPEE_BLUE : randoItemId;
        case RI_ROOM_KEY:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY) ? RI_RUPEE_BLUE : randoItemId;
        case RI_LETTER_TO_MAMA:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_LETTER_TO_KAFEI:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI) ? RI_RUPEE_BLUE : randoItemId;
        case RI_PENDANT_OF_MEMORIES:
            return Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES) ? RI_RUPEE_BLUE : randoItemId;
        case RI_DOUBLE_DEFENSE:
            return gSaveContext.save.saveInfo.playerData.doubleDefense ? RI_RUPEE_BLUE : randoItemId;
        case RI_DOUBLE_MAGIC:
            return gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired ? RI_RUPEE_BLUE : randoItemId;
        case RI_SINGLE_MAGIC:
            return gSaveContext.save.saveInfo.playerData.isMagicAcquired ? RI_RUPEE_BLUE : randoItemId;
        case RI_GREAT_SPIN_ATTACK:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK) ? RI_RUPEE_BLUE : randoItemId;
        case RI_WOODFALL_BOSS_KEY:
            return CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_WOODFALL_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_WOODFALL_COMPASS:
            return CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_INDEX_WOODFALL_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_WOODFALL_MAP:
            return CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_INDEX_WOODFALL_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SNOWHEAD_BOSS_KEY:
            return CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_SNOWHEAD_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SNOWHEAD_COMPASS:
            return CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_INDEX_SNOWHEAD_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SNOWHEAD_MAP:
            return CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_INDEX_SNOWHEAD_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_GREAT_BAY_BOSS_KEY:
            return CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_GREAT_BAY_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_GREAT_BAY_COMPASS:
            return CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_INDEX_GREAT_BAY_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_GREAT_BAY_MAP:
            return CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_INDEX_GREAT_BAY_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_STONE_TOWER_BOSS_KEY:
            return CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_INDEX_STONE_TOWER_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_STONE_TOWER_COMPASS:
            return CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_INDEX_STONE_TOWER_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        case RI_STONE_TOWER_MAP:
            return CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_INDEX_STONE_TOWER_TEMPLE) ? RI_RUPEE_BLUE : randoItemId;
        // These items are technically fine to receive again because they don't do anything, but we'll convert them to
        // ensure it's clear to the player something didn't go wrong.
        // Quest Items
        case RI_REMAINS_ODOLWA:
            return CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_REMAINS_GOHT:
            return CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) ? RI_RUPEE_BLUE : randoItemId;
        case RI_REMAINS_GYORG:
            return CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) ? RI_RUPEE_BLUE : randoItemId;
        case RI_REMAINS_TWINMOLD:
            return CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_BOSSA_NOVA:
            return CHECK_QUEST_ITEM(QUEST_SONG_BOSSA_NOVA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_ELEGY:
            return CHECK_QUEST_ITEM(QUEST_SONG_ELEGY) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_EPONA:
            return CHECK_QUEST_ITEM(QUEST_SONG_EPONA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_HEALING:
            return CHECK_QUEST_ITEM(QUEST_SONG_HEALING) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_LULLABY_INTRO:
            return CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_LULLABY:
            return CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_OATH:
            return CHECK_QUEST_ITEM(QUEST_SONG_OATH) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_SARIA:
            return CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_SOARING:
            return CHECK_QUEST_ITEM(QUEST_SONG_SOARING) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_SONATA:
            return CHECK_QUEST_ITEM(QUEST_SONG_SONATA) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_STORMS:
            return CHECK_QUEST_ITEM(QUEST_SONG_STORMS) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_SUN:
            return CHECK_QUEST_ITEM(QUEST_SONG_SUN) ? RI_RUPEE_BLUE : randoItemId;
        case RI_SONG_TIME:
            return CHECK_QUEST_ITEM(QUEST_SONG_TIME) ? RI_RUPEE_BLUE : randoItemId;
        // These items are technically fine to receive again because they don't do anything, but we'll convert them to
        // ensure it's clear to the player something didn't go wrong. We just simply check the inventory state
        // Masks
        case RI_MASK_BLAST:
        case RI_MASK_BREMEN:
        case RI_MASK_BUNNY:
        case RI_MASK_CAPTAIN:
        case RI_MASK_DEKU:
        case RI_MASK_GIANT:
        case RI_MASK_GORON:
        case RI_MASK_GREAT_FAIRY:
        case RI_MASK_KAFEIS_MASK:
        case RI_MASK_KAMARO:
        case RI_MASK_ROMANI:
        case RI_MASK_STONE:
        case RI_MASK_ZORA: {
            ItemId itemId = StaticData::Items[randoItemId].itemId;
            return INV_CONTENT(itemId) == itemId ? RI_RUPEE_BLUE : randoItemId;
        }
        default:
            break;
    }

    if (vanillaCantObtain) {
        return RI_RUPEE_BLUE;
    }

    return randoItemId;
}
