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

std::string generateInjectedFile(const std::string& basePath, const std::string& injectionPath) {
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

PyObject* loadTrainer(const std::string& moduleName) {
    PyObject *trainer_str = PyUnicode_FromString(moduleName.c_str());
    return PyImport_ReloadModule(PyImport_Import(trainer_str));
}

bool abc = 0;
void test_run() {
    abc = 2;
    if (PyImport_AppendInittab("nodespy", PyInit_nodespy) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }


    Py_Initialize();

    auto moduleName   = generateInjectedFile("/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/py/test.py", "/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/py/injection.py");
    std::cout << "Model path: " << moduleName << "\n";
    PyObject *trainer = loadTrainer(moduleName);

    PyObject *load  = PyObject_GetAttrString(trainer, (char *) "load_dataset");
    PyObject *train = PyObject_GetAttrString(trainer, (char *) "train");

    PyObject_CallObject(load,  nullptr);
    PyObject_CallObject(train, nullptr);

    Py_Finalize();
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
        codeManager.optimizer = availableOptimizers.at(selectedOptimizer);
        codeManager.generateCode(editor);
        codeManager.save();
        return this->start_exec(pythonPath + " " + filePath);
    }

    bool start_exec(const std::string& command) {
        std::thread t(test_run);
        usleep(2000000);
        std::cout << "\n\n\n\n\nsdsdjogsfp[kfogp\n\n\n" << abc << "\n";
        t.join();
        return true;
    }

    void exec_step() {
    }

};




#endif //NODENET_TRAINER_H
