//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_EXECTOR_CPP
#define NODENET_EXECTOR_CPP

#include "executor.h"
#include "src/Core/data_types.h"

#include <regex>
#include <array>
#include <sstream>


//std::vector<std::pair<bool, std::array<int, 3>>> CodeExecutor::getIOCodePositionsAndLengths( std::string baseCode) { // <bool, {int, int, int}> ==> <isInput, {line number, character index (inside the line), substring length}>
//    std::regex find_input_call_regex(R"((?:.*)\ *=\ *?input+\((.*)\))");
//    std::regex find_output_call_regex(R"((?:\ *)output+\((.*)\))");
//
//    std::vector<std::pair<bool, std::array<int, 3>>> output = {};
//
//    std::stringstream ss(baseCode);
//    std::string line;
//    std::smatch IOCallMatch;
//    int groupId = 1;
//    int linesNumber = 0;
//
//    while (std::getline(ss, line, '\n')) { // iterate over lines
//        if (std::regex_search(line, IOCallMatch, find_input_call_regex)) { // check for inputs
//            auto data = std::array<int, 3>{linesNumber++, static_cast<int>(IOCallMatch.position(groupId)),
//                                           static_cast<int>(IOCallMatch.length(groupId))};
//            output.emplace_back(true, data);
//        }
//        if (std::regex_match(line, IOCallMatch, find_output_call_regex)) { // check for outputs
//            auto data = std::array<int, 3>{linesNumber++, static_cast<int>(IOCallMatch.position(groupId)),
//                                           static_cast<int>(IOCallMatch.length(groupId))};
//            output.emplace_back(false, data);
//        }
//    }
//    return output;
//}


/*
    std::vector<PinLocation> getSequenceOfPinsForInputValue(Editor &editor, PinLocation startPinLocation) {
        std::vector<PinLocation > sequence = {};

        auto initialSearchResult = CodeExecutor::getNextPinLocation(editor, editor.nodes[startPinLocation.first],
                                                                startPinLocation.second);
        if (initialSearchResult.second) { // there is a node that's been connected to the current finish node
            bool isEndNodes;
            std::pair<int, int> newPinLocationToCheck = initialSearchResult.first;

            do { //  literally first time I use do/while instead of while

                auto searchResult = CodeExecutor::getNextPinLocation(editor,
                                                                 editor.nodes[newPinLocationToCheck.first],
                                                                 newPinLocationToCheck.second);

                isEndNodes = !searchResult.second; // check if next node was found

                if (!isEndNodes) {
                    sequence.insert(sequence.begin(), searchResult.first); // push to the beginning
                    newPinLocationToCheck = std::move(searchResult.first);
                }
            } while (!isEndNodes);
        }
        return sequence;
    }
*/

#endif