//
// Created by  KiviCode on 26/07/2020.
//

#pragma once

#include "data_types.h"

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>

#include "utils.cpp"
#include "executor.h"
#include "exceptions.h"
#include "tinyfiledialogs.h"


/* ========== NODE IO PIN ========== */
NodeIOPin::NodeIOPin(std::string name, bool isEditable)                                                                                     : name(std::move(name)), isEditable(isEditable) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, SliderDataType dataType)                                                            : name(std::move(name)), isEditable(isEditable), dataType(dataType) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, std::vector<std::string> options)                          : name(std::move(name)), isEditable(isEditable), dataType(dataType), options(std::move(options)) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, SliderDataType dataType, float minSliderVal, float maxSliderVal, float sliderSpeed) : name(std::move(name)), isEditable(isEditable), dataType(dataType), minSliderVal(minSliderVal), maxSliderVal(maxSliderVal), sliderSpeed(sliderSpeed) {}
NodeIOPin::NodeIOPin(std::string name, bool isEditable, float minSliderVal, float maxSliderVal, float sliderSpeed)                          : name(std::move(name)), isEditable(isEditable), minSliderVal(minSliderVal), maxSliderVal(maxSliderVal), sliderSpeed(sliderSpeed) {}




/* ========== NODE CONFIG ========== */

NodeConfig::NodeConfig(std::string title, std::vector<NodeIOPin> inputs, std::vector<NodeIOPin> outputs) : title(std::move(title)), inputs(std::move(inputs)), outputs(std::move(outputs)) {}

void NodeConfig::setCode(std::string newCode) {
    this->baseCode = std::move(newCode);
}




/* ========== NODE ========== */

Node::Node(const int i, const float v) : id(i), value(v) {}

void Node::init() {
    for (int ii = 0; ii < config.inputs.size(); ii++) { //// init start input values
        int pinId = this->id + (MAX_PINS * (ii + 1));
        bool hasVal = MAP_HAS_KEY(this->inputs, pinId);

        if (!hasVal) {
            this->inputs[pinId].empty();
            this->inputIds.push_back(pinId);
            if(config.inputs[ii].dataType == STRING_SELECTOR) {
                this->config.inputs[ii].selectedOption = 0;
                this->inputs[pinId].setString(this->config.inputs[ii].options[0]);
            }
#ifdef DEBUG
            std::cout << "Create input for node: " << config.title.c_str() << "  Input name: " << config.inputs[ii].name << " id: " << pinId << "\n";
#endif
        }
    }

    for (int oi = 0; oi < config.outputs.size(); oi++) { //// init out start values
        int pinId = _GET_OUTPUT_ID(this, oi);
        bool hasVal = MAP_HAS_KEY(this->outputs, pinId);
        if (!hasVal) {
            this->outputs[pinId].empty();
            this->outputIds.push_back(pinId);
#ifdef DEBUG
            std::cout << "Create output for node: " << config.title.c_str() << "  Output name: " << config.outputs[oi].name << " id: " << pinId << "\n";
#endif
        }
    }
}

std::pair<int, bool> Node::globalPinIdToLocal(int globalId) const {
    auto searchOverInputs  = findInVector<int>(this->inputIds,  globalId);
    auto searchOverOutputs = findInVector<int>(this->outputIds, globalId);
    if (searchOverInputs.second) return  searchOverInputs;
    if (searchOverOutputs.second) return searchOverOutputs;
    return {-1, false};
}

int Node::inputIdByName(std::string& name) {
    int i = 0;
    for(const NodeIOPin& pin : this->config.inputs) {
        if (pin.name == name) return i;
        i++;
    }
    return -1;
}

std::any Node::getInputValueById(Editor &editor, int localIndex) {
    if (this->config.inputs[localIndex].name == LINK_PIN_TEXT) {
        auto val = this->getPrevVarname(editor, editor.getLinkToPin(editor.nodes.at(this->id - 1), localIndex));
#ifdef DEBUG
        std::cout << "Found staff pin: " << this->config.inputs[localIndex].name << " With val: " << val << " ID: " << this->inputIds[localIndex] << "\n";
#endif

        // convert to char array
        char charVal[INPUT_BUFF_SIZE];
        strncpy(charVal, val.c_str(), sizeof(charVal));
        charVal[sizeof(charVal) - 1] = 0;
        return charVal;
    }
    auto pinType = this->config.inputs[localIndex].dataType;
    localIndex = this->inputIds[localIndex];

    switch(pinType) {

        case INTEGER: {
            return this->inputs[localIndex].i;
        }

        case FLOAT: {
            return this->inputs[localIndex].f;
        }
        case STRING: {
            return this->inputs[localIndex].s;
        }
        case STRING_SELECTOR: {
            return this->inputs[localIndex].s;
        }

        default:
            break;
    }
    return 0;
}

std::string Node::getPrevVarname(Editor &editor, std::pair<Link, bool> link) {
    bool hasInputConnection = false; //// staff pins only

    if (link.second) {
        Node node = editor.getNodeThatHasPinById(link.first.start_attr).first;
        std::cout << "POP:: " << node.inputIds[0] << " | " << node.inputIds[1] << "\n";
        return node.inputs[node.inputIds[node._type == START ? 0 : 1]].s;
    }
    return std::string("<error: not connected anywhere>");
}

void Node::setBaseCode(std::string code) {
    this->baseCode = std::move(code);
    this->processedCode = this->baseCode;
}

void Node::setProcessedCode(std::string code) {
    this->processedCode = std::move(code);
}

void Node::setConfig(NodeConfig newConfig) {
    if (this->_type == PrivatePinType::CUSTOM || this->_type == PrivatePinType::FINISH) {

        NodeIOPin previousVarnamePin(LINK_PIN_TEXT, true, SliderDataType::CODE);
        NodeIOPin varnamePin(NAME_PIN_TEXT, true, SliderDataType::STRING);

        varnamePin.isStaff = true;
        previousVarnamePin.isStaff = true;

        newConfig.inputs.insert(newConfig.inputs.begin(), varnamePin);
        newConfig.inputs.insert(newConfig.inputs.begin(), previousVarnamePin);
    }
    this->setBaseCode(newConfig.baseCode);
    this->config = std::move(newConfig);
}

void Node::replaceInputsWithValues(Editor& editor) {

    this->processedCode = "";

    const std::regex regex(R"(input\((.*)\))");
    std::stringstream ss(this->baseCode);
    std::string line;
    std::smatch sm;

    while (std::getline(ss, line, '\n')) { // iterate over lines
        int n = 0;
        while (regex_search(line, sm, regex)) {

            int L = (int)std::string("input(\"").size() - 1; // offset shift size in order to replace the whole statement
            auto match = sm[1];
            int pos = sm.position(1);
            int endPos = CodeExecutor::getClosingBracketPos(line, pos, false);

            //// input("just a name") = ...
            ////       ^           ^
            ////     pos+1     nameEndPos
            std::string name = line.substr(pos + 1, endPos - 1);
            int nameEndPos = CodeExecutor::firstCharMatch(name, ')') - 1;
            name = name.substr(0, nameEndPos);
            std::any val = this->getInputValueById(editor, this->inputIdByName(name));


            std::string stringVal; // important for further replacement

            // select appropriate type
            if (val.type() == typeid(char*)) {
                stringVal = std::string((std::any_cast<char *>(val)));
            } else {
                if (val.type() == typeid(float))    stringVal = std::to_string(std::any_cast<float>(val));
                else if (val.type() == typeid(int)) stringVal = std::to_string(std::any_cast<int>(val));
            }

            if(name == "Link") std::cout << this->inputIdByName(name) << "    " << stringVal << "\n";

            // replace input statement with it's actual value
            line.erase(pos - L, endPos - (pos - L) + 2);
            line.insert(pos - L, stringVal);

            if(++n == MAX_INLINE_PIN_DECLARATIONS) break; // infinite while fuse
        }
        if (line != "" && line != "\n") this->processedCode += line + "\n"; // avid empty lines
        else this->processedCode += line;
    }
}

void Node::deleteOutputsFromCode() {
    std::string tmpCode;
    const std::regex regex(R"(output\((.*)\))");
    std::stringstream ss(this->processedCode);
    std::string line;
    std::smatch sm;

    while (std::getline(ss, line, '\n')) { // iterate over lines
        int n = 0;
        while (regex_search(line, sm, regex)) {
            int L = (int)std::string("output(\"").size() - 1; // offset shift size in order to replace the whole statement
            auto match = sm[1];
            int pos = sm.position(1);
            int endPos = CodeExecutor::getClosingBracketPos(line, pos, false);

            // replace input statement with it's actual value
            line.erase(pos - L, endPos - (pos - L) + 2);

            if(++n == MAX_INLINE_PIN_DECLARATIONS) break; // infinite while fuse
        }
        if (line != "" && line != "\n") tmpCode += line + "\n";
        else tmpCode += line;
    }
    this->processedCode = tmpCode;
}

std::string Node::getInputName(Editor &editor) {
    std::string res;

    int n = 0;
    while(true) {

        auto linkSearch = editor.getLinkToPin(editor.nodes.at(this->id-1), 0);
        if (linkSearch.second) {
            auto nodeSearch = editor.getNodeThatHasPinById(linkSearch.first.start_attr);
            if (nodeSearch.second) {
                if(nodeSearch.first._type == START) return nodeSearch.first.inputs[nodeSearch.first.inputIds[0]].s;
                else return nodeSearch.first.getInputName(editor);
            }
        }
        if(++n == MAX_SEQUENCE_LENGTH) break; // infinite while fuse
    }
    return "";
}

bool Node::hasName(Editor& editor) {
    int nameId;
    if (this->_type == START) {
        nameId = this->inputIds[0];
    } else {
//        std::cout << this->_type << " -===- " << FINISH << "\n";
        nameId = this->inputIds[1];
    }
    return !std::string(this->inputs[nameId].s).empty() || editor.pinHasLink(nameId);
}

bool Node::hasLink(Editor& editor) {
    if (this->_type == START) {
        return true;
    }
    return editor.pinHasLink(this->inputIds[0]);
}

NodeException Node::checkExceptions(Editor &editor) {
    if(!this->hasName(editor)) {
        this->mark = imnodes::NodeMarks::Red;
        this->markDescription = "Unnamed";
        return NodeException(this->id, this->markDescription);
    }
    if(!this->hasLink(editor)) {
        this->mark = imnodes::NodeMarks::Orange;
        this->markDescription = "Not linked";
        return NodeException(this->id, this->markDescription);
    }
    this->mark = imnodes::NodeMarks::None;
    this->markDescription = "";
    return NodeException(-1, ""); // placeholder
}

std::string Node::generateProcessedCode(Editor &editor, int indentation) {
    NodeException exc = this->checkExceptions(editor);
    if (exc.nodeId != -1) throw exc; // check for 'placeholder' meanless exception

    this->replaceInputsWithValues(editor);
    this->deleteOutputsFromCode();

    std::string indentation_string = std::move(generateIndentation(indentation));

    if (this->_type == START) this->processedCode.insert(0, indentation_string);

    auto linkSearch = editor.getLinkToPin(editor.nodes.at(this->id-1), 0);
    if (linkSearch.second) {
        auto nodeSearch = editor.getNodeThatHasPinById(linkSearch.first.start_attr);
        if (nodeSearch.second) {
            this->processedCode.insert(0, nodeSearch.first.generateProcessedCode(editor, indentation) + indentation_string);
        }
    }

    if (this->_type == FINISH) { // replace {{INPUT_NAME}} with the actual input name
        this->processedCode = std::regex_replace(this->processedCode, std::regex(R"(\{\{INPUT_NAME\}\})"), this->getInputName(editor));
    }

    return this->processedCode;
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

std::vector<Link> Editor::getLinksToNode(Node &node) {
    std::vector<Link> result = {};
    for (Link &link : this->links) {
        bool endIsInput = VECTOR_CONTAINS(node.inputIds, link.end_attr);

        if (endIsInput) {
            result.push_back(link);
        }
    }
    return result;
}

std::vector<Link> Editor::getLinksFromNode(Node &node) {
    std::vector<Link> result = {};
    for (Link &link : this->links) {
        bool startIsOutput = VECTOR_CONTAINS(node.outputIds, link.start_attr);

        if (startIsOutput) {
            result.push_back(link);
        }
    }
    return result;
}

std::vector<Link> Editor::getLinksOfNode(Node &node) {
    std::vector<Link> result = {};
    for (auto link : this->links) {

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

std::pair<Node&, bool> Editor::getNodeThatHasPinById(int id) {
    for (Node& _node : this->nodes) {
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
        int newPinId = linkData.first.start_attr;
        auto searchResult = this->getNodeThatHasPinById(newPinId);
        if (searchResult.second) { // it's not an edge node (has incoming links)
            auto newPinSearchResult = searchResult.first.globalPinIdToLocal(newPinId);
            if (newPinSearchResult.second) { // target pin exists
                return {{searchResult.first.id, newPinSearchResult.first}, true};
            }
        }
    }
#ifdef DEBUG
    std::cout << "Link not found for: " << node.config.title << "\n";
#endif
    return {{-1, -1}, false};
}

bool Editor::pinHasLink(int pinId) {
    for (Link &link : this->links) {
        if (link.start_attr == pinId || link.end_attr == pinId) return true;
    }
    return false;
}
