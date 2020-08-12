//
// Created by  KiviCode on 04/08/2020.
//

#ifndef NODENET_CODE_MANAGER_H
#define NODENET_CODE_MANAGER_H

#include <vector>
#include "data_types.h"
#include <fstream>


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

    std::string getCode(){return this->code;}

    char fileSavePath[1024] = {};
    std::vector<std::string> imports = {};
    std::string optimizer = "";

    void addLib(std::string importStatement) {
        this->imports.push_back(importStatement);
    }

    void generateCode(Editor& editor, int indentation = 1){
        this->code = ""; // clear
        this->generateHeader();
        this->code += "\n\ndef generate_model():\n";
        for (int i : editor.finishNodeIds) {
            auto _code = editor.nodes[i].generateProcessedCode(editor, indentation);
            this->code += _code;
            this->code += "\n" + generateIndentation(indentation) + "return ";
            this->code += std::string(std::any_cast<char*>(editor.nodes[i].getInputValueById(editor, 1)));
        }
        this->code += "\n\n";
        this->generateDatasetLoader();
        this->code += "\n";
        this->generateBody();
    }

    void save() {
        std::ofstream file;
        file.open(this->fileSavePath);
        file << this->code;
        file.close();
    }

private:
    std::vector<std::string> modelNames = {};
    std::string code;

    void generateHeader() {
        for(std::string import : this->imports) this->code.append(import + "\n");
    }

    void generateBody() {
        this->code += "if __name__ == '__main__':\n";
        this->code += "\tmodel = generate_model()\n";
        this->code += "\tx_train, y_train, x_test, y_test = load_dataset()\n\n";
        this->code += std::string("\tmodel.compile(optimizer=\'") + optimizer + std::string("\', loss='mse')\n\n");
        this->code += "\tprint('[INFO ] Training started')\n";
        this->code += "\tmodel.fit(x_train, y_train, batch_size=32, epochs=10)\n";
        this->code += "\tprint('[INFO ] Training finished')\n\n";
        this->code += "\tprint(model.predict(np.array([[1, 0]]))[0])\n";
    }

    void generateDatasetLoader() {
        this->code += "def load_dataset():\n";
        this->code += "\treturn np.array([[0,0], [0,1], [1,1]]), np.array([0, 1, 0]), [], []\n";
    }
};

#endif //NODENET_CODE_MANAGER_H
