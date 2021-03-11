//
// Created by  KiviCode on 07/08/2020.
//

#ifndef NODENET_EXCEPTIONS_H
#define NODENET_EXCEPTIONS_H

#include <exception>
#include <string>
#include <utility>

struct NodeException : public std::exception {

    int nodeId;

public:
    NodeException(int nodeId, std::string  nodeTitle): nodeId(nodeId), nodeTitle(std::move(nodeTitle)) {}

    std::string nodeTitle;
};

#endif //NODENET_EXCEPTIONS_H
