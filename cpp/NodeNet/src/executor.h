//
// Created by  KiviCode on 28/07/2020.
//

#ifndef NODENET_EXECUTOR_H
#define NODENET_EXECUTOR_H

#include <regex>
#include <sstream>
#include "data_storages.h"

namespace Executor {

//  std::regex extract_params_regex  (R"([^,]+)"                                             );

    std::vector<std::pair<bool, std::pair<int, int>>> getIOCodePositionsAndLengths(std::string baseCode) { // <bool, <int, int>> ==> <isInput, <character index, substring length>>
        std::regex find_input_call_regex (R"((?:.*)\ *=\ *?input+\((.*)\))" );
        std::regex find_output_call_regex(R"((?:.*)\ *=\ *?output+\((.*)\))");
        std::regex extract_comments      ("(#.*\n?)"                    );

        std::vector<std::pair<bool, std::pair<int, int>>> output = {};

        std::stringstream ss(baseCode);
        std::string line;
        std::smatch IOCallMatch;
        int groupId = 1;
        int linesShift = 0;

        while(std::getline(ss, line, '\n')){ // iterate over lines
            if (std::regex_search(line, IOCallMatch, find_input_call_regex)) { // check for inputs
                output.push_back({true, {linesShift + IOCallMatch.position(groupId), IOCallMatch.length(groupId)}});
            }
            if (std::regex_match(line, IOCallMatch, find_output_call_regex)) { // check for outputs
                output.push_back({false, {linesShift + IOCallMatch.position(groupId), IOCallMatch.length(groupId)}});
            }
            linesShift += line.length();
        }
        return output;
    };

    std::pair<PinLocation, bool> getNextPinLocation(Editor& editor, Node& node, int localPinId) { // returns: {{parent node id, local pin id}, was found?}

        auto linkData = editor.getLinkToPin(node, localPinId);
        std::cout << linkData.first.start_attr << " " << linkData.second << "\n";

        if (linkData.second) { // link was found
            std::cout << 1 << "\n";
            int newPinId = linkData.first.start_attr;
            auto searchResult = editor.getNodeThatHasPinById(newPinId);
            if (searchResult.second) { // it's not an edge node (has incoming links)
                auto newPinSearchResult = searchResult.first.globalPinIdToLocal(newPinId);
                if (newPinSearchResult.second) { // target pin exists
                    return {{searchResult.first.id, newPinSearchResult.first}, true};
                }
            }
        }
        std::cout << "Link not found for: " << node.config.title << "\n";
        return {{-1,-1}, false};
    }



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
