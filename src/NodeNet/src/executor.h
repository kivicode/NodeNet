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

namespace Executor {

    std::regex find_input_call_regex( R"(input+\()");
    std::regex find_output_call_regex(R"(output+\()");

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