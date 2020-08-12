//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_UTILS_H
#define NODENET_UTILS_H

#include <iostream>
#include <map>
#include <vector>
#include "tinydir.h"


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

#define STR(x) std::to_string(x)

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>& vecOfElements, const T  & element);

std::string printPinLocation(PinLocation pinLocation);

std::string generateIndentation(int indentation) {
    std::string indentation_string;
    for(int i = 0; i < indentation; i++) indentation_string.append("\t");
    return indentation_string;
}

std::string readFile(const std::string& path);
void writeFile(const std::string& path, const std::string& content);

size_t countChar(std::string s, char target) {
    return std::count_if( s.begin(), s.end(), [target]( char c ){if(c == target) return true; });
}

std::string trim(const std::string& str) {
    std::string_view s(str);
    s.remove_prefix(std::min(s.find_first_not_of(" \t\r\v\n"), s.size()));
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t\r\v\n") - 1, s.size()));

    return std::string(s);
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::vector<tinydir_file> scanDir(const std::string& path, bool ignoreDots = true) {
    std::vector<tinydir_file> result = {};
    std::string name;
    tinydir_dir dir;
    tinydir_open_sorted(&dir, path.c_str());
    for (int i = 0; i < dir.n_files; i++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);
        name = file.name;
        if (ignoreDots && name.at(0) == '.') continue;
        result.push_back(file);
    }
    tinydir_close(&dir);
    return result;
}

std::string fnameToNodeName(std::string name) {
    name.at(0) = toupper(name.at(0));
    replaceAll(name, "_", " ");
    replaceAll(name, ".node", "");
    return name;
}

int* shiftPtr(int *ptr, int shift) {
    int *v;     // we don't know what type of data v will point to
    v =  ptr + shift; // pointer arithmetic
    return v;    // return the resulting memory address
}

#include <algorithm>
#include <string>
#include <cctype>

bool findStringIC(const std::string & strHaystack, const std::string & strNeedle) { //// Ignore case search
    auto it = std::search(
            strHaystack.begin(), strHaystack.end(),
            strNeedle.begin(),   strNeedle.end(),
            [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (it != strHaystack.end() );
}

#endif //NODENET_UTILS_H
