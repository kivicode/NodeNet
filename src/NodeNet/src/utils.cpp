//
// Created by  KiviCode on 27/07/2020.
//


#ifndef NODENET_UTILS_CPP
#define NODENET_UTILS_CPP

#include "utils.h"

#include "settings.h"
#include "data_types.h"
#include <iostream>
#include <fstream>

//template<typename T>
//bool VECTOR_CONTAINS(std::vector<T> vector, T value) {
//#ifdef DEBUG
//    std::cout << "{2}\n";
//#endif
//    bool found = false;
//    for(auto v : vector) {
//        if (v == value) found = true;
//    }
//#ifdef DEBUG
//    std::cout << "[2]\n";
//#endif
//    return found;
//}

//template<typename T1, typename T2>
//bool MAP_HAS_KEY(std::map<T1, T2>map, T1 key) {
//#ifdef DEBUG
//    std::cout << "{1}\n";
//#endif
//    return map.find(key) != map.end();
//}


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

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::trunc);
    file << content;
    file.close();
}

#endif //NODENET_UTILS_CPP
