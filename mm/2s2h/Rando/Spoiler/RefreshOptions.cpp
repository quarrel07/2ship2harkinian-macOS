#include "Spoiler.h"
#include <libultraship/libultraship.h>
#include <filesystem>
#include "BenPort.h"

std::vector<std::string> Rando::Spoiler::spoilerOptions;
const std::filesystem::path randomizerFolderPath(Ship::Context::GetPathRelativeToAppDirectory("randomizer",
                                                                                              appShortName));

void Rando::Spoiler::RefreshOptions() {
    Rando::Spoiler::spoilerOptions.clear();

    Rando::Spoiler::spoilerOptions.push_back("Generate New Seed");
    s32 spoilerFileIndex = -1;

    // ensure the randomizer folder exists
    if (!std::filesystem::exists(randomizerFolderPath)) {
        std::filesystem::create_directory(randomizerFolderPath);
    }
    for (const auto& entry : std::filesystem::directory_iterator(randomizerFolderPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            Rando::Spoiler::spoilerOptions.push_back(fileName);

            if (fileName == CVarGetString("gRando.SpoilerFile", "")) {
                spoilerFileIndex = Rando::Spoiler::spoilerOptions.size() - 1;
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
