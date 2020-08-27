#pragma once

#include <Python.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include "plotter.h"

class NodeSpyLogger {
public:
    std::map<std::string, float> logs;
    std::set<std::string> keys;
    Logger*  m_logger  = nullptr;
    Plotter* m_plotter = nullptr;

    NodeSpyLogger() = default;

    void setLogger(Logger* logger){
        m_logger = logger;
    }

    void setPlotter(Plotter* plotter){
        m_plotter = plotter;
    }

    void log(const std::string& key, float val, bool echo=true, bool plot=true) {
        logs[key] = val;
        if (echo)
            m_logger->log(key + ": " + STR(logs[key]), LogLevel::NONE);

        if (plot)
            m_plotter->push(key, val);
    }

    void echoToLog() {
        for (const auto& key : keys) {
            m_logger->log(key + ": " + STR(logs[key]), LogLevel::NONE);
        }
    }
};

NodeSpyLogger spyLogger;

static PyObject* nodespy_start_logging(PyObject *self, PyObject *args) {

    std::cout << spyLogger.logs["loss"] << "\n";
    return PyLong_FromLong(0);
}

static PyObject* nodespy_push_log(PyObject *self, PyObject *args) {
    char* key;
    float val=-1;

    if (!PyArg_ParseTuple(args, "sf", &key, &val)) {
        std::cout << "Fuck\n";
        return nullptr;
    }
    spyLogger.log(key, val);
    Py_RETURN_NONE;
}

static PyMethodDef NodespyMethods[] = {
    {"start_logging",  nodespy_start_logging, METH_VARARGS, ""},
    {"push_log",       nodespy_push_log,      METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef nodespy_module = {
    PyModuleDef_HEAD_INIT,
    "nodespy",   /* name of module */
    NULL,      /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    NodespyMethods
};

PyMODINIT_FUNC PyInit_nodespy(void) {
    return PyModule_Create(&nodespy_module);
}