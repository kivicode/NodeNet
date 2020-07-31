//
// Created by  KiviCode on 28/07/2020.
//

#ifndef NODENET_EXECTOR_H
#define NODENET_EXECTOR_H

#include <stdio.h>
#include <regex>
#include <array>
#include <sstream>

namespace Executor {

    std::regex find_input_call_regex(R"((?:.*)\ *=\ *?input+\((.*)\))");
    std::regex find_output_call_regex(R"((?:\ *)output+\((.*)\))");

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

    std::vector<std::pair<bool, std::array<int, 3>>> getIOCodePositionsAndLengths( std::string baseCode) { // <bool, {int, int, int}> ==> <isInput, {line number, character index (inside the line), substring length}>

        std::vector<std::pair<bool, std::array<int, 3>>> output = {};

        for(auto res : Executor::findRegexInCode(baseCode, find_input_call_regex)) {
            output.emplace_back(true, res);
        }

        for(auto res : Executor::findRegexInCode(baseCode, find_output_call_regex)) {
            output.emplace_back(false, res);
        }
        return output;
    }
}

#endif