//
// Created by  KiviCode on 28/07/2020.
//

#ifndef NODENET_EXECUTOR_H
#define NODENET_EXECUTOR_H

#include <regex>
#include <array>
#include <sstream>
#include "data_types.cpp"

namespace Executor {

    std::vector<std::pair<bool, std::array<int, 3>>> getIOCodePositionsAndLengths(const std::string& baseCode) { // <bool, {int, int, int}> ==> <isInput, {line number, character index (inside the line), substring length}>
        std::regex find_input_call_regex (R"((?:.*)\ *=\ *?input+\((.*)\))" );
        std::regex find_output_call_regex(R"((?:\ *)output+\((.*)\))");
        std::regex extract_comments      ("(#.*\n?)"                    );

        std::vector<std::pair<bool, std::array<int, 3>>> output = {};

        std::stringstream ss(baseCode);
        std::string line;
        std::smatch IOCallMatch;
        int groupId = 1;
        int linesNumber = 0;

        while(std::getline(ss, line, '\n')){ // iterate over lines
            if (std::regex_search(line, IOCallMatch, find_input_call_regex)) { // check for inputs
                auto data = std::array<int, 3>{linesNumber++,static_cast<int>(IOCallMatch.position(groupId)), static_cast<int>(IOCallMatch.length(groupId))};
                output.emplace_back(true, data);
            }
            if (std::regex_match(line, IOCallMatch, find_output_call_regex)) { // check for outputs
                auto data = std::array<int, 3>{linesNumber++,static_cast<int>(IOCallMatch.position(groupId)), static_cast<int>(IOCallMatch.length(groupId))};
                output.emplace_back(false, data);
            }
        }
        return output;
    };



    /*
    std::vector<PinLocation> getSequenceOfPinsForInputValue(Editor& editor, PinLocation startPinLocation) {
        std::vector<PinLocation> sequence = {};

        auto initialSearchResult = Executor::getNextPinLocation(editor, editor.nodes[startPinLocation.first], startPinLocation.second);
        if (initialSearchResult.second) { // there is a node that's been connected to the current finish node
            bool isEndNodes;
            std::pair<int, int> newPinLocationToCheck = initialSearchResult.first;

            do { //  literally first time I use do/while instead of while

                auto searchResult = Executor::getNextPinLocation(editor,
                                                                 editor.nodes[newPinLocationToCheck.first],
                                                                 newPinLocationToCheck.second);

                isEndNodes = !searchResult.second; // check if next node was found

                if(!isEndNodes) {
                    sequence.insert(sequence.begin(), searchResult.first); // push to the beginning
                    newPinLocationToCheck = std::move(searchResult.first);
                }
            } while (!isEndNodes);
        }
        return sequence;
    }*/
}

#endif //NODENET_EXECUTOR_H