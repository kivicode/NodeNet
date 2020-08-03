//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_GENERATOR_H
#define NODENET_GENERATOR_H

#include "imnodes.h"
#include "imgui.h"
#include "data_types.h"

namespace NodeGenerator {

    void generateFromConfig(Editor& editor, Node& node, NodeConfig& config);
    void buildTestNode(Node &node, const std::string &path);
    void buildFinishNode(Node &node, const std::string &path);
    void buildStartNode(Node &node, const std::string &path);

}

#endif //NODENET_GENERATOR_H
