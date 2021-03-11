//
// Created by  KiviCode on 04/08/2020.
//

#ifndef NODENET_DATASET_MANAGER_H
#define NODENET_DATASET_MANAGER_H

#include "src/settings.h"
#include <string>
#include <vector>
#include "tinydir.h"

enum DatasetLoadType {
    WEB,
    WEB_PY,
    WEB_CMD,

    LOCAL,
    LOCAL_PY,
    LOCAL_CMD,
};

class DatasetManager {
public:

    char datasetPath[MAX_DATASET_PATH_LENGTH] = {0};
};

class Dataset {
public:
    char datasetPath[MAX_DATASET_PATH_LENGTH] = {0};

    DatasetLoadType loadType;

    std::string cmd = "";
    std::string py = "";

    std::pair<std::vector<std::string>, std::string> getLoadFromWebCode(const std::string& url) {
        std::string result;
        std::vector<std::string> libs = {"import subprocess"};
        loadType = WEB;

        result += R"(subprocess.run(["wget", ")" + url + "\"])\n";
        result += "dataset = ";
        return {libs, result};
    }
};

#endif //NODENET_DATASET_MANAGER_H
