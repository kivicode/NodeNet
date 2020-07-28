//
// Created by  KiviCode on 26/07/2020.
//

#include "imnodes.h"
#include "imgui.h"
#include "graph.h"

#include "data_storages.h"
#include "settings.h"

#include "utils.h"

namespace NodeGenerator {

    int uniquePinIdCounter = 1;

    void generateFromConfig(Editor& editor, Node& node, NodeConfig& config) {

        imnodes::BeginNode(node.id);

        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(config.title.c_str());
        imnodes::EndNodeTitleBar();

        // Inputs
        {
            for (int i = 0; i < config.inputs.size(); i++) { // init start values
                int pinId = node.id + (MAX_PINS * (i + 1));
                bool hasVal = MAP_HAS_KEY(node.inputs, pinId);
                if (!hasVal) {
                    node.inputs[pinId] = 0;
                    node.inputIds.push_back(pinId);
#ifdef DEBUG
                    std::cout << "Create input for node: " << config.title.c_str() << "  Input name: " << config.inputs[i].name << " id: " << pinId << "\n";
#endif
                }
            }

            for (int i = 0; i < config.inputs.size(); i++) {
                std::string pinName = config.inputs[i].name;
                int pinId = GET_INPUT_ID(node, i);

                imnodes::BeginInputAttribute(pinId);
                const float label_width = ImGui::CalcTextSize(pinName.c_str()).x;
                ImGui::TextUnformatted(pinName.c_str());

                if (config.inputs[i].isEditable && !editor.pinHasLink(pinId)) { // Draw slider if has no links
                    ImGui::SameLine();
                    int nodeWidth = config.inputs[i].dataType == SliderDataType::STRING ? 170 : 100;
                    ImGui::PushItemWidth(nodeWidth - label_width);

                    switch(config.inputs[i].dataType) {
                        case SliderDataType::FLOAT:
                            ImGui::DragFloat("##hidelabel", &node.inputs[pinId],
                                                                  config.inputs[i].sliderSpeed,
                                                                  config.inputs[i].minSliderVal,
                                                                  config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::INTEGER:
                            ImGui::DragInt("##hidelabel", reinterpret_cast<int *>(&node.inputs[pinId]),
                                           config.inputs[i].sliderSpeed,
                                           (int)config.inputs[i].minSliderVal,
                                           (int)config.inputs[i].maxSliderVal);
                            break;

                        case SliderDataType::STRING:
                            if(node._type != PrivatePinType::CUSTOM) {
                                ImGui::InputText("##hidelabel", reinterpret_cast<char *>(node.modelName), 1024);
                            }
                            break;
                    }
                    ImGui::PopItemWidth();
                }

                imnodes::EndInputAttribute();
            }
        }


        // Outputs
        {
            for (int i = 0; i < config.outputs.size(); i++) { // init start values
                int pinId = GET_OUTPUT_ID(node, i);
                bool hasVal = MAP_HAS_KEY(node.outputs, pinId);
                if (!hasVal) {
                    node.outputs[pinId] = 0;
                    node.outputIds.push_back(pinId);
#ifdef DEBUG
                    std::cout << "Create output for node: " << config.title.c_str() << "  Output name: " << config.outputs[i].name << " id: " << pinId << "\n";
#endif
                }
            }

            for (int i = 0; i < config.outputs.size(); i++) { // draw node staff
                std::string pinName = config.outputs[i].name;
                int pinId = GET_OUTPUT_ID(node, i);

                imnodes::BeginOutputAttribute(pinId);
                const float text_width = ImGui::CalcTextSize(pinName.c_str()).x;
                ImGui::Indent(120.f + ImGui::CalcTextSize(pinName.c_str()).x - text_width);
                ImGui::TextUnformatted(pinName.c_str());
                imnodes::EndOutputAttribute();
            }
        }

        imnodes::EndNode();
    }

    void generateTestNode(Editor& editor, Node& node) {
        std::vector<NodeIOPin> inputs =  {NodeIOPin("input", true, SliderDataType::FLOAT, 0, 1, 0.01f),
                                          NodeIOPin("Layers", false, SliderDataType::INTEGER, 0, 1000, 1)};
        std::vector<NodeIOPin> outputs = {NodeIOPin("output", false), NodeIOPin("output 2", false)};

        NodeConfig test_config = NodeConfig("abc", inputs, outputs);
        NodeGenerator::generateFromConfig(editor, node, test_config);
    }

    void generateFinishNode(Editor& editor, Node& node) {
        std::vector<NodeIOPin> inputs =  {NodeIOPin("Code", false), NodeIOPin("Model name", true, SliderDataType::STRING)};

        node._type = PrivatePinType::FINISH;
        NodeConfig test_config = NodeConfig("Finish", inputs, std::vector<NodeIOPin>{});
        NodeGenerator::generateFromConfig(editor, node, test_config);
    }

    void generateStartNode(Editor& editor, Node& node) {
        std::vector<NodeIOPin> inputs =  {NodeIOPin("Input name", true, SliderDataType::STRING)};
        std::vector<NodeIOPin> outputs = {NodeIOPin("", false)};

        node._type = PrivatePinType::START;
        NodeConfig test_config = NodeConfig("Input", inputs, outputs);
        NodeGenerator::generateFromConfig(editor, node, test_config);
    }

}