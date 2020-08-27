//
// Created by  KiviCode on 27/08/2020.
//

#ifndef NODENET_THREADSMANAGER_H
#define NODENET_THREADSMANAGER_H

#include <thread>

std::thread pythonThread;
bool pythonThreadIninted = false;
bool pythonThreadAlive   = true;
bool pythonThreadPaused  = false;

#endif //NODENET_THREADSMANAGER_H
