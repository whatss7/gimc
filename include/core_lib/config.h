#ifndef GIMC_CONFIG_H
#define GIMC_CONFIG_H

#include "core_lib/utils.h"
#include <string>

enum Mode { mode_fast, mode_sim, mode_prob, mode_pto };

struct Config {
	bool detail;
	bool command;
	bool interactive;
	Mode mode;
	std::string path;
	int prob_repeat;
	int pto_level;
	int pto_num;
	Config() {
		detail = true;
		command = false;
		interactive = true;
		mode = mode_sim;
		prob_repeat = 2000;
	}
	std::string get_mode_str() {
		switch (mode) {
		case mode_fast:
			return "fast";
		case mode_sim:
			return "simulate";
		case mode_prob:
			return "probabilistic";
		case mode_pto:
			return "p-to " + ftos(pto_level, 0) + " " + ftos(pto_num, 0);
		default:
			return "";
		}
	}
};

extern Config config;

#endif // GIMC_CONFIG_H