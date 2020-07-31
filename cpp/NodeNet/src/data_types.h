//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_DATA_TYPES_H
#define NODENET_DATA_TYPES_H

#include "utils.cpp"
#include <array>
#include <vector>
#include <imnodes.h>
#include <map>
#include <any>


class Node;
class Link;
class Editor;


enum SliderDataType : int {
    INTEGER,
    FLOAT,
    STRING,
    CODE
};

enum PrivatePinType {
    CUSTOM,
    FINISH,
    START
};

class NodeIOPin {
public:
    std::string name = "";
    bool isEditable = false;

    SliderDataType dataType = SliderDataType::INTEGER;

    float minSliderVal = 0;
    float maxSliderVal = 1;
    float sliderSpeed = .1;

    NodeIOPin(std::string name, bool isEditable);

    NodeIOPin(std::string name, bool isEditable, SliderDataType dataType);

    NodeIOPin(std::string name, bool isEditable, float minSliderVal, float maxSliderVal, float sliderSpeed);

    NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, float minSliderVal, float maxSliderVal, float sliderSpeed);
};

class NodeConfig {
public:
    std::string title = "";

    std::vector<NodeIOPin> inputs = {};
    std::vector<NodeIOPin> outputs = {};

    std::string baseCode = "";

    NodeConfig() = default;

    NodeConfig(std::string title, std::vector<NodeIOPin> inputs, std::vector<NodeIOPin> outputs);

    void setCode(std::string newCode);
};



class Node {
public:
    int id;
    float value;
    char* modelName[1024] = {0};

    std::string baseCode = "";
    std::string processedCode = "";

    NodeConfig config = NodeConfig();

    PrivatePinType _type = PrivatePinType::CUSTOM;

    std::vector<int>  inputIds = {};
    std::vector<int> outputIds = {};

    std::map<int, std::pair<float, std::string>>  inputs = {};
    std::map<int, std::pair<float, std::string>> outputs = {};

    Node(int i, float v);

    std::pair<int, bool> globalPinIdToLocal(int globalId) const;

    void setBaseCode(std::string code);

    void setProcessedCode(std::string code);

    void setConfig(NodeConfig newConfig);

    void replaceInputsWithValues(std::vector<std::pair<bool, std::array<int, 3>>>& ioCodePositions);

    void generateProcessedCode(const std::vector<Link>&);

};

class Link {
public:
    int id;
    int start_attr, end_attr;

    void sort(std::vector<Node> nodes);

    Link();
    Link(int i, int start, int end);
};

class Editor {
public:
    imnodes::EditorContext *context = nullptr;
    std::vector<Node> nodes;
    std::vector<Link> links;

    std::vector<int> inputNodeIds  = {};
    std::vector<int> finishNodeIds = {};

    int current_id = 0;

    std::vector<Link> getLinksOfNode(Node& node);

    std::pair<Link, bool> getLinkToPin(Node& node, int localPinId);

    bool pinHasLink(int pinId);

    std::pair<Node&, bool> getNodeThatHasPinById(int id);

    std::pair<PinLocation, bool> getNextPinLocation(Node& node, int localPinId); // returns: {{parent node id, local pin id}, was found?}
};

#endif //NODENET_DATA_TYPES_H
