//
//  main.cpp
//  SE-Hack-Day
//
//  Created by Justin Stribling and Jitin Dodd on 2014-11-21.
//  Copyright (c) JJGuitarGroup LTD. All rights reserved.
//


#define _USE_MATH_DEFINES
#include <stdio.h>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <stdio.h>
#include <time.h>
#include <dos.h>
#include <windows.h>
#include <myo/myo.hpp>
#include "communications.h"

std::string instrument = "";

class MyoListener : public myo::DeviceListener {
public:
	int pitch_w;
	float pitch_f;
	bool isDown = false;
	bool isUp = false;

	void onOrientationData(myo::Myo *myo, uint64_t timestamp, const myo::Quaternion<float> &quat) {
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		pitch_f = ((asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x())))) + (float)M_PI / 2.0f) / M_PI * 18);
		if (pitch_f < 6){
			if (!isDown && instrument == "strum")
				redisPublish("0");
			if (!isDown && instrument == "drum")
				redisPublish("2");
			isDown = true;
			isUp = false;
		}
		else if (pitch_f > 8){
			if (!isUp && instrument == "strum")
				redisPublish("1");
			isUp = true;
			isDown = false;
		}
	}

	void onPair(myo::Myo *myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
		knownMyos.push_back(myo);
		std::cout << "Paired with " << identifyMyo(myo) << "." << std::endl;
	}

	void onPose(myo::Myo *myo, uint64_t timestamp, myo::Pose pose) {
		std::string poseString = pose.toString();
		if (pose == myo::Pose::fingersSpread)
			instrument = "strum";
		else if (pose == myo::Pose::fist)
			instrument = "drum";
		else
			instrument = "";
	}

	void onConnect(myo::Myo *myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion) {
		std::cout << "Myo " << identifyMyo(myo) << " has connected." << std::endl;
	}

	size_t identifyMyo(myo::Myo *myo) {
		for (size_t i = 0; i < knownMyos.size(); ++i)
			if (knownMyos[i] == myo)
				return i + 1;
		return 0;
	}

	void onDisconnect(myo::Myo *myo, uint64_t timestamp) {
		std::cout << "Myo " << identifyMyo(myo) << " has disconnected." << std::endl;
	}
	std::vector<myo::Myo *> knownMyos;
};

int main(int argc, const char *argv[]) {
	initialSetup();
	try {
		myo::Hub hub("com.hackathon.justinMyo");
		MyoListener listener;
		hub.addListener(&listener);
		while (1)
			hub.run(10);
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
	}
}