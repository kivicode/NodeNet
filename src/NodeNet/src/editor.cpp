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
#include <SDL_keycode.h>

#include <algorithm>
#include <dependencies/imgui-1.76/imgui_internal.h>
#include "TextEditor.h"


namespace fs = std::filesystem;

namespace graphics {

    void handleNewLinks();
    void handleNewNodes();
    void handleDragging();
    void handleItemDeleting();
    void updateLinks();
    void drawNode(Node &node);
    void debug();
    void debug_save();
    void debug_load();
    void spawnNode(const std::string& path);
    void spawnNodeDragInstance(tinydir_file _file, int &j, bool starable);

    imnodes::EditorContext *context = nullptr;

    ImFont* editorFont;

    std::string consoleLogString;
    std::string consoleCodeString;


    Editor editor;
    TextEditor codeEditor;
    Logger console(&consoleLogString);
    Logger codeConsole(&consoleCodeString);

    MlTrainer trainer(&codeConsole);
    DatasetManager dataset;
    CodeManager codeManager;

    char newNodePath[1024] = {0};
    tinydir_file selectedDragDropFile;

    char newNodeSearch[MAX_SEARCH_LENGTH] = {0};

    int selectedImportStatement = -1;

    bool draggingWasStarted = false;
    ImVec2 posWhenStartedDragging = ImVec2(0, 0);
    float wheelSpeed = 10;
    bool invertScrolling= false;

    std::vector<NodeMenuItem> menuNodes = {};

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

        std::string favourites[] = {"Core/dense", "Core/flatten"};

        if (ImGui::CollapsingHeader("Favourites")) {
            for (auto & favourite : favourites) {
                tinydir_file tmp_file;
                strcpy(tmp_file.path, (PREFABS_PATH + "/" + favourite + ".node").c_str());
                strcpy(tmp_file.name, favourite.c_str());

                int j = 0;
                spawnNodeDragInstance(tmp_file, j, false);
            }
        }

        ImGui::Separator();
        if (ImGui::CollapsingHeader("Nodes")) {
            int j = 0;
            for (tinydir_file file : scanDir(PREFABS_PATH)) {

                if (!file.is_dir) {
                    spawnNodeDragInstance(file, j, true);

                } else {
                    if (ImGui::TreeNode(fnameToNodeName(file.name).c_str())) {
                        for (tinydir_file _file : scanDir(file.path)) {
                            spawnNodeDragInstance(_file, j, true);
                        }
                        ImGui::TreePop();
                    }
                }
            }
        }


        ImGui::End();
    }

    void show_configuration_settings() {
        ImGui::Begin("Configuration");

//        ImGui::BeginGroupPanel("Dataset settings", ImVec2(-1.0f, 0.0f));


        if (ImGui::CollapsingHeader("Dataset")) {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            std::vector<std::string> options = {"a", "d"};
            if (ImGui::BeginCombo("  Dataset type", "abc")) {
                for (const auto& option : options) {
//                    console.log(option);
                    if (ImGui::Selectable(option.c_str(), true)) {
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }

        if (ImGui::CollapsingHeader("Trainer")) {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::Combo("Optimizer", &trainer.selectedOptimizer, trainer.availableOptimizers);
            ImGui::PopItemWidth();
        }

        if (ImGui::CollapsingHeader("Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);

            ImGui::InputTextWithHint( "##hidelabel", "Save path", codeManager.fileSavePath, 1024);
            ImGui::SameLine();
            if(ImGui::Button("Select path")) {
                auto _path = tinyfd_saveFileDialog("File save path", "", 0, nullptr, "");
                if(!_path) return;
                strncpy(codeManager.fileSavePath, _path, 1024);
                std::string path = codeManager.fileSavePath;
                console.log("Path: " + path);
            }
            ImGui::PopItemWidth();
            ImGui::Separator();

            if (ImGui::TreeNode("Imports")) {
                ImGui::PushItemWidth(-1);
                ImGui::ListBox("##hidelabel", &selectedImportStatement, codeManager.imports);
                ImGui::PushFont(editorFont);
                if(ImGui::SmallButton("+")) codeManager.imports.emplace_back(tinyfd_inputBox("Import a new lib", "Type a new lib statement", "from lib import module"));
                ImGui::SameLine();
                if(ImGui::SmallButton("-")) codeManager.imports.erase(codeManager.imports.begin() + selectedImportStatement);
                ImGui::TreePop();
                ImGui::PopFont();
                ImGui::PopItemWidth();
            }
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
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node_DragDrop")) {
                console.log(std::string("Node from path: ") + std::string(selectedDragDropFile.path));
                spawnNode(selectedDragDropFile.path);
            }
            ImGui::EndDragDropTarget();
        }

        graphics::handleNewNodes();
        graphics::handleDragging();

        graphics::handleNewLinks();
        graphics::handleItemDeleting();

        ImGui::End();
    }

    void show_code_editor() {
        ImGui::Begin("Code Editor");
//        if (ImGui::IsWindowFocused()) {
            ImGui::InputText("##hidelabel", newNodePath, 1024);
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
                writeFile(std::string(newNodePath), codeEditor.GetText());
            }
            ImGui::PushFont(editorFont);
            codeEditor.Render("Code Editor");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node_DragDrop")) {
                    strcpy(newNodePath, selectedDragDropFile.path);
                    codeEditor.SetText(readFile(std::string(selectedDragDropFile.path)));
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopFont();
        ImGui::End();
    }

    void initCodeEditor() {
        auto lang = TextEditor::LanguageDefinition::NodeScript();

        codeEditor.SetLanguageDefinition(lang);
        codeEditor.SetPalette(TextEditor::GetCustomDarkPalette());
        codeEditor.SetShowWhitespaces(false);

        static const char *fileToEdit = "/Users/kivicode/Documents/GitHub/NodeNet/src/NodeNet/templates/finish.node";

        {
            std::ifstream t(fileToEdit);
            if (t.good()) {
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                codeEditor.SetText(str);
            }
        }
        codeEditor.inited = true;
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
        ImGui::SetWindowFocus("Code");
        trainer.train(editor, codeManager, codeManager.fileSavePath);
    }

    void drawNode(Node &node) {
        NodeGenerator::generateFromConfig(editor, node, node.config);
    }

    void spawnNode(const std::string& path) {
        const int node_id = ++editor.current_node_id;
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

    void spawnNodeDragInstance(tinydir_file _file, int &j, bool starable=false) {
        ImGui::Spacing(); ImGui::SameLine();
        ImGui::Spacing(); ImGui::SameLine();
        ImGui::Selectable(fnameToNodeName(_file.name).c_str(), false);
        if (ImGui::BeginDragDropSource()) {
            ImGui::Text(fnameToNodeName(_file.name).c_str(), j);
            strcpy(selectedDragDropFile.path, _file.path);
            ImGui::SetDragDropPayload("Node_DragDrop", &j, sizeof(int));
            ImGui::EndDragDropSource();
            j++;
        }
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
        Link link;
        if (imnodes::IsLinkCreated(&link.start_attr, &link.end_attr)) {
            link.id = ++editor.current_link_id;
            link.sort(editor.nodes);
            #ifdef DEBUG
                std::cout << "Create link: " << link.start_attr << "  " << link.end_attr << "\n";
            #endif
            editor.links.push_back(link);
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
            menuNodes.clear();
            for (tinydir_file file : scanDir(PREFABS_PATH)) {
                if (!file.is_dir) {
                    menuNodes.emplace_back(fnameToNodeName(file.name), file);
                } else {
                    for (tinydir_file _file : scanDir(file.path)) {
                        menuNodes.emplace_back(fnameToNodeName(_file.name), _file);
                    }
                }
            }
        }

        if (ImGui::BeginPopup("Spawn node")) {
            float f = 0;
            if(!ImGui::IsAnyItemActive()) ImGui::SetKeyboardFocusHere(0);
            bool entered = ImGui::InputTextWithHint("##hidelabel", "Type to search", newNodeSearch, MAX_SEARCH_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue);

            if(std::string(newNodeSearch).empty()) {
                for (tinydir_file file : scanDir(PREFABS_PATH)) {
                    if (!file.is_dir) {
                        makeMenuItemAndSpawnNodeFromFile(file);
                    } else {
                        if (ImGui::TreeNode(fnameToNodeName(file.name).c_str())) {
                            for (tinydir_file _file : scanDir(file.path)) {
                                makeMenuItemAndSpawnNodeFromFile(_file);
                            }
                            ImGui::TreePop();
                        }
                    }
                }
            } else {
                NodeMenuItem firstFound;
                bool wasFound = false;
                for (auto &node : menuNodes) {
                    std::cout << node.first << "\n";
                    if (findStringIC(node.first, newNodeSearch)) {
                        if (!wasFound) {
                            wasFound = true;
                            firstFound = node;
                        }
                        if (ImGui::MenuItem(node.first.c_str())) {
                            spawnNode(node.second.path);
                        }
                    }
                }
                if (wasFound && entered) {
                    spawnNode(firstFound.second.path);
                    ImGui::CloseCurrentPopup();
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

    void handleItemDeleting() {
        if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
          (ImGui::IsKeyReleased(SDL_SCANCODE_BACKSPACE) || ImGui::IsKeyReleased(SDL_SCANCODE_DELETE))) {

            int selectedNodeIds[imnodes::NumSelectedNodes()];
            imnodes::GetSelectedNodes(selectedNodeIds);

            for (int id : selectedNodeIds) {
                auto iter = std::find_if(editor.nodes.begin(), editor.nodes.end(),
                                         [id](const Node &node) -> bool { return node.id == id; });
                if (iter == editor.nodes.end()) continue;
                auto links = editor.getLinksOfNode(*iter);
                for (Link &link : links) {
                    auto link_iter = std::find_if(editor.links.begin(), editor.links.end(),
                                                  [link](const Link &link_) -> bool { return link_.id == link.id; });
                    if (link_iter != editor.links.end()) {
                        editor.links.erase(link_iter);
                    }
                }
            }

            for (int id : selectedNodeIds) {
                auto iter = std::find_if(editor.nodes.begin(), editor.nodes.end(),
                                         [id](const Node &node) -> bool { return node.id == id; });
                if (iter != editor.nodes.end()) {
                    editor.nodes.erase(iter);
                }
            }
        }
    }

    void debug(){

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

    void NewNode() {
        ImGui::SetWindowFocus("Code Editor");
        auto path = std::string(tinyfd_saveFileDialog("New Node", "unnamed.node", 0, {}, ""));
        strcpy(newNodePath, path.c_str());
    }

    void EditNode() {
        ImGui::SetWindowFocus("Code Editor");
        auto path = std::string(tinyfd_openFileDialog("New Node", "", 0, {}, "", false));
        strcpy(newNodePath, path.c_str());
        codeEditor.SetText(readFile(path));
    }

    void NodeEditorLoadFonts() {
        editorFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("fonts/dejavu-sans-mono/DejaVuSansMono.ttf", 18);
    }

    void NodeEditorInitialize() {
        context = imnodes::EditorContextCreate();
        imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);

        imnodes::IO& io = imnodes::GetIO();
        io.link_detach_with_modifier_click.modifier = &ImGui::GetIO().KeyCtrl;
    }

    void NodeEditorShow() {
        if (!codeEditor.inited) initCodeEditor();
        show_code_inspector();
        show_node_inspector();
        show_settings();
        show_configuration_settings();
        show_log();
        show_code_editor();
        show_editor("Nodes", editor);
    }

    void NodeEditorShutdown() {
        imnodes::PopAttributeFlag();
        imnodes::EditorContextFree(context);
    }
} // namespace example
