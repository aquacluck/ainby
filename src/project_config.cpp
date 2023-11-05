#include "project_config.hpp"

static ProjectConfig projectConfig = ProjectConfig();
ProjectConfig& ProjectConfig::Get(void) { return projectConfig; }

void ProjectConfig::LoadFromJson(const json& j) {
    // more magic than i need rn but maybe neat?
    // projectConfig = j.template get<ProjectConfig>();

    projectConfig.appCachePath.assign(j["appCachePath"]);
    projectConfig.romfsPath.assign(j["romfsPath"]);
    // projectConfig.modRomfsPath.assign(j["modRomfsPath"]);

    projectConfig.titleName = j["titleName"];
    projectConfig.globalAinbPacks = j["globalAinbPacks"];
    projectConfig.rootAinbFolders = j["rootAinbFolders"];
    projectConfig.zsdicPackFile = j["zsdicPackFile"];
}
