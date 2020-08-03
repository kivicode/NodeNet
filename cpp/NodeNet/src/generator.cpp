//
// Created by  KiviCode on 26/07/2020.
//

#pragma once

#include "imnodes.h"
#include "imgui.h"

#include "data_types.h"
#include "settings.h"
#include "templates.h"

#include "utils.h"
#include <any>


namespace NodeGenerator {

    int uniquePinIdCounter = 1;

    void generateFromConfig(Editor& editor, Node& node, NodeConfig& config) {

        imnodes::BeginNode(node.id);

        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(config.title.c_str());
        imnodes::EndNodeTitleBar();

        // Inputs
        {
            for (int i = 0; i < config.inputs.size(); i++) {
                std::string pinName = config.inputs[i].name;
                int pinId = GET_INPUT_ID(node, i);

                imnodes::BeginInputAttribute(pinId, config.inputs[i].dataType == CODE ? imnodes::PinShape_QuadFilled : imnodes::PinShape_CircleFilled);
                const float label_width = ImGui::CalcTextSize(pinName.c_str()).x;
                ImGui::TextUnformatted(pinName.c_str());

                if (config.inputs[i].isEditable && !editor.pinHasLink(pinId)) { // Draw slider if has no links
                    ImGui::SameLine();
                    float nodeWidth = config.inputs[i].dataType == SliderDataType::STRING ? 170 : 100;
                    ImGui::PushItemWidth(nodeWidth - label_width);

                    switch(config.inputs[i].dataType) {
                        case SliderDataType::FLOAT:
                            ImGui::DragFloat("##hidelabel", &node.inputs[pinId].f,
                                                                  config.inputs[i].sliderSpeed,
                                                                  config.inputs[i].minSliderVal,
                                                                  config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::INTEGER:
                            ImGui::DragInt("##hidelabel", &node.inputs[pinId].i,
                                           config.inputs[i].sliderSpeed,
                                           (int)config.inputs[i].minSliderVal,
                                           (int)config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::STRING:
                            ImGui::InputText("##hidelabel", node.inputs[pinId].s, INPUT_BUFF_SIZE);
                            break;

                        default:
                            break;
                    }
                    ImGui::PopItemWidth();
                }

                imnodes::EndInputAttribute();
            }
        }


        // Outputs
        {
            for (int i = 0; i < config.outputs.size(); i++) { // draw node staff
                std::string pinName = config.outputs[i].name;
                int pinId = GET_OUTPUT_ID(node, i);

                imnodes::BeginOutputAttribute(pinId, config.outputs[i].dataType == CODE ? imnodes::PinShape_QuadFilled : imnodes::PinShape_CircleFilled);
                const float text_width = ImGui::CalcTextSize(pinName.c_str()).x;
                ImGui::Indent(120.f + ImGui::CalcTextSize(pinName.c_str()).x - text_width);
                ImGui::TextUnformatted(pinName.c_str());
                imnodes::EndOutputAttribute();
            }
        }

        imnodes::EndNode();
    }

    void buildTestNode(Node &node, const std::string &path) {
        node._type = PrivatePinType::CUSTOM;
        test_config.setCode(readFile(path));
        node.setConfig(test_config);
        node.init();
    }

    void buildFinishNode(Node &node, const std::string &path) {
        node._type = PrivatePinType::FINISH;
        finish_config.setCode(readFile(path));
        node.setConfig(finish_config);
        node.init();
    }

    void buildStartNode(Node &node, const std::string &path) {
        node._type = PrivatePinType::START;
        start_config.setCode(readFile(path));
        node.setConfig(start_config);
        node.init();
    }

}