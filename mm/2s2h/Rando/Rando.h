#ifndef RANDO_H
#define RANDO_H

#include "StaticData/StaticData.h"
#include "Types.h"

#define RANDO_SAVE_CHECKS gSaveContext.save.shipSaveInfo.rando.randoSaveChecks

namespace Rando {

extern std::vector<std::string> spoilerFileOptions;
bool HandleFileDropped(std::string filePath);

void Init();
void DrawItem(RandoItemId randoItemId);
void GiveItem(RandoItemId randoItemId);
RandoItemId ConvertItem(RandoItemId randoItemId);
void DrawMenu();

} // namespace Rando

#endif
