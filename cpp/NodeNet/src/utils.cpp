//
// Created by  KiviCode on 27/07/2020.
//


#ifndef NODENET_UTILS_CPP
#define NODENET_UTILS_CPP

#include "settings.h"
#include <iostream>
#include <fstream>

#define VECTOR_CONTAINS(vector, value) std::find(vector.begin(), vector.end(), value) != vector.end()
#define MAP_HAS_KEY(map, key) map.find(key) != map.end()

#define GET_INPUT_ID(node, internalId) node.id + (MAX_PINS * (internalId + 1))
#define GET_OUTPUT_ID(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)
#define PinLocation std::pair<int, int> // {parent node id, local pin id}

#define GLOBAL_PIN_ID_TO_LOCAL(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element) {
    std::pair<bool, int > result;
    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);
    if (it != vecOfElements.end()) {
        result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    } else {
        result.first = false;
        result.second = -1;
    }
    return result;
}

std::string printPinLocation(PinLocation pinLocation) {
    std::cout << "{ node=" << pinLocation.first << "\tpin=" << pinLocation.second << "}";
    return "";
}

std::string readFile(const std::string& path) {
    std::string output, line;
    std::ifstream file(path);

    while (getline(file, line)) {
        output += line + "\n";
    }

    file.close();
    return output;
}

#endif //NODENET_UTILS_CPP
