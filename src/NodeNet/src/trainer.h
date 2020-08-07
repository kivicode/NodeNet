//
// Created by  KiviCode on 03/08/2020.
//

#ifndef NODENET_TRAINER_H
#define NODENET_TRAINER_H

#include <string>

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <string>
#include "logger.h"

class MlTrainer {
public:

    std::string pythonPath = "/usr/local/bin/py";
    FILE* pipe;
    bool isTraining;
    Logger* logTarget;

    MlTrainer(Logger* logTarget): logTarget(logTarget){}

    bool train(const std::string& filePath) {
        return this->start_exec(pythonPath + " " + filePath);
    }

    bool start_exec(const std::string& command) {
        std::string result;

        // Open pipe to file
        pipe = popen((command + " 2>&1").c_str(), "r");
        if (!pipe) {
            return false;
        }
        this->isTraining = true;

        return true;
    }

    void exec_step() {
        if(!isTraining) return;

        char buffer[1024];
        // read till end of process:
        if (!feof(pipe)) {

            // use buffer to read and add to result
            if (fgets(buffer, 1024, pipe) != nullptr) {
                logTarget->log(buffer, NONE);
            }
        } else {
            isTraining = false;
            pclose(pipe);
        }
    }


};




#endif //NODENET_TRAINER_H
