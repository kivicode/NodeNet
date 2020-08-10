#include "editor.h"
#include "generator.cpp"
#include "executor.h"
#include "dataset_manager.h"
#include "trainer.h"
#include "code_manager.h"
#include "logger.h"
#include "exceptions.h"

#include <imnodes.h>
#include <imgui.h>

#include <algorithm>
#include <dependencies/imgui-1.76/imgui_internal.h>

namespace fs = std::filesystem;

namespace graphics {

    void handleNewLinks();
    void handleNewNodes();
    void handleDragging();
    void updateLinks();
    void drawNode(Node &node);
    void debug();
    void debug_save();
    void debug_load();

    imnodes::EditorContext *context = nullptr;

    std::string consoleLogString;
    std::string consoleCodeString;
    char consoleInputBuff[1024] = {0};

    Editor editor;
    Logger console(&consoleLogString);
    Logger codeConsole(&consoleCodeString);

    MlTrainer trainer(&codeConsole);
    DatasetManager dataset;
    CodeManager codeManager;


    bool draggingWasStarted = false;
    ImVec2 posWhenStartedDragging = ImVec2(0, 0);
    float wheelSpeed = 10;
    bool invertScrolling= false;

    void show_code_inspector() {
        ImGui::Begin("Code");
        trainer.exec_step();
        codeConsole.draw();
        ImGui::End();
    }

    void show_log() {
        ImGui::Begin("Log");
        console.draw();
        ImGui::End();
    }

    void show_node_inspector() {
        ImGui::Begin("Inspector");

        ImGui::BeginChild("OuterRegion");
        for (int i = 0; i < 40; ++i) {
            ImGui::Text("Entry %i", i);
        }
        ImGui::EndChild();


        ImGui::End();
    }

    void show_configuration_settings() {
        ImGui::Begin("Configuration");

//        ImGui::BeginGroupPanel("Dataset settings", ImVec2(-1.0f, 0.0f));


        if (ImGui::CollapsingHeader("Dataset")) {

        }

        if (ImGui::CollapsingHeader("Trainer")) {

        }

        if (ImGui::CollapsingHeader("Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

            ImGui::InputText( "Save path", codeManager.fileSavePath, 1024);

            if(ImGui::Button("Select path")) {
                auto _path = tinyfd_saveFileDialog("File save path", "", 0, nullptr, "");
                if(!_path) return;
                strncpy(codeManager.fileSavePath, _path, 1024);
                std::string path = codeManager.fileSavePath;
                console.log("Path: " + path);
            }
            ImGui::PopItemWidth();
        }

        ImGui::End();
    }

    void show_settings() {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if(ImGui::CollapsingHeader("Saving", ImGuiTreeNodeFlags_DefaultOpen)) {

        }

        ImGui::End();
    }

    void generateCode();
    void train();

    void show_editor(const char *editor_name, Editor& _editor) {
        imnodes::EditorContextSet(context);

        ImGui::Begin(editor_name);

        if (ImGui::Button("Generate code")) generateCode();
        ImGui::SameLine();
        if (ImGui::Button("Train")) train();
        ImGui::SameLine();
        if (ImGui::Button("Debug")) debug();
        ImGui::SameLine();
        if (ImGui::Button("Debug save")) debug_save();
        ImGui::SameLine();
        if (ImGui::Button("Debug load")) debug_load();

        imnodes::BeginNodeEditor();

        graphics::updateLinks();

        for (Node &node : _editor.nodes) {
            graphics::drawNode(node);
        } // update/redraw nodes

        imnodes::EndNodeEditor();

        graphics::handleNewNodes();
        graphics::handleDragging();

        graphics::handleNewLinks();

        ImGui::End();
    }

    void generateCode() {
        try {
            codeManager.imports.clear();
            codeManager.addLib("from keras.models import Model");
            codeManager.addLib("from keras.layers import *");
            codeManager.generateCode(editor);
            codeManager.save();
        } catch (NodeException& exception) {
            console.log("Unnamed node: " + editor.nodes[exception.nodeId-1].config.title, LogLevel::ERR);
            return;
        }
//        console.log("Code generated:\n" + codeManager.getCode());
    }

    void train() {
        if (std::string(codeManager.fileSavePath).empty()) {
            console.log("Empty file save path", ERR);
            return;
        }
        codeManager.save();
        trainer.train(codeManager.fileSavePath);
    }

    void drawNode(Node &node) {
        NodeGenerator::generateFromConfig(editor, node, node.config);
    }

    void spawnNode(const std::string& path) {
        const int node_id = ++editor.current_id;
        imnodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
        Node node = Node(node_id, 0.f);
        editor.nodes.push_back(node);
        editor.inputNodeIds.push_back(node.id);
        NodeGenerator::buildNode(editor.nodes[editor.nodes.size() - 1], path);
        if (editor.nodes[editor.nodes.size() - 1]._type == FINISH) editor.finishNodeIds.push_back(node.id - 1);
        #ifdef DEBUG
                std::cout << "Create node: " << node_id << "\n";
        #endif
    }

    void makeMenuItemAndSpawnNodeFromFile(tinydir_file file) {
        std::string name(file.name);
        name = fnameToNodeName(name);

        if (ImGui::MenuItem(name.c_str())) {
            spawnNode(file.path);
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

        if(draggingWasStarted) return;

        const bool open_popup = !ImGui::IsAnyItemHovered() &&
                                 ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                                 ImGui::GetIO().MouseClicked[1];

        if (open_popup) {
            ImGui::OpenPopup("Spawn node");
        }

        if (ImGui::BeginPopup("Spawn node")) {

            for (tinydir_file file : scanDir(PREFABS_PATH)) {
                if (file.name[0] == '.') continue;

                if (!file.is_dir) {
                    makeMenuItemAndSpawnNodeFromFile(file);
                } else {
                    if (ImGui::BeginMenu(fnameToNodeName(file.name).c_str())){
                        for (tinydir_file _file : scanDir(PREFABS_PATH)) {
                            makeMenuItemAndSpawnNodeFromFile(_file);
                        }
                        ImGui::EndMenu();
                    }
                }
            }

            ImGui::EndPopup();
        }
    }

    void handleDragging() {
        if (!imnodes::IsEditorHovered()) return;
        const bool nowDragging = ImGui::IsMouseDragging(1, 2);

        if(!draggingWasStarted && nowDragging) {
            draggingWasStarted = true;
            posWhenStartedDragging = imnodes::EditorContextGetPanning();
        } else if(draggingWasStarted && !nowDragging) {
            draggingWasStarted = false;
        } else if(draggingWasStarted && nowDragging) {
            auto mouseDelta = ImGui::GetMouseDragDelta(1);
            auto editorDelta = ImVec2(posWhenStartedDragging.x + mouseDelta.x, posWhenStartedDragging.y + mouseDelta.y);
            imnodes::EditorContextResetPanning(editorDelta);
            // std::cout << "Mouse move: (" << delta.x << ", " << delta.y << ")\n";
        }
        auto cur = imnodes::EditorContextGetPanning();
        float speed = wheelSpeed * (invertScrolling ? -1.f : 1.f);
        imnodes::EditorContextResetPanning(ImVec2(cur.x - (ImGui::GetIO().MouseWheelH * speed), cur.y + (ImGui::GetIO().MouseWheel * speed)));
    }

    void debug(){
        std::string path = "/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/templates/finish.node";
        CodeExecutor::configFromFile(path);
//        std::cout << "Decl: " << CodeExecutor::getDeclarationSegment(readFile(path));
    }

    void debug_save() {
        std::ofstream os;
        os.open("out.cereal", std::ios::out | std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        for(int i = 0; i < editor.nodes.size(); i++) {
            editor.nodes.at(i).gridPosition = imnodes::GetNodeGridSpacePos(editor.nodes.at(i).id-1);
        }
        archive(editor);
        os.close();

    }

    void debug_load() {
        std::ifstream fs;
        fs.open("out.cereal", std::ios::in | std::ios::binary);
        cereal::BinaryInputArchive archive(fs);
        archive(editor);
        for(int i = 0; i < editor.nodes.size(); i++) {
            imnodes::SetNodeGridSpacePos(editor.nodes.at(i).id-1, editor.nodes.at(i).gridPosition);
        }
        fs.close();
    }

    void NodeEditorInitialize() {
        context = imnodes::EditorContextCreate();
        imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);

        imnodes::IO& io = imnodes::GetIO();
        io.link_detach_with_modifier_click.modifier = &ImGui::GetIO().KeyCtrl;
    }

    void NodeEditorShow() {
        show_code_inspector();
        show_node_inspector();
        show_settings();
        show_configuration_settings();
        show_log();
        show_editor("Nodes", editor);
    }

    void NodeEditorShutdown() {
        imnodes::PopAttributeFlag();
        imnodes::EditorContextFree(context);
    }
} // namespace example
