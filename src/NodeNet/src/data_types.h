//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_DATA_TYPES_H
#define NODENET_DATA_TYPES_H

#include "utils.cpp"
#include "exceptions.h"
#include <array>
#include <vector>
#include <imnodes.h>
#include <map>
#include <any>
#include <string>

#include <cereal/types/string.hpp>
#include <cereal/types/common.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

class Node;

class Link;

class Editor;

typedef std::pair<std::string, tinydir_file> NodeMenuItem;

struct CodeIODeclaration {
    std::string code;
    std::vector<std::string> names;
    std::vector<int> lineNumbers;
    std::vector<int> startPoses;
    std::vector<int> endPoses;

    CodeIODeclaration(std::string code) : code(std::move(code)) {}

    void add(int lineNumber, int startPos, int endPos, std::string name = "") {
        this->lineNumbers.push_back(lineNumber);
        this->startPoses.push_back(startPos);
        this->endPoses.push_back(endPos);
        this->names.push_back(name);
    }
};


enum SliderDataType : int {
    INTEGER,
    INTEGER_DRAG,
    FLOAT,
    FLOAT_DRAG,
    STRING,
    STRING_SELECTOR,
    CODE
};

enum PrivatePinType : int {
    CUSTOM,
    FINISH,
    START,
    NULL_NODE
};

template<class Archive>
void serialize(Archive &archive, ImVec2 &vec) {
    archive(vec.x, vec.y);
}


class NodeIOPin {
public:
    std::string name = "";
    bool isEditable = false;

    SliderDataType dataType = SliderDataType::INTEGER;

    float minSliderVal = 0;
    float maxSliderVal = 1;
    float sliderSpeed = .1;

    std::vector<std::string> options = {};
    int selectedOption = 0;

    bool isStaff = false;
    bool isInput = false;

    NodeIOPin() = default;

    NodeIOPin(std::string name, bool isEditable);

    NodeIOPin(std::string name, bool isEditable, SliderDataType dataType);

    NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, std::vector<std::string> options);

    NodeIOPin(std::string name, bool isEditable, float minSliderVal, float maxSliderVal, float sliderSpeed);

    NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, float minSliderVal, float maxSliderVal,
              float sliderSpeed);

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(name), CEREAL_NVP(isEditable), CEREAL_NVP(static_cast<int>(dataType)),
                CEREAL_NVP(minSliderVal), CEREAL_NVP(maxSliderVal), CEREAL_NVP(sliderSpeed), CEREAL_NVP(options),
                CEREAL_NVP(selectedOption), CEREAL_NVP(isStaff));
    }

};

class NodeConfig {
public:
    float headColor[3] = {0/255.f, 108/255.f, 187/255.f};

    std::string title = "";
    PrivatePinType type = CUSTOM;

    std::vector<NodeIOPin> inputs = {};
    std::vector<NodeIOPin> outputs = {};

    std::string baseCode = "";

    NodeConfig() = default;

    NodeConfig(std::string title, std::vector<NodeIOPin> inputs, std::vector<NodeIOPin> outputs);

    void setCode(std::string newCode);

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(title), CEREAL_NVP(inputs), CEREAL_NVP(baseCode));
    }
};

struct IOData {
    char s[INPUT_BUFF_SIZE] = {};
    float f = 0;
    int i = 0;

    SliderDataType dtype = INTEGER;

    void empty() {
        strcpy(this->s, "");
        this->f = 0;
        this->i = 0;
    }

    void setString(std::string str) {
        strcpy(this->s, str.c_str());
    }

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(s), CEREAL_NVP(f), CEREAL_NVP(i));
    }
};


class Node {
public:
    int id;
    float value;

    ImVec2 gridPosition = {0, 0};

    std::string baseCode = "";
    std::string processedCode = "";

    NodeConfig config = NodeConfig();

    PrivatePinType _type = PrivatePinType::CUSTOM;

    imnodes::NodeMarks mark = imnodes::NodeMarks::None;
    std::string markDescription = "";

    std::vector<int> inputIds = {};
    std::vector<int> outputIds = {};

    std::map<int, IOData> inputs = {};
    std::map<int, IOData> outputs = {};

    Node() = default;

    Node(int i, float v);

    [[nodiscard]] std::pair<int, bool> globalPinIdToLocal(int globalId) const;

    void setBaseCode(std::string code);

    void setConfig(NodeConfig newConfig);

    std::string generateProcessedCode(Editor &editor, int indentation);

    void init();

    NodeException checkExceptions(Editor &editor);

    int inputIdByName(std::string &name);

    std::any getInputValueById(Editor &editor, int localIndex);

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(gridPosition),
                CEREAL_NVP(config),
                CEREAL_NVP(baseCode), CEREAL_NVP(processedCode),
                CEREAL_NVP(static_cast<int>(_type)), CEREAL_NVP(static_cast<int>(mark)), CEREAL_NVP(markDescription),
                CEREAL_NVP(inputIds), CEREAL_NVP(outputIds),
                CEREAL_NVP(inputs), CEREAL_NVP(outputs));
    }

private:
    std::string getInputName(Editor &editor);

    void setProcessedCode(std::string code);

    void replaceInputsWithValues(Editor &editor);

    void deleteOutputsFromCode();

    static std::string getPrevVarname(Editor &editor, std::pair<Link, bool> link);

    bool hasName(Editor &editor);

    bool hasLink(Editor &editor);
};

class Link {
public:
    int id;
    int start_attr, end_attr;

    void sort(std::vector<Node> nodes);

    Link();

    Link(int i, int start, int end);

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(id),
                CEREAL_NVP(start_attr),
                CEREAL_NVP(end_attr));
    }
};

class Editor {
public:
    Editor() = default;

    std::vector<Node> nodes;
    std::vector<Link> links;

    std::vector<int> inputNodeIds = {};
    std::vector<int> finishNodeIds = {};

    int current_node_id = 0;
    int current_link_id = 0;

    std::vector<Link> getLinksFromNode(Node &node);

    std::vector<Link> getLinksToNode(Node &node);

    std::vector<Link> getLinksOfNode(Node &node);

    Node &nodeById(int id);

    std::pair<Link, bool> getLinkToPin(Node &node, int localPinId);

    bool pinHasLink(int pinId);

    std::pair<Node &, bool> getNodeThatHasPinById(int id);

    std::pair<PinLocation, bool>
    getNextPinLocation(Node &node, int localPinId); // returns: {{parent node id, local pin id}, was found?}

    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(nodes),
                CEREAL_NVP(links),
                CEREAL_NVP(inputNodeIds),
                CEREAL_NVP(finishNodeIds),
                CEREAL_NVP(current_node_id));
    }
};

#endif //NODENET_DATA_TYPES_H
