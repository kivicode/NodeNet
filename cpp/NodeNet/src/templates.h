//
// Created by  KiviCode on 30/07/2020.
//

#ifndef NODENET_TEMPLATES_H
#define NODENET_TEMPLATES_H

#include "data_storages.h"

std::vector<NodeIOPin> testInputs =  {NodeIOPin("input", true, SliderDataType::CODE, 0, 1, 0.01f),
                                      NodeIOPin("Layers", true, SliderDataType::INTEGER, 0, 1000, 1)};
std::vector<NodeIOPin> testOutputs = {NodeIOPin("output", false), NodeIOPin("output 2", false)};
NodeConfig test_config = NodeConfig("abc", testInputs, testOutputs);

std::vector<NodeIOPin> finish_inputs =  {NodeIOPin("Code", false), NodeIOPin("Model name", true, SliderDataType::STRING)};
NodeConfig finish_config = NodeConfig("Finish", finish_inputs, std::vector<NodeIOPin>{});

std::vector<NodeIOPin> start_inputs =  {NodeIOPin("Input name", true, SliderDataType::STRING)};
std::vector<NodeIOPin> start_outputs = {NodeIOPin("", false)};
NodeConfig start_config = NodeConfig("Input", start_inputs, start_outputs);

#endif //NODENET_TEMPLATES_H
