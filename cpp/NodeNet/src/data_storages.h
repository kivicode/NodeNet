//
// Created by  KiviCode on 26/07/2020.
//
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <map>

#ifndef NODENET_DTYPES_H
#define NODENET_DTYPES_H

#include "utils.h"

enum SliderDataType : int {
    INTEGER,
    FLOAT,
    STRING
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

        NodeIOPin(std::string name, bool isEditable) {
            this->name = name;
            this->isEditable = isEditable;
        }

        NodeIOPin(std::string name, bool isEditable, SliderDataType dataType) {
            this->name = name;
            this->isEditable = isEditable;
            this->dataType = dataType;
        }

        NodeIOPin(std::string name, bool isEditable, float minSliderVal, float maxSliderVal, float sliderSpeed) {
            this->name = name;
            this->isEditable = isEditable;
            this->minSliderVal = minSliderVal;
            this->maxSliderVal = maxSliderVal;
            this->sliderSpeed = sliderSpeed;
        }

        NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, float minSliderVal, float maxSliderVal, float sliderSpeed) {
            this->name = name;
            this->isEditable = isEditable;
            this->minSliderVal = minSliderVal;
            this->maxSliderVal = maxSliderVal;
            this->sliderSpeed = sliderSpeed;
            this->dataType = dataType;
        }
};

class NodeConfig {
public:
    std::string title = "";

    std::vector<NodeIOPin> inputs = {};
    std::vector<NodeIOPin> outputs = {};

    std::string baseCode = "";

    NodeConfig() = default;

    NodeConfig(std::string title, std::vector<NodeIOPin> inputs, std::vector<NodeIOPin> outputs) {
        this->title =  std::move(title);
        this->inputs = std::move(inputs);
        this->outputs = outputs;
    }

    void setCode(std::string newCode) {
        this->baseCode = std::move(newCode);
    }
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

    std::map<int, float>  inputs = {};
    std::map<int, float> outputs = {};

    Node(const int i, const float v) : id(i), value(v) {}

    std::pair<int, bool> globalPinIdToLocal(int globalId) const {
        auto searchOverInputs  = findInVector<int>(this->inputIds,  globalId);
        auto searchOverOutputs = findInVector<int>(this->outputIds, globalId);
        if (searchOverInputs.second) return  searchOverInputs;
        if (searchOverOutputs.second) return searchOverOutputs;
        return {-1, false};
    }

    void setBaseCode(std::string code) {
        this->baseCode = std::move(code);
        this->processedCode = this->baseCode;
    }

    void setConfig(NodeConfig newConfig) {
        this->setBaseCode(newConfig.baseCode);
        this->config = std::move(newConfig);
    }

    void replaceInputsWithValues(const std::vector<std::pair<bool, std::pair<int, int>>>& ioCodePositions) {
        for (std::pair<bool, std::pair<int, int>> io : ioCodePositions) {
            bool isInput = io.first;
            int start = io.second.first;
            int length = io.second.second;



            if (isInput) {
                int funcNameSize = std::string("input(").length();
                std::cout << start << "\n";
                std::cout << start << "\n";
                this->processedCode.replace(start - funcNameSize, length + funcNameSize + 1, "1");
            }
            std::cout << "New code: \n" << this->processedCode;
        }
    }

};

class Link {
public:
    int id;
    int start_attr, end_attr;

    void sort(std::vector<Node> nodes) {
        bool firstIsInput = false;
        for (Node& node : nodes){
            if (VECTOR_CONTAINS(node.inputIds, this->start_attr)) {
                firstIsInput = true;
            } else if (VECTOR_CONTAINS(node.outputIds, this->start_attr)) {
                firstIsInput = false;
            }
        }
        if (firstIsInput) {
            std::swap(this->start_attr, this->end_attr);
        }
    }

    Link() : id(-1), start_attr(-1), end_attr(-1) {}
    Link(int i, int start, int end) : id(i), start_attr(start), end_attr(end) {}
};

class Editor {
    public:
        imnodes::EditorContext *context = nullptr;
        std::vector<Node> nodes;
        std::vector<Link> links;

        std::vector<int> inputNodeIds  = {};
        std::vector<int> finishNodeIds = {};

        int current_id = 0;

        std::vector<Link> getLinksOfNode(Node& node) {
            std::vector<Link> result = {};
            for (Link& link : this->links) {
                bool startIsInput = VECTOR_CONTAINS(node.inputIds, link.start_attr);
                bool endIsInput   = VECTOR_CONTAINS(node.inputIds, link.end_attr);

                bool startIsOutput = VECTOR_CONTAINS(node.outputIds, link.start_attr);
                bool endIsOutput   = VECTOR_CONTAINS(node.outputIds, link.end_attr);

                if (startIsInput || endIsInput || startIsOutput || endIsOutput) {
                    result.push_back(link);
                }
            }
            return result;
        }

        std::pair<Link, bool> getLinkToPin(Node& node, int localPinId) {
            int globalPinId = node.inputIds[localPinId];
            std::cout << "Global pin id: " << globalPinId << "\n";
            std::vector<Link> possibleLinks = this->getLinksOfNode(node);

            bool targetLinkFound = false;
            Link targetLink = possibleLinks[0]; // placeholder
            for(Link link : possibleLinks) {
                if (link.end_attr == globalPinId) {
                    targetLink = link;
                    targetLinkFound = true;
                }
            }
            return {targetLink, targetLinkFound};
        }

        bool pinHasLink(int pinId) {
            for (Link& link : this->links) {
                if (link.start_attr == pinId || link.end_attr == pinId) return true;
            }
            return false;
        }

        std::pair<Node&, bool> getNodeThatHasPinById(int id) {
            for (Node& _node : this->nodes) {
                if(VECTOR_CONTAINS(_node.inputIds, id) || VECTOR_CONTAINS(_node.outputIds, id)) {
                    return {_node, true};
                }
            }
            Node placeholderNode = Node(-1, -1);
            return {placeholderNode, false};
        }
};

#endif