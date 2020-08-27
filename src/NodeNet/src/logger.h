//
// Created by  KiviCode on 04/08/2020.
//

#ifndef NODENET_LOGGER_H
#define NODENET_LOGGER_H


#include <string>
#include <utility>
#include <imgui.h>

using namespace std;

enum LogLevel {
    INFO,
    WARN,
    ERR,
    NONE
};

class Logger {
public:
    string* target;

    explicit Logger(string* target) : target(target) {}

    void draw() {
        ImGui::TextUnformatted(this->target->c_str());
        if (ScrollToBottom) ImGui::SetScrollHereY(1.f);
        ScrollToBottom = false;
    }

    void log(const string& msg, LogLevel level) {
        ScrollToBottom = true;
        this->target->append(getPrefix(level) + msg + "\n");
    }

    void log(const string& msg) {
        this->log(msg, INFO);
    }

private:
    bool ScrollToBottom = true;

    static string getPrefix(LogLevel level) {
        switch (level) {

            case INFO:
                return "[INFO  ] ";

            case WARN:
                return "[WARN  ] ";

            case ERR:
                return "[ERROR ] ";

            case NONE:
                return "";
        }
    }
};
#endif //NODENET_LOGGER_H
