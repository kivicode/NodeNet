//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_TEMPLATES_H
#define NODENET_TEMPLATES_H

#include "data_types.cpp"

std::vector<NodeIOPin> testInputs =  {NodeIOPin("Units", true, INTEGER, 0, 1000, 1)};
std::vector<NodeIOPin> testOutputs = {NodeIOPin("output", false, CODE)};
NodeConfig test_config = NodeConfig("Dense", testInputs, testOutputs);

std::vector<NodeIOPin> finish_inputs =  {};
NodeConfig finish_config = NodeConfig("Finish", finish_inputs, std::vector<NodeIOPin>{});

std::vector<NodeIOPin> start_inputs =  {NodeIOPin("Name", true, STRING), NodeIOPin("Size", true, INTEGER)};
std::vector<NodeIOPin> start_outputs = {NodeIOPin("", false, CODE)};
NodeConfig start_config = NodeConfig("Input", start_inputs, start_outputs);

#endif //NODENET_TEMPLATES_H
