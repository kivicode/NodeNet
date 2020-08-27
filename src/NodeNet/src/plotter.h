//
// Created by  KiviCode on 27/08/2020.
//

#ifndef NODENET_PLOTTER_H
#define NODENET_PLOTTER_H

#include <map>
#include "implot/implot.h"

class Plotter {
public:

    std::map<std::string, std::vector<float>> plots = {};

    void push(const std::string& name, float val) {
        if (!(MAP_HAS_KEY(plots, name))) {
            plots[name] = {};
        }
        plots.at(name).push_back(val);
    }

    void render() {
        if (ImPlot::BeginPlot("Training", "epoch", "val")) {

            for (auto const&[key, val] : plots) {
                ImPlot::PlotLine(key.c_str(), val.data(), val.size());
            }

            ImPlot::EndPlot();
        }
    }
};

#endif //NODENET_PLOTTER_H
