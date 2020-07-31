//
// Created by  KiviCode on 26/07/2020.
//

#pragma once

#include "data_types.h"

#include <iostream>
#include <utility>
#include <vector>
#include <string>

#include "utils.cpp"
#include "executor.h"
#include <array>


/* ========== NODE IO PIN ========== */
NodeIOPin::NodeIOPin(std::string name, bool isEditable)                                                                                     : name(std::move(name)), isEditable(isEditable) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, SliderDataType dataType)                                                            : name(std::move(name)), isEditable(isEditable), dataType(dataType) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, float minSliderVal, float maxSliderVal, float sliderSpeed) : name(std::move(name)), isEditable(isEditable), dataType(dataType), minSliderVal(minSliderVal), maxSliderVal(maxSliderVal), sliderSpeed(sliderSpeed) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, float minSliderVal, float maxSliderVal, float sliderSpeed)                          : name(std::move(name)), isEditable(isEditable), minSliderVal(minSliderVal), maxSliderVal(maxSliderVal), sliderSpeed(sliderSpeed) {}




/* ========== NODE CONFIG ========== */

NodeConfig::NodeConfig(std::string title, std::vector<NodeIOPin> inputs, std::vector<NodeIOPin> outputs) : title(std::move(title)), inputs(std::move(inputs)), outputs(std::move(outputs)) {}

void NodeConfig::setCode(std::string newCode) {
    this->baseCode = std::move(newCode);
}




/* ========== NODE ========== */

Node::Node(const int i, const float v) : id(i), value(v) {}

std::pair<int, bool> Node::globalPinIdToLocal(int globalId) const {
    auto searchOverInputs  = findInVector<int>(this->inputIds,  globalId);
    auto searchOverOutputs = findInVector<int>(this->outputIds, globalId);
    if (searchOverInputs.second) return  searchOverInputs;
    if (searchOverOutputs.second) return searchOverOutputs;
    return {-1, false};
}

void Node::setBaseCode(std::string code) {
    this->baseCode = std::move(code);
    this->processedCode = this->baseCode;
}

void Node::setProcessedCode(std::string code) {
    this->processedCode = std::move(code);
}

void Node::setConfig(NodeConfig newConfig) {
    if (this->_type == PrivatePinType::CUSTOM) {
        NodeIOPin previousVarnamePin("Connected to", true, SliderDataType::CODE);
        NodeIOPin varnamePin("Name", true, SliderDataType::STRING);
        newConfig.inputs.insert(newConfig.inputs.begin(), previousVarnamePin);
        newConfig.inputs.insert(newConfig.inputs.begin(), varnamePin);
    }
    this->setBaseCode(newConfig.baseCode);
    this->config = std::move(newConfig);
}

void Node::replaceInputsWithValues(std::vector<std::pair<bool, std::array<int, 3>>>& ioCodePositions) {
    this->processedCode = "";
    for (std::pair<bool, std::array<int, 3>> io : ioCodePositions) {
        bool isInput = io.first;

        int lineNo = io.second[0];
        int start  = io.second[1];
        int length = io.second[2];

        std::cout << lineNo << " " << start << " " << length << "\n";

        std::stringstream ss(this->baseCode);
        std::string line;
        int linesNumber = 0;

        while (std::getline(ss, line, '\n')) { // iterate over lines
            if (linesNumber == lineNo and isInput) {
                int funcNameSize = std::string("input(").length();
                line.replace(start - funcNameSize, length + funcNameSize + 1, "1");
            }
            this->processedCode += line + "\n\r";
            std::cout << line;
            linesNumber++;
        }
    }
}

void Node::generateProcessedCode(const std::vector<Link>& connections) {
    if (this->_type == PrivatePinType::CUSTOM) {

        auto ioCodePositions = Executor::getIOCodePositionsAndLengths(this->baseCode);
        this->replaceInputsWithValues(ioCodePositions);

        // Add reference to the previous node varname
        std::cout << "Code: \"\n" << this->processedCode << "\"\n";

    }

}






/* ========== LINK ========== */

void Link::sort(std::vector<Node> nodes) {
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
Link::Link() : id(-1), start_attr(-1), end_attr(-1) {}
Link::Link(int i, int start, int end) : id(i), start_attr(start), end_attr(end) {}





/* ========== EDITOR ========== */

std::vector<Link> Editor::getLinksOfNode(Node &node) {
    std::vector<Link> result = {};
    for (Link &link : this->links) {
        bool startIsInput = VECTOR_CONTAINS(node.inputIds, link.start_attr);
        bool endIsInput = VECTOR_CONTAINS(node.inputIds, link.end_attr);

        bool startIsOutput = VECTOR_CONTAINS(node.outputIds, link.start_attr);
        bool endIsOutput = VECTOR_CONTAINS(node.outputIds, link.end_attr);

        if (startIsInput || endIsInput || startIsOutput || endIsOutput) {
            result.push_back(link);
        }
    }
    return result;
}

std::pair<Link, bool> Editor::getLinkToPin(Node &node, int localPinId) {
    int globalPinId = node.inputIds[localPinId];
    std::cout << "Global pin id: " << globalPinId << "\n";
    std::vector<Link> possibleLinks = this->getLinksOfNode(node);

    bool targetLinkFound = false;
    Link targetLink = possibleLinks[0]; // placeholder
    for (Link link : possibleLinks) {
        if (link.end_attr == globalPinId) {
            targetLink = link;
            targetLinkFound = true;
        }
    }
    return {targetLink, targetLinkFound};
}

bool Editor::pinHasLink(int pinId) {
    for (Link &link : this->links) {
        if (link.start_attr == pinId || link.end_attr == pinId) return true;
    }
    return false;
}

std::pair<Node &, bool> Editor::getNodeThatHasPinById(int id) {
    for (Node &_node : this->nodes) {
        if (VECTOR_CONTAINS(_node.inputIds, id) || VECTOR_CONTAINS(_node.outputIds, id)) {
            return {_node, true};
        }
    }
    Node placeholderNode = Node(-1, -1);
    return {placeholderNode, false};
}

std::pair<PinLocation, bool> Editor::getNextPinLocation(Node &node, int localPinId) { // returns: {{parent node id, local pin id}, was found?}

    auto linkData = this->getLinkToPin(node, localPinId);
    std::cout << linkData.first.start_attr << " " << linkData.second << "\n";

    if (linkData.second) { // link was found
        std::cout << 1 << "\n";
        int newPinId = linkData.first.start_attr;
        auto searchResult = this->getNodeThatHasPinById(newPinId);
        if (searchResult.second) { // it's not an edge node (has incoming links)
            auto newPinSearchResult = searchResult.first.globalPinIdToLocal(newPinId);
            if (newPinSearchResult.second) { // target pin exists
                return {{searchResult.first.id, newPinSearchResult.first}, true};
            }
        }
    }
    std::cout << "Link not found for: " << node.config.title << "\n";
    return {{-1, -1}, false};
}
