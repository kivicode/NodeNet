//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_UTILS_H
#define NODENET_UTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <sstream>

#include "tinydir.h"



#define VECTOR_CONTAINS(vector, value) std::find(vector.begin(), vector.end(), value) != vector.end()
#define MAP_HAS_KEY(map, key) map.find(key) != map.end()

#define _GET_INPUT_ID(node, internalId) node->id + (MAX_PINS * (internalId + 1))
#define _GET_OUTPUT_ID(node, internalId) _GET_INPUT_ID(node, node->inputs.size() + internalId)

#define GET_INPUT_ID(node, internalId) node.id + (MAX_PINS * (internalId + 1))
#define GET_OUTPUT_ID(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

#define PinLocation std::pair<int, int> // {parent node id, local pin id}

#define GLOBAL_PIN_ID_TO_LOCAL(node, internalId) GET_INPUT_ID(node, node.inputs.size() + internalId)

#define STR(x) std::to_string(x)

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T  & element);

std::string generateIndentation(int indentation);

std::string readFile(const std::string& path);
void writeFile(const std::string& path, const std::string& content);

inline size_t countChar(std::string s, char target);

std::string trim(const std::string& str);

void replaceAll(std::string& str, const std::string& from, const std::string& to);

std::vector<tinydir_file> scanDir(const std::string& path, bool ignoreDots = true);

std::string fnameToNodeName(std::string name);

int* shiftPtr(int *ptr, int shift);

bool findStringIC(const std::string & strHaystack, const std::string & strNeedle); //// Ignore case search

std::vector<std::string> split(const std::string &s, char delim);

#endif //NODENET_UTILS_H
