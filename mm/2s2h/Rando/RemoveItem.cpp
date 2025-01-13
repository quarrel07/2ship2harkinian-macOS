#include "Rando/Rando.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CLEAR_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags &= ~(1 << owlId))

void Rando::RemoveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_SNOWHEAD_TEMPLE]--;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_GREAT_BAY_TEMPLE]--;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_INDEX_STONE_TOWER_TEMPLE]--;
            break;
        case RI_GREAT_SPIN_ATTACK:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = false;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
            break;
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = false;
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
            break;
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_MAP:
        case RI_WOODFALL_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_WOODFALL_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_MAP:
        case RI_SNOWHEAD_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_SNOWHEAD_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_MAP:
        case RI_GREAT_BAY_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_GREAT_BAY_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_MAP:
        case RI_STONE_TOWER_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_INDEX_STONE_TOWER_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
            // TODO
            break;
        case RI_GS_TOKEN_SWAMP: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINSTA1);
            gSaveContext.save.saveInfo.skullTokenCount =
                ((int)(skullTokenCount & 0xFFFF) << 0x10) | (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
            break;
        }
        case RI_GS_TOKEN_OCEAN: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINDAN2);
            gSaveContext.save.saveInfo.skullTokenCount =
                (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) | (skullTokenCount & 0xFFFF);
            break;
        }
        case RI_MOONS_TEAR:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
            break;
        case RI_DEED_LAND:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
            break;
        case RI_DEED_SWAMP:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
            break;
        case RI_DEED_MOUNTAIN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
            break;
        case RI_DEED_OCEAN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
            break;
        case RI_ROOM_KEY:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
            break;
        case RI_LETTER_TO_MAMA:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
            break;
        case RI_LETTER_TO_KAFEI:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
            break;
        case RI_PENDANT_OF_MEMORIES:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
            break;
        case RI_OWL_CLOCK_TOWN_SOUTH:
            CLEAR_OWL_WARP(OWL_WARP_CLOCK_TOWN);
            break;
        case RI_OWL_GREAT_BAY_COAST:
            CLEAR_OWL_WARP(OWL_WARP_GREAT_BAY_COAST);
            break;
        case RI_OWL_IKANA_CANYON:
            CLEAR_OWL_WARP(OWL_WARP_IKANA_CANYON);
            break;
        case RI_OWL_MILK_ROAD:
            CLEAR_OWL_WARP(OWL_WARP_MILK_ROAD);
            break;
        case RI_OWL_MOUNTAIN_VILLAGE:
            CLEAR_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE);
            break;
        case RI_OWL_SNOWHEAD:
            CLEAR_OWL_WARP(OWL_WARP_SNOWHEAD);
            break;
        case RI_OWL_SOUTHERN_SWAMP:
            CLEAR_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP);
            break;
        case RI_OWL_STONE_TOWER:
            CLEAR_OWL_WARP(OWL_WARP_STONE_TOWER);
            break;
        case RI_OWL_WOODFALL:
            CLEAR_OWL_WARP(OWL_WARP_WOODFALL);
            break;
        case RI_OWL_ZORA_CAPE:
            CLEAR_OWL_WARP(OWL_WARP_ZORA_CAPE);
            break;
        case RI_TINGLE_MAP_CLOCK_TOWN:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
            break;
        case RI_TINGLE_MAP_WOODFALL:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
            break;
        case RI_TINGLE_MAP_SNOWHEAD:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
            break;
        case RI_TINGLE_MAP_ROMANI_RANCH:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
            break;
        case RI_TINGLE_MAP_GREAT_BAY:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
            break;
        case RI_TINGLE_MAP_STONE_TOWER:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
            break;
        case RI_HEART_CONTAINER:
            gSaveContext.save.saveInfo.playerData.healthCapacity -= 0x10;
            gSaveContext.save.saveInfo.playerData.health =
                MIN(gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
            break;
        case RI_HEART_PIECE:
            if (GET_QUEST_HEART_PIECE_COUNT == 0) {
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                gSaveContext.save.saveInfo.playerData.healthCapacity -= 0x10;
                gSaveContext.save.saveInfo.playerData.health = MIN(
                    gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
            } else {
                DECREMENT_QUEST_HEART_PIECE_COUNT;
            }
            break;
        case RI_BOMBERS_NOTEBOOK:
            REMOVE_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK);
            break;
        case RI_SONG_ELEGY:
            REMOVE_QUEST_ITEM(QUEST_SONG_ELEGY);
            break;
        case RI_SONG_EPONA:
            REMOVE_QUEST_ITEM(QUEST_SONG_EPONA);
            break;
        case RI_SONG_HEALING:
            REMOVE_QUEST_ITEM(QUEST_SONG_HEALING);
            break;
        case RI_SONG_LULLABY_INTRO:
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
            break;
        case RI_SONG_LULLABY:
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY);
            break;
        case RI_SONG_NOVA:
            REMOVE_QUEST_ITEM(QUEST_SONG_BOSSA_NOVA);
            break;
        case RI_SONG_OATH:
            REMOVE_QUEST_ITEM(QUEST_SONG_OATH);
            break;
        case RI_SONG_SARIA:
            REMOVE_QUEST_ITEM(QUEST_SONG_SARIA);
            break;
        case RI_SONG_SOARING:
            REMOVE_QUEST_ITEM(QUEST_SONG_SOARING);
            break;
        case RI_SONG_SONATA:
            REMOVE_QUEST_ITEM(QUEST_SONG_SONATA);
            break;
        case RI_SONG_STORMS:
            REMOVE_QUEST_ITEM(QUEST_SONG_STORMS);
            break;
        case RI_SONG_SUN:
            REMOVE_QUEST_ITEM(QUEST_SONG_SUN);
            break;
        case RI_SONG_TIME:
            REMOVE_QUEST_ITEM(QUEST_SONG_TIME);
            break;
        case RI_REMAINS_GOHT:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_GOHT);
            break;
        case RI_REMAINS_GYORG:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_GYORG);
            break;
        case RI_REMAINS_ODOLWA:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_ODOLWA);
            break;
        case RI_REMAINS_TWINMOLD:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_TWINMOLD);
            break;
        // Ignore Ammo
        case RI_BOMBCHU:
        case RI_DEKU_STICK:
        case RI_DEKU_NUT:
        case RI_MILK_REFILL:
        case RI_RED_POTION_REFILL:
        case RI_GREEN_POTION_REFILL:
        case RI_BLUE_POTION_REFILL:
        case RI_FAIRY_REFILL:
        case RI_GOLD_DUST_REFILL:
            break;
        default:
            if (Rando::StaticData::Items[randoItemId].itemId < 77) {
                Inventory_DeleteItem(Rando::StaticData::Items[randoItemId].itemId,
                                     SLOT(Rando::StaticData::Items[randoItemId].itemId));
            }
            break;
    }
}
