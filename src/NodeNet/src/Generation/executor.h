//
// Created by  KiviCode on 28/07/2020.
//

#ifndef NODENET_EXECTOR_H
#define NODENET_EXECTOR_H

#include <cstdio>
#include <regex>
#include <array>
#include <sstream>
#include <iostream>
#include "src/Core/data_types.h"

namespace CodeExecutor {

    std::regex find_input_call_regex( R"(input+\()");
    std::regex find_output_call_regex(R"(output+\()");
    std::regex find_declaration_segment_regex(R"([^<%][\s\S]*%>)");

    std::vector<std::array<int, 3>> findRegexInCode(std::string baseCode, std::regex regex) { // {line number, character index (inside the line), substring length}
        std::vector<std::array<int, 3>> output = {};

        std::stringstream ss(baseCode);
        std::string line;
        std::smatch CallMatch;
        int groupId = 1;
        int linesNumber = 0;

        while (std::getline(ss, line, '\n')) { // iterate over lines
            if (std::regex_search(line, CallMatch, regex)) { // check for inputs
                auto data = std::array<int, 3>{linesNumber++, static_cast<int>(CallMatch.position(groupId)),
                                               static_cast<int>(CallMatch.length(groupId))};
                output.emplace_back(data);
            }
        }
        return output;
    }

    int getClosingBracketPos(std::string& line, int startPos, bool openBracketIncluded) {
        if (!openBracketIncluded) startPos++;
        int shift = 0;
        int internalBracketsBuff = 0;

        for (char ch : line.substr(startPos, line.size())) {
            if (ch == ')' && internalBracketsBuff == 0) break;
            if (ch == '(') internalBracketsBuff++;
            shift++;
        }

        return startPos + shift - 1; // -1 in order not to include the closing bracket
    }

    int firstCharMatch(std::string& str, char ch) {
        for(int i = 0; i < str.size(); i++) {
            if (str.at(i) == ch) return i;
        }
        return -1;
    }

    std::vector<std::pair<bool, std::array<int, 3>>> getIOCodePositionsAndLengths( const std::string& baseCode) { // <bool, {int, int, int}> ==> <isInput, {line number, character index (inside the line), substring length}>

        std::vector<std::pair<bool, std::array<int, 3>>> output = {};

        for(auto res : CodeExecutor::findRegexInCode(baseCode, find_input_call_regex)) {
            output.emplace_back(true, res);
        }

        for(auto res : CodeExecutor::findRegexInCode(baseCode, find_output_call_regex)) {
            output.emplace_back(false, res);
        }
        return output;
    }

    std::string getDeclarationSegment(const std::string& code) {
        std::stringstream ss(code);
        std::string block, line;
        std::smatch sm;
        regex_search(code, sm, find_declaration_segment_regex);
        return sm.str().substr(1, sm.str().length()-1-2);
    }

    CodeIODeclaration getInCodeIODeclarations(const std::string& code, const std::string& targetRegex, const std::string& realToken) {
        CodeIODeclaration result(code);

        const std::regex regex(targetRegex);
        std::stringstream ss(code);
        std::string line;
        std::smatch sm;
        int lineCounter = 0;

        while (std::getline(ss, line, '\n')) { // iterate over lines
            int n = 0;
            while (regex_search(line, sm, regex)) {

                int L = (int)std::string(realToken).size() - 1; // offset shift size in order to replace the whole statement
                auto match = sm[1];
                int pos = sm.position(1);
                int endPos = CodeExecutor::getClosingBracketPos(line, pos, false);

                //// input("just a name") = ...
                ////       ^           ^
                ////     pos+1     nameEndPos
                std::string name = line.substr(pos + 1, endPos - 1);
                int nameEndPos = CodeExecutor::firstCharMatch(name, ')') - 1;
                name = name.substr(0, nameEndPos);

                result.add(lineCounter, pos, endPos, name);

                // replace input statement with it's actual value
                line.erase(pos - L, endPos - (pos - L) + 2);

                if(++n == MAX_INLINE_PIN_DECLARATIONS) break; // infinite while fuse
            }
            lineCounter++;
        }
        return result;
    }

    NodeIOPin processSingleDeclarationLine(std::string line) { // pin, found
        if (countChar(line, ' ') == 0)  line = line.append(" ");

        std::stringstream ss(line);
        auto paramList = split(line, ' ');

        NodeIOPin result;

        result.isInput = paramList.at(0) == "input";
        paramList.erase(paramList.begin());

        bool trigDefaultOption = false;
        bool lock = false;
        std::string default_option;


        for (std::string part : paramList) {
            if (part.empty()) continue;
            if (part.at(0) == '@') continue;
            auto deconstructed = split(part, '=');
            if (deconstructed.size() < 2) continue;
            std::string param = deconstructed.at(0), val = deconstructed.at(1);
            if (param == "name") result.name = val;
            if (param == "type") {
                if      (val == "int")    result.dataType = SliderDataType::INTEGER;
                else if (val == "float")  result.dataType = SliderDataType::FLOAT;
                else if (val == "string") result.dataType = SliderDataType::STRING;

                else if (val == "int_drag")    result.dataType = SliderDataType::INTEGER_DRAG;
                else if (val == "float_drag")  result.dataType = SliderDataType::FLOAT_DRAG;
                else if (val == "string_list") result.dataType = SliderDataType::STRING_SELECTOR;
            }
            if (param == "min_val")    result.minSliderVal = std::strtof(val.c_str(), nullptr);
            if (param == "max_val")    result.maxSliderVal = std::strtof(val.c_str(), nullptr);
            if (param == "drag_speed") result.sliderSpeed  = std::strtof(val.c_str(), nullptr);

            if (param == "lock" && val == "true") {lock = true;}
            else if (param == "lock")             {lock = false;}

            if (param == "options") {
                val = val.substr(1, val.size()-2);
                for (std::string& option : split(val, ',')) {
                    result.options.push_back(option);
                }
            }
            if (param == "default_option") {
                default_option = val;
                trigDefaultOption = true;
            }
        }

        if (trigDefaultOption) {
            for(int i = 0; i < result.options.size(); i++) {
                if (result.options.at(i) == default_option) {
                    result.selectedOption = i;
                    break;
                }
            }
        }
        result.isEditable = !lock;
        return result;
    }

    std::array<std::vector<NodeIOPin>, 2> getPinsFromDeclarationSegment(const std::string& code) {
        std::vector<NodeIOPin> inputs, outputs;
        for(const std::string& line : split(code, '\n')) {
            if (line.empty() || line == "\n" || line == "\t" || line == " ") continue;
            NodeIOPin pin = processSingleDeclarationLine(line);
            if (pin.isInput) { inputs.push_back(pin); }
            else { outputs.push_back(pin); }
        }
        return {inputs, outputs};
    }

    NodeConfig configFromFile(const std::string& path) {
        NodeConfig result;
        std::string fileContent = readFile(path);
        std::string declarationSegment;

        try {
            declarationSegment = getDeclarationSegment(fileContent);
        } catch (std::out_of_range& err) {
            return result;
        }

        auto lines = split(declarationSegment, '\n');
        if (lines.empty()) return NodeConfig();

        std::string nodeTitle = lines.at(0);
        declarationSegment = declarationSegment.substr(nodeTitle.length(), declarationSegment.length()+1); // remove title statement from the segment

        for (auto line : split(declarationSegment, '\n')) {
            if(line.empty()) continue;

            if (line.at(0) == '@') {
                if (line.substr(1, line.size()) == "start_node")  result.type = START;
                if (line.substr(1, line.size()) == "finish_node") result.type = FINISH;
                if (line.substr(1, 5) == "color") {
                    std::string decl = split(line.substr(1, line.size()), '(').at(1);
                    decl = split(decl, ')').at(0);
                    replaceAll(decl, " ", "");
                    replaceAll(decl, "\t", "");
                    auto colorComponents = split(decl, ',');
                    std::cout << colorComponents.size() << "\n";
                    if (colorComponents.size() != 3) continue; // only 3-color rgb format accepted
                    for(int i = 0; i < 3; i++) {
                        result.headColor[i] = std::strtof(colorComponents.at(i).c_str(), nullptr) / 255.f;
                    }
                    std::cout << colorComponents[0] << ", " << colorComponents[1] << ", " << colorComponents[2] << "\n";
                }
                std::cout << declarationSegment << "\n";
                replaceAll(declarationSegment, line, "");
            }
        }

        auto pins = getPinsFromDeclarationSegment(declarationSegment);
        result.inputs  = pins.at(0);
        result.outputs = pins.at(1);
        result.title = nodeTitle;
        result.setCode(trim(fileContent.substr(fileContent.rfind("%>")+2, fileContent.size())));
        return result;
    }
}

#endif