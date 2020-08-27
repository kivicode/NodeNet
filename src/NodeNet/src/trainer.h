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
#include "code_manager.h"
#include "nodespy.h"
#include "utils.h"
#include <Python.h>
#include <thread>

#include <unistd.h>
#include "ThreadsManager.h"


std::string getFileName(const std::string& path) {
    auto parts = split(path, '/'); // TODO: Add Win delimiter support
    return parts.at(parts.size()-1); // get last part
}

std::string getParentDir(const std::string& path) {
    auto parts = split(path, '/'); // TODO: Add Win delimiter support
    std::string out;
    for(unsigned long i = 0; i < parts.size()-1; i++) { // concat all except the last one
        out += parts[i] + "/";
    }
    return out;
}

inline std::string generateInjectedFile(const std::string& basePath, const std::string& injectionPath) {
    auto code = readFile(basePath);
    auto injection = readFile(injectionPath);
    auto fullCode = injection + "\n\n" + code;
    auto fullPath = getParentDir(basePath) + "injected_"  + getFileName(basePath);
    writeFile(fullPath, fullCode);
    printf("%s\n", getParentDir(basePath).c_str());
    PyRun_SimpleString(("import sys;sys.path.append(\"" + getParentDir(basePath) + "\")").c_str()); // TODO: Make less ugly

    auto moduleName = split(getFileName(fullPath), '.');
    return moduleName[moduleName.size()-2]; // TODO: Add Win delimiter support (universal delimiter)
}

inline PyObject* loadTrainer(const std::string& moduleName) {
    PyObject *trainer_str = PyUnicode_FromString(moduleName.c_str());
    auto outp = PyImport_Import(trainer_str);
    return outp;
}


class MlTrainer {

public:

    std::vector<std::string> availableOptimizers = {"SGD", "RMSprop", "Adam", "Adadelta", "Adagrad", "Adamax", "Nadam", "Ftrl"};
    int selectedOptimizer = 0;

    std::string pythonPath = "/usr/local/bin/py";

    bool isTraining = false;
    Logger* logTarget;

    explicit MlTrainer(Logger* logTarget): logTarget(logTarget){}

    bool train(Editor& editor, CodeManager& codeManager, const std::string& filePath) {
//        codeManager.optimizer = availableOptimizers.at(selectedOptimizer);
//        codeManager.generateCode(editor);
//        codeManager.save();
        return this->start_exec(pythonPath + " " + filePath);
    }

    bool start_exec(const std::string& command) {
        if (!pythonThreadIninted) {
            printf("\n\n\nStart thread\n\n\n\n");
            pythonThread = std::thread(run);
            pythonThreadIninted = true;
        }
        pythonThreadPaused = false;
        printf("\n\n\nUnpause thread\n\n\n\n");
        return true;
    }

    void exec_step() {
    }

private:

    static void run() {
        if (PyImport_AppendInittab("nodespy", PyInit_nodespy) == -1) {
            fprintf(stderr, "Error: could not extend in-built modules table\n");
            exit(1);
        }

        Py_Initialize();
        while (true) {

            auto moduleName = generateInjectedFile("/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/py/test.py",
                                                   "/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/py/injection.py");
            std::cout << "Model path: " << moduleName << "\n";
            PyObject *trainer = loadTrainer(moduleName);

            PyObject *load = PyObject_GetAttrString(trainer, (char *) "load_dataset");
            PyObject *train = PyObject_GetAttrString(trainer, (char *) "train");

            PyObject_CallObject(load, nullptr);
            PyObject_CallObject(train, nullptr);

            if (!pythonThreadAlive) break;
            pythonThreadPaused = true;

            while (pythonThreadPaused) {std::this_thread::sleep_for(std::chrono::microseconds(10));}
        }
        Py_Finalize();
    }

};




#endif //NODENET_TRAINER_H
