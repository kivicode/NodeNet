//
// Created by  KiviCode on 28/07/2020.
//

#ifndef NODENET_EXECUTOR_H
#define NODENET_EXECUTOR_H

#include "data_storages.h"

namespace Executor {

    std::pair<std::pair<int, int>, bool> getNextPinId(Editor& editor, Node& node, int localPinId) { // returns: {{parent node id, local pin id}, was found?}

        auto linkData = editor.getLinkToPin(node, localPinId);

        if (linkData.second) { // link was found
            int newPinId = linkData.first.start_attr;
            auto searchResult = editor.getNodeThatHasPinById(newPinId);
            if (searchResult.second) { // it's not an edge node (has incoming links)
                auto newPinSearchResult = searchResult.first.globalPinIdToLocal(newPinId);
                if (newPinSearchResult.second) { // target pin exists
                    return {{searchResult.first.id, newPinSearchResult.first}, true};
                }
            }
        }
        std::cout << "Link not found for: " << node.config.title << "\n";
        return {{0,0}, false};
    }
}

#endif //NODENET_EXECUTOR_H
