//
// Created by  KiviCode on 26/07/2020.
//

#pragma once

#include "imnodes.h"
#include "imgui.h"

#include "src/Core/data_types.h"
#include "src/settings.h"
#include "src/Debug/templates.h"

#include "src/Core/utils.h"
#include <any>
#include "src/Core/exceptions.h"
#include "src/fonts.h"



namespace NodeGenerator {

    void generateFromConfig(Editor& editor, Node& node, NodeConfig& config) {
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBar,        IM_COL32(config.headColor[0] * 255,       config.headColor[1] * 255,       config.headColor[2] * 255,       255));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarHovered, IM_COL32(config.headColor[0] * 255,       config.headColor[1] * 255,       config.headColor[2] * 255,       255));
        imnodes::PushColorStyle(imnodes::ColorStyle_TitleBarSelected,IM_COL32(config.headColor[0] * 255,       config.headColor[1] * 255,       config.headColor[2] * 255,       255));

        imnodes::BeginNode(node.id);

        NodeException ex = node.checkExceptions(editor);
        imnodes::MarkNode(node.id, node.mark, node.markDescription);

        ImGui::PushFont(font_manager::fonts.headerFont);
        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(config.title.c_str());
        imnodes::EndNodeTitleBar();
        ImGui::PopFont();

        ImGui::PushFont(font_manager::fonts.bodyFont);


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
                            ImGui::InputFloat("##hidelabel", &node.inputs[pinId].f, 0, 0);
                            break;

                        case SliderDataType::FLOAT_DRAG:
                            ImGui::DragFloat("##hidelabel", &node.inputs[pinId].f,
                                             config.inputs[i].sliderSpeed,
                                             config.inputs[i].minSliderVal,
                                             config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::INTEGER:
                            ImGui::InputInt("##hidelabel", &node.inputs[pinId].i, 0, 0);
                            break;

                        case SliderDataType::INTEGER_DRAG:
                            ImGui::DragInt("##hidelabel", &node.inputs[pinId].i,
                                           config.inputs[i].sliderSpeed,
                                           (int)config.inputs[i].minSliderVal,
                                           (int)config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::STRING:
                            ImGui::InputText("##hidelabel", node.inputs[pinId].s, INPUT_BUFF_SIZE);
                            break;

                        case SliderDataType::STRING_SELECTOR:
                            ImGui::PushItemWidth(100);
                            if (ImGui::BeginCombo("##hidelabel", node.config.inputs[i].options.at(node.config.inputs[i].selectedOption).c_str())) {
                                for (int j = 0; j < node.config.inputs[i].options.size(); j++) {
                                    if (ImGui::Selectable(node.config.inputs[i].options.at(j).c_str(),
                                                          j == node.config.inputs[i].selectedOption)) {
                                        node.config.inputs[i].selectedOption = j;
                                        node.inputs[pinId].setString(node.config.inputs[i].options[j]);
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::PopItemWidth();
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
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        imnodes::PopColorStyle();
        ImGui::PopFont();
    }

    void buildTestNode(Node &node, const std::string &path) {
        node._type = PrivatePinType::CUSTOM;
        test_config.setCode(readFile(path));
        node.setConfig(test_config);
        node.init();
    }

    void buildFinishNode(Node &node, const std::string &path) {
        finish_config.setCode(readFile(path));
        node.setConfig(CodeExecutor::configFromFile(path));
        node.init();
    }

    void buildStartNode(Node &node, const std::string &path) {
        node._type = PrivatePinType::START;
        start_config.setCode(readFile(path));
        start_config.type = START;
        node.setConfig(start_config);
        node.init();
    }

    void buildNode(Node &node, const std::string &path) {
        node.setConfig(CodeExecutor::configFromFile(path));
        node.init();
    }



}