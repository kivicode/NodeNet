#include "editor.h"
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <imnodes.h>
#include <cstdio>
#include <SDL2/SDL.h>
#include <GL/gl3w.h>
#include <dependencies/imgui-1.76/imgui_internal.h>


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

//ImVec2 ImGui::ImVec

static ImVector<ImRect> s_GroupPanelLabelStack;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
    ImGui::BeginGroup();

    auto cursorPos = ImGui::GetCursorScreenPos();
    auto itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();
    ImGui::BeginGroup();

    ImVec2 effectiveSize = size;
    if (size.x < 0.0f)
        effectiveSize.x = ImGui::GetContentRegionAvail().x;
    else
        effectiveSize.x = size.x;
    ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::TextUnformatted(name);
    auto labelMin = ImGui::GetItemRectMin();
    auto labelMax = ImGui::GetItemRectMax();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
    ImGui::BeginGroup();

    //ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x          -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x         -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x                   -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth();
    ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

    s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void ImGui::EndGroupPanel()
{
    ImGui::PopItemWidth();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();

    ImGui::EndGroup();

    //ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

    ImGui::EndGroup();

    auto itemMin = ImGui::GetItemRectMin();
    auto itemMax = ImGui::GetItemRectMax();
    //ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

    auto labelRect = s_GroupPanelLabelStack.back();
    s_GroupPanelLabelStack.pop_back();

    ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
    ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for (int i = 0; i < 4; ++i)
    {
        switch (i)
        {
            // left half-plane
            case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
                // right half-plane
            case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
                // top
            case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
                // bottom
            case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
        }

        ImGui::GetWindowDrawList()->AddRect(
                frameRect.Min, frameRect.Max,
                ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
                halfFrame.x);

        ImGui::PopClipRect();
    }

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x          += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x         += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x                   += frameHeight;

    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    ImGui::EndGroup();
}

void setStyle() {
    ImGuiStyle &st = ImGui::GetStyle();
    st.FrameBorderSize = 1.0f;
    st.FramePadding = ImVec2(4.0f,2.0f);
    st.ItemSpacing = ImVec2(8.0f,2.0f);
    st.WindowBorderSize = 1.0f;
    st.TabBorderSize = 1.0f;
    st.WindowRounding = 1.0f;
    st.ChildRounding = 1.0f;
    st.FrameRounding = 1.0f;
    st.ScrollbarRounding = 1.0f;
    st.GrabRounding = 1.0f;
    st.TabRounding = 1.0f;

// Setup style
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
    colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
    colors[ImGuiCol_Border]                = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//    colors[ImGuiCol_FrameBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
    colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
    colors[ImGuiCol_Button]                = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
    colors[ImGuiCol_Header]                = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
    colors[ImGuiCol_Separator]             = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
    colors[ImGuiCol_SeparatorActive]       = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
    colors[ImGuiCol_ResizeGrip]            = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
    colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
    colors[ImGuiCol_Tab]                   = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_TabUnfocused]          = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_DockingPreview]        = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
    colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
void setStyleCherry() {
    // cherry colors, 3 intensities
#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
    // text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
//    style.Colors[ImGuiCol_ChildWindowBg]         = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = MED( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = MED( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
//    style.Colors[ImGuiCol_Column]                = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
//    style.Colors[ImGuiCol_ColumnHovered]         = MED( 0.78f);
//    style.Colors[ImGuiCol_ColumnActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);
    // [...]
//    style.Colors[ImGuiCol_ModalWindowDarkening]  = BG( 0.73f);

    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5, 2);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 20.0f;
    style.GrabRounding             = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
}
void setStyleDark() {
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
    colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

    style->ChildRounding = 4.0f;
    style->FrameBorderSize = 1.0f;
    style->FrameRounding = 2.0f;
    style->GrabMinSize = 7.0f;
    style->PopupRounding = 2.0f;
    style->ScrollbarRounding = 12.0f;
    style->ScrollbarSize = 13.0f;
    style->TabBorderSize = 1.0f;
    style->TabRounding = 0.0f;
    style->WindowRounding = 4.0f;
}

int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS,
		SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	SDL_Window* window = SDL_CreateWindow(
		"NodeNet",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1280,
		720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	if (gl3wInit())
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	imnodes::Initialize();

	// Setup style
	ImGui::StyleColorsDark();
	imnodes::StyleColorsCustomDark();

	ImGui::GetIO().Fonts->AddFontFromFileTTF("fonts/Roboto/Roboto-Regular.ttf", 16.0f);

	bool done = false;
	bool initialized = false;

	{
		const ImVec4 clear_color = ImVec4(32.f/255, 32.f/255, 32.f/255, 1.00f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	}

	bool dockspaceBuilt = false;

	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
        setStyleDark();
        ImGui::GetStyle().WindowBorderSize = 0.0f;
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);


		ImGui::NewFrame();


        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetWindowViewport()->GetWorkSize());
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("Dockspace", nullptr, window_flags);
        ImGui::PopStyleVar(1);


        ImGuiID dockspace_id = ImGui::GetID("Dockspace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (!dockspaceBuilt) {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags); // Add empty node
            ImGui::DockBuilderSetNodeSize(dockspace_id, {viewport->GetWorkSize().x, viewport->GetWorkSize().y});

            ImGuiID dock_main_id = dockspace_id;

            ImGuiID dock_id_left   = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left,  0.15f, nullptr, &dock_main_id);
            ImGuiID dock_id_right  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down,  0.20f, nullptr, &dock_main_id);
            ImGuiID dock_id_center = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up,    0.20f, nullptr, &dock_main_id);

            ImGui::DockBuilderDockWindow("Nodes",         dock_id_center); // editor
            ImGui::DockBuilderDockWindow("Code",          dock_id_bottom); // bottom
            ImGui::DockBuilderDockWindow("Inspector",     dock_id_left);   // left
            ImGui::DockBuilderDockWindow("Configuration", dock_id_right);  // right
            ImGui::DockBuilderDockWindow("Settings",      dock_id_right);  // right
            ImGui::DockBuilderFinish(dockspace_id);
            dockspaceBuilt = true;
        }

        ImGui::End();
        

		if (!initialized)
		{
			initialized = true;
			graphics::NodeEditorInitialize();
		}

		graphics::NodeEditorShow();

		// Rendering
		ImGui::Render();

		int fb_width, fb_height;
		SDL_GL_GetDrawableSize(window, &fb_width, &fb_height);
		glViewport(0, 0, fb_width, fb_height);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	graphics::NodeEditorShutdown();
	imnodes::Shutdown();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}