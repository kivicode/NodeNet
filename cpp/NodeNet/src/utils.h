//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_UTILS_H
#define NODENET_UTILS_H

#include <iostream>

#define VECTOR_CONTAINS(vector, value) std::find(vector.begin(), vector.end(), value) != vector.end()
#define MAP_HAS_KEY(map, key) map.find(key) != map.end()

#define GET_INPUT_ID(node, internalId) node.id + (MAX_PINS * (internalId + 1))
#define GET_OUTPUT_ID(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)
#define PinLocation std::pair<int, int> // {parent node id, local pin id}

#define GLOBAL_PIN_ID_TO_LOCAL(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T  & element);

std::string printPinLocation(PinLocation pinLocation);

std::string readFile(const std::string& path);


#endif //NODENET_UTILS_H
