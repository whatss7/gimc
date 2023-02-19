#include "core_lib/parser.h"
#include "core_lib/config.h"
#include "core_lib/utils.h"
#include <sstream>
#include <vector>
using namespace std;

string last_executed;

ParseResult execute_command(string str, bool first_command) {
	for (int i = 0; i < str.length(); i++) {
		if (isupper(str[i])) str[i] = str[i] + 32;
	}
	istringstream is(str);
	string inst;
	is >> inst;
	vector<string> params;
	while (true) {
		string param;
		is >> param;
		if (!is) break;
		params.push_back(param);
	}
	const char *ext_enabled = "Extreme information displayed.";
	const char *ext_disabled = "Extreme information hidden.";
	const char *cmd_enabled = "Command mode on.";
	const char *cmd_disabled = "Command mode off.";
	const char *help =
	    "(sequence of number): Calculate how many material you can get by "
	    "crafting.\n"
	    "                      input format: From highest to lowest, splited "
	    "by ' '/'-'/'+'/etc.\n"
	    "                                    Add zeros if some levels are "
	    "skipped.\n"
	    "                      output format: (common)-(25% return)-(10% "
	    "double)\n"
	    "           clear/clr: Clear the screen.\n"
	    "         exit/quit/q: Quit program.\n"
	    "       command/cmd/c: Toggle command mode.\n"
	    "       extreme/ext/e: Toggle extreme output (default on).\n"
	    "              mode/m: Get current mode.\n"
	    "              fast/f: Toggle fast mode (default on).\n"
	    "                      Fast, but too optimistic.\n"
	    "      simulate/sim/s: Toggle simulating mode (default mode).\n"
	    "                      Pessimistic, more precise compared to fast.\n"
	    "probabilistic/prob/p: Toggle probabilistic mode.\n"
	    "                      Slow, outputs expectation (not materials "
	    "left).\n"
	    "          pc (count): Probabilistic mode simulation count.\n"
	    "   pto (level) (num): Toggle p-to mode. Calculate prob. for goal.\n"
	    "                      Based on probabilistic mode.\n"
	    "      help/h/(empty): Display this message.\n";
	string credit = "Genshin Impact Material Calculator (GIMC)\n" +
	                config.path + "\n" + "Programmed by whatss7";
	const auto incorrect_argc = [](string inst, int provided, int min_arg,
	                               int max_arg) {
		if (min_arg == max_arg)
			return inst + " takes " + ftos(min_arg, 0) + " argument(s), but " +
			       ftos(provided, 0) + " was provided.";
		else if (max_arg - min_arg == 1)
			return inst + " takes " + ftos(min_arg, 0) + " or " +
			       ftos(max_arg, 0) + " argument(s), but " + ftos(provided, 0) +
			       " was provided.";
		else
			return inst + " takes " + ftos(min_arg, 0) + " - " +
			       ftos(max_arg, 0) + " argument(s), but " + ftos(provided, 0) +
			       " was provided.";
	};
	const auto boolean_fail = [](string param) {
		return "Unrecognized boolean value \"" + param + "\"";
	};

	const auto no_argument =
	    [incorrect_argc](const string &inst, const vector<string> params,
	                     const string &success, void (*action)(),
	                     Action success_action = action_skip,
	                     Action fail_action = action_none) {
		    if (params.size() == 0) {
			    if (action) action();
			    return ParseResult(success_action, success);
		    } else {
			    return ParseResult(fail_action,
			                       incorrect_argc(inst, params.size(), 0, 0));
		    }
	    };

	const auto single_boolean = [incorrect_argc](const string &inst,
	                                             const vector<string> params,
	                                             bool &boolean,
	                                             const string &to_true,
	                                             const string &to_false) {
		if (params.size() == 0) {
			boolean = !boolean;
			return ParseResult(action_skip, boolean ? to_true : to_false);
		} else if (params.size() == 1) {
			bool result = false;
			if (get_boolean(params[0], result)) {
				boolean = result;
				return ParseResult(action_skip, boolean ? to_true : to_false);
			} else if (params[0] == "show" || params[0] == "value") {
				return ParseResult(action_skip,
				                   inst + (boolean ? ": true" : ": false"));
			} else {
				return ParseResult(action_none,
				                   "Unrecognized boolean value \"" + params[0] +
				                       "\"");
			}
		} else {
			return ParseResult(action_none,
			                   incorrect_argc(inst, params.size(), 0, 1));
		}
	};

	if (inst == "exit" || inst == "quit" || inst == "q") {
		return no_argument(inst, params, "", nullptr, action_exit, action_none);
	}
	if (inst.empty() && config.interactive && first_command || inst == "help" ||
	    inst == "h") {
		return no_argument(inst, params, help, nullptr);
	}
	if (inst == "gimc") {
		ParseResult result = no_argument(inst, params, credit, nullptr,
		                                 action_skip, action_none);
		result.msg = credit;
		return result;
	}
	if (inst == "extreme" || inst == "ext" || inst == "e") {
		return single_boolean(inst, params, config.detail, ext_enabled,
		                      ext_disabled);
	}
	if (inst == "command" || inst == "cmd" || inst == "c") {
		return single_boolean(inst, params, config.command, cmd_enabled,
		                      cmd_disabled);
	}
	string mode_chg_str = "Mode changed from " + config.get_mode_str() + " to ";
	if (inst == "fast" || inst == "f") {
		return no_argument(inst, params, mode_chg_str + "fast",
		                   []() { config.mode = mode_fast; });
	}
	if (inst == "simulate" || inst == "sim" || inst == "s") {
		return no_argument(inst, params, mode_chg_str + "simulate",
		                   []() { config.mode = mode_sim; });
	}
	if (inst == "probabilistic" || inst == "prob" || inst == "p") {
		return no_argument(inst, params, mode_chg_str + "probabilistic",
		                   []() { config.mode = mode_prob; });
	}
	if (inst == "pto") {
		if (params.size() != 2)
			return ParseResult(action_none,
			                   incorrect_argc(inst, params.size(), 2, 2));
		if (!try_stoi(params[0], config.pto_level))
			return ParseResult(action_none, "Invalid number " + params[0]);
		if (!try_stoi(params[1], config.pto_num))
			return ParseResult(action_none, "Invalid number " + params[1]);
		config.mode = mode_pto;
		return ParseResult(action_skip, mode_chg_str + "p-to " + params[0] +
		                                    " " + params[1]);
	}
	if (inst == "pc") {
		if (params.size() > 1)
			return ParseResult(action_none,
			                   incorrect_argc(inst, params.size(), 0, 1));
		if (params.size() == 0)
			return ParseResult(action_skip, "Current PC is " +
			                                    to_string(config.prob_repeat));
		if (!try_stoi(params[0], config.prob_repeat))
			return ParseResult(action_none, "Invalid number " + params[0]);
		return ParseResult(action_skip, "PC changed to " + params[0]);
	}
	if (inst == "mode" || inst == "m") {
		return no_argument(
		    inst, params, "Current mode is: " + config.get_mode_str(), nullptr);
	}
	if (inst == "clear" || inst == "clr") {
		return no_argument(inst, params, "", clearscr);
	}
	if (config.command) {
		return ParseResult(action_skip,
		                   "Unrecognized instruction \"" + inst + "\"");
	} else if (!inst.empty() && !isdigit(inst[0])) {
		return ParseResult(action_none,
		                   "Unrecognized instruction \"" + inst + "\"");
	} else {
		return ParseResult(action_none, "");
	}
}
