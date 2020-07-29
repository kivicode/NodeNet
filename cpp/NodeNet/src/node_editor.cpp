#include "node_editor.h"
#include "node_generator.h"
#include "executor.h"

#include <imnodes.h>
#include <imgui.h>
#include <SDL2/SDL_scancode.h>

#include <algorithm>

namespace graphics {

    void handleNewLinks();
    void handleNewNodes();
    void updateLinks();
    void drawNode(Node &node);
    void debug();

    Editor editor;

    void show_editor(const char *editor_name, Editor &editor) {
        imnodes::EditorContextSet(editor.context);

        ImGui::Begin(editor_name);

        imnodes::BeginNodeEditor();

        graphics::updateLinks();

        for (Node &node : editor.nodes) {
            graphics::drawNode(node);
        } // update/redraw nodes

        imnodes::EndNodeEditor();

        graphics::handleNewNodes();

        graphics::handleNewLinks();
        graphics::debug();

        ImGui::End();
    }

    void drawNode(Node &node) {
        switch (node._type) {
            case PrivatePinType::CUSTOM :
                NodeGenerator::generateTestNode(editor, node);
                break;

            case PrivatePinType::START :
                NodeGenerator::generateStartNode(editor, node);
                break;

            case PrivatePinType::FINISH :
                NodeGenerator::generateFinishNode(editor, node);
                break;
        }
    }

    void updateLinks() {
        for (const Link &link : editor.links) {
            imnodes::Link(link.id, link.start_attr, link.end_attr);
        }
    }

    void handleNewLinks() {
        {
            Link link;
            if (imnodes::IsLinkCreated(&link.start_attr, &link.end_attr)) {
                link.id = ++editor.current_id;
                link.sort(editor.nodes);
                std::cout << "Create link: " << link.start_attr << "  " << link.end_attr << "\n";
                editor.links.push_back(link);
            }
        }

        {
            int link_id;
            if (imnodes::IsLinkDestroyed(&link_id)) {
                auto iter = std::find_if(
                        editor.links.begin(), editor.links.end(), [link_id](const Link &link) -> bool {
                            return link.id == link_id;
                        });
                assert(iter != editor.links.end());
                editor.links.erase(iter);
            }
        }
    }

    void handleNewNodes() {

        const bool open_popup = !ImGui::IsAnyItemHovered() &&
                                 ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                 ImGui::GetIO().MouseClicked[1];

        if (open_popup) {
            ImGui::OpenPopup("Spawn node");
        }

        if (ImGui::BeginPopup("Spawn node")) {
            if (ImGui::MenuItem("Input node")) {

                const int node_id = ++editor.current_id;
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                Node node = Node(node_id, 0.f);
                node._type = PrivatePinType::START;
                editor.nodes.push_back(node);
                editor.inputNodeIds.push_back(node.id);
#ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
#endif
            }
            else if (ImGui::MenuItem("Custom node")) {

                const int node_id = ++editor.current_id;
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                Node node = Node(node_id, 0.f);
                node._type = PrivatePinType::CUSTOM;
                editor.nodes.push_back(node);
#ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
#endif
            }
            else if (ImGui::MenuItem("Finish node")) {

                const int node_id = ++editor.current_id;
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                Node node = Node(node_id, 0.f);
                node._type = PrivatePinType::FINISH;
                editor.nodes.push_back(node);
                editor.finishNodeIds.push_back(node.id);
#ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
#endif
            }
            ImGui::EndPopup();
        }
    }

    void debug() {
        if (ImGui::IsKeyReleased(SDL_SCANCODE_K)) {
            std::cout << "\n\n";
            editor.nodes[0].setBaseCode("a = input(\"abc\")\nb = input(\"lol\")\nj = output(\"qwqw\")\n");
            auto res = Executor::getIOCodePositionsAndLengths(editor.nodes[0].baseCode);
            editor.nodes[0].replaceInputsWithValues(res);
//            for (auto io : res) {
//                std::cout << (io.first ? "Input" : "Output") << ": pos=" << io.second.first << " len=" << io.second.second << "\n";
//            }
//            for (int finishNodeId : editor.finishNodeIds) {
//                auto sequence = Executor::getSequenceOfPinsForInputValue(editor, PinLocation(finishNodeId, 0)); // First pin of a finish node is always a code pin
//                int i = 0;
//                for (PinLocation pinLocation : sequence) {
//                    std::cout << "[" << i << "]  " << printPinLocation(pinLocation) << "\n";
//                    i++;
//                }
//            }
        }
    }

    void NodeEditorInitialize() {
        editor.context = imnodes::EditorContextCreate();
        imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);

        imnodes::IO& io = imnodes::GetIO();
        io.link_detach_with_modifier_click.modifier = &ImGui::GetIO().KeyCtrl;
    }

    void NodeEditorShow() {
        show_editor("editor", editor);
    }

    void NodeEditorShutdown() {
        imnodes::PopAttributeFlag();
        imnodes::EditorContextFree(editor.context);
    }
} // namespace example
