//
//  main.cpp
//  SE-Hack-Day
//
//  Created by Justin Stribling and Jitin Dodd on 2014-11-21.
//  Copyright (c) JJGuitarGroup LTD. All rights reserved.
//

#define _USE_MATH_DEFINES
#include "communications.h"
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <stdio.h>

int totalComputers = -1;
float *angles;
std::clock_t lastActionTime = std::clock();

int numSubscribers() {
    if (totalComputers == -1) {
		std::string str = "redis-cli PUBSUB NUMSUB JJGuitar";
        char *writable = new char[str.size() + 1];
        std::copy(str.begin(), str.end(), writable);
        writable[str.size()] = '\0';
		std::string output = exec(writable);
        delete[] writable;
        std::string lastLine;
        std::istringstream f(output);
        std::string line;
        while (std::getline(f, line))
            lastLine = line;
        return atoi(lastLine.c_str());
    } else
        return totalComputers;
}

std::string exec(char *cmd) {
    std::cout << "Executing " << cmd << "." << std::endl;
    FILE *pipe = _popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    _pclose(pipe);
    return result;
}

void redisPublish(std::string action) {
	std::string str = "redis-cli PUBLISH JJGuitar " + action;
    char *writable = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    exec(writable);
    delete[] writable;
}

//void action(int act) {
//    std::cout << "\n\nIn Action!\n\n" << std::endl;
//    double duration = (std::clock() - lastActionTime) / (double)CLOCKS_PER_SEC;
//    duration *= 100;
//    if (duration > 0.25) {
//		redisPublish("");
//        lastActionTime = std::clock();
//		std::cout << "Action " << std::to_string(act) << std::endl;
//    }
//}

void initialSetup() {
    totalComputers = numSubscribers();
    if (totalComputers == 0) {
        std::cerr << "Error: No subscribed computers to destroy :(" << std::endl;
        std::cin.ignore();
    }
    angles = (float *)malloc(totalComputers * sizeof(float));
    std::cout << std::to_string(totalComputers) << " Subscribers" << std::endl;
    std::cout << "Point at every computer and stretch your fingers in order from left to right" << std::endl;
}

void setAngle(int computer, float yaw) {
    angles[computer] = yaw;
    std::cout << "Setting computer " << computer << " to yaw " << yaw << std::endl;
}