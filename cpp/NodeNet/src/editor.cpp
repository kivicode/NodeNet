#include "editor.h"
#include "generator.cpp"
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

        // Dockspace frame creation
        {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowBgAlpha(0.0f);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            bool p_open = true;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &p_open, window_flags);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("Dockspace");
            ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            ImGui::End();
        }

        for(int i = 0; i < 3; i++) {
            std::string name = "test" + std::to_string(i);
            ImGui::Begin(name.c_str());
            ImGuiID dockspace_id = ImGui::GetID(name.c_str());
            ImGui::DockSpace(dockspace_id);
            ImGui::End();
        }


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
//        std::cout << node.config.title << "\n";
        NodeGenerator::generateFromConfig(editor, node, node.config);
//        switch (node._type) {
//            case PrivatePinType::CUSTOM :
//                NodeGenerator::generateTestNode(editor, node, "");
//                break;
//
//            case PrivatePinType::START :
//                NodeGenerator::generateStartNode(editor, node);
//                break;
//
//            case PrivatePinType::FINISH :
//                NodeGenerator::generateFinishNode(editor, node);
//                break;
//        }
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
#ifdef DEBUG
                std::cout << "Create link: " << link.start_attr << "  " << link.end_attr << "\n";
#endif
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
                editor.nodes.push_back(node);
                editor.inputNodeIds.push_back(node.id);
                NodeGenerator::buildStartNode(editor.nodes[editor.nodes.size() - 1],
                                            "/Users/kivicode/Documents/GitHub/NodeNet/cpp/NodeNet/templates/start.node");
#ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
#endif
            }
            else if (ImGui::MenuItem("Custom node")) {

                const int node_id = ++editor.current_id;
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                Node node = Node(node_id, 0.f);
                editor.nodes.push_back(node);
                NodeGenerator::buildTestNode(editor.nodes[editor.nodes.size() - 1],
                                             "/Users/kivicode/Documents/GitHub/NodeNet/cpp/NodeNet/templates/test.node");

#ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
#endif
            }
            else if (ImGui::MenuItem("Finish node")) {

                const int node_id = ++editor.current_id;
                imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
                Node node = Node(node_id, 0.f);
                editor.nodes.push_back(node);
                editor.finishNodeIds.push_back(node.id - 1);
                NodeGenerator::buildFinishNode(editor.nodes[editor.nodes.size() - 1],
                                               "/Users/kivicode/Documents/GitHub/NodeNet/cpp/NodeNet/templates/finish.node");
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
            int i = editor.finishNodeIds[0];
            std::string finalCode = editor.nodes[i].generateProcessedCode(editor);
            std::cout << "\n\nCode:\"\n" << finalCode << "\"\n";
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
