//
// Created by  KiviCode on 04/08/2020.
//

#ifndef NODENET_LOGGER_H
#define NODENET_LOGGER_H

#include <string>
#include <utility>

using namespace std;

enum LogLevel {
    INFO,
    WARN,
    ERR
};

class Logger {
public:
    string* target;

    Logger(string* target) : target(target) {}

    void log(const string& msg, LogLevel level) {
        this->target->append(this->getPrefix(level) + msg + "\n");
    }

    void log(const string& msg) {
        this->log(msg, INFO);
    }

private:

    string getPrefix(LogLevel level) {
        switch (level) {

            case INFO:
                return "[INFO  ] ";

            case WARN:
                return "[WARN  ] ";

            case ERR:
                return "[ERROR ] ";
        }
    }
};

#endif //NODENET_LOGGER_H
