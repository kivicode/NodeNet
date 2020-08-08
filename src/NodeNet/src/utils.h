//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_UTILS_H
#define NODENET_UTILS_H

#include <iostream>
#include <map>

//template<typename T>
//bool VECTOR_CONTAINS(std::vector<T> vector, T value);
#define VECTOR_CONTAINS(vector, value) std::find(vector.begin(), vector.end(), value) != vector.end()

//template<typename T1, typename T2>
//bool MAP_HAS_KEY(std::map<T1, T2>map, T1 key);
#define MAP_HAS_KEY(map, key) map.find(key) != map.end()

#define _GET_INPUT_ID(node, internalId) node->id + (MAX_PINS * (internalId + 1))
#define _GET_OUTPUT_ID(node, internalId) _GET_INPUT_ID(node, node->inputs.size() + internalId)

#define GET_INPUT_ID(node, internalId) node.id + (MAX_PINS * (internalId + 1))
#define GET_OUTPUT_ID(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

#define PinLocation std::pair<int, int> // {parent node id, local pin id}

#define GLOBAL_PIN_ID_TO_LOCAL(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T  & element);

std::string printPinLocation(PinLocation pinLocation);

std::string generateIndentation(int indentation) {
    std::string indentation_string;
    for(int i = 0; i < indentation; i++) indentation_string.append("\t");
    return indentation_string;
}

std::string readFile(const std::string& path);


#endif //NODENET_UTILS_H
