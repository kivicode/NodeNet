//
// Created by  KiviCode on 18/08/2020.
//

#ifndef NODENET_FONTS_H
#define NODENET_FONTS_H

#include <dependencies/imgui-1.76/imgui.h>

namespace font_manager {
    class fonts_storage {
    public:
        ImFont *editorFont = nullptr;
        ImFont *headerFont = nullptr;
        ImFont *bodyFont   = nullptr;
    };

    extern fonts_storage fonts;
}

#endif //NODENET_FONTS_H
