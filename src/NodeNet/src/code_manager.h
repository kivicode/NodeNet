//
// Created by  KiviCode on 04/08/2020.
//

#ifndef NODENET_CODE_MANAGER_H
#define NODENET_CODE_MANAGER_H

#include <vector>
#include "data_types.h"

class CodeManager {
public:
    static std::vector<std::string> getAvailableModelNames(Editor &editor){
        std::vector<std::string> result = {};
        for (int i : editor.finishNodeIds) {
            Node& node = editor.nodes.at(i);
            result.emplace_back(node.inputs[node.inputIds[1]].s);
        }
        return result;
    }
    void generateCode(Editor& editor){
        this->code = "";
        for (int i : editor.finishNodeIds) {
            auto _code = editor.nodes[i].generateProcessedCode(editor);
            std::cout << editor.nodes[i].config.title;
            this->code += _code;
            this->code += "\n\n";
        }
    }
    std::string getCode(){return this->code;}

private:
    std::vector<std::string> modelNames = {};
    std::string code;
};

#endif //NODENET_CODE_MANAGER_H
