//
// Created by  KiviCode on 27/07/2020.
//


#ifndef NODENET_UTILS_CPP
#define NODENET_UTILS_CPP

#include "utils.h"

#include <iostream>
#include <fstream>


/****  Vectors & Maps  ****/
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





/****  MISC  ****/
std::string printPinLocation(PinLocation pinLocation) {
    std::cout << "{ node=" << pinLocation.first << "\tpin=" << pinLocation.second << "}";
    return "";
}

int* shiftPtr(int *ptr, int shift) {
    int *v;     // we don't know what type of data v will point to
    v =  ptr + shift; // pointer arithmetic
    return v;    // return the resulting memory address
}





/****  Strings  ****/
bool findStringIC(const std::string & strHaystack, const std::string & strNeedle) { //// Ignore case search
    auto it = std::search(
            strHaystack.begin(), strHaystack.end(),
            strNeedle.begin(),   strNeedle.end(),
            [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
    );
    return (it != strHaystack.end() );
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string part;
    while(std::getline(ss, part, delim)) {
        elems.push_back(part);
    }
    return elems;
}

inline size_t countChar(std::string s, char target) {
    return std::count_if(s.begin(), s.end(), [target](char c) { if (c == target) return true; });
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

std::string generateIndentation(int indentation) {
    std::string indentation_string;
    for (int i = 0; i < indentation; i++) indentation_string.append("\t");
    return indentation_string;
}

std::string fnameToNodeName(std::string name) {
    name.at(0) = toupper(name.at(0));
    replaceAll(name, "_", " ");
    replaceAll(name, ".node", "");
    return name;
}





/****  Files  ****/
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

std::vector<tinydir_file> scanDir(const std::string& path, bool ignoreDots) {
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
#endif //NODENET_UTILS_CPP
