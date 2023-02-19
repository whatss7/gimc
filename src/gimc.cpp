#include "core_lib/calcs.h"
#include "core_lib/config.h"
#include "core_lib/parser.h"
#include "core_lib/utils.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

Action execute_input(string str, bool first_command = true) {
	string left_str = "";
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == ';') {
			left_str = str.substr(i + 1, str.length() - i - 1);
			str = str.substr(0, i);
		}
	}

	str = trim(str);

	string original_str = str;
	for (int i = 0; i < str.length(); i++) {
		if (!isdigit(str[i])) str[i] = ' ';
	}
	istringstream is(str);
	int tmp = 0;
	vector<int> values;
	while (is >> tmp) values.push_back(tmp);
	
	ParseResult result = execute_command(original_str, first_command);
	if (!result.msg.empty()) cout << result.msg << endl;
	Action action = result.action;
	if (action == action_exit) return action_exit;

	if (action == action_none && !values.empty()) {
		cout << " Sequence: ";
		for (int i = 0; i < values.size(); i++) {
			cout << values[i] << (i == values.size() - 1 ? "" : " ");
		}
		cout << endl;

		if (config.mode == mode_pto) {
			output_info(cout, getNo(config.pto_level), values,
			            config.pto_level - 1);
		} else {
			for (int i = values.size() - 1; i >= 4; i--) {
				output_info(cout, getNo(i + 1), values, i);
			}

			output_info(cout, "EC/WM/4th: ", values, 3);
			output_info(cout, "CM/TM/3rd: ", values, 2);
			output_info(cout, "      2nd: ", values, 1);

			cout << "Conv. 1st: " << ftos(calc(values, 3, -1), 0) << endl;
		}
	}

	Action next_action_result = action;
	if (!left_str.empty()) {
		cout << endl;
		next_action_result = execute_input(left_str, false);
		return next_action_result;
	} else {
		last_executed = original_str;
		return action;
	}
}

int main(int argc, const char **argv) {
	string str;
	config.path = argv[0];
	if (argc != 1) {
		config.interactive = false;
		string input;
		for (int i = 1; i < argc; i++) {
			input += argv[i];
			if (i != argc - 1) input += " ";
		}
		if (execute_input(input) != action_none) {
			cout << "------\nInsructions like \"" << last_executed
			     << "\" are only fully usable in interactive mode.\nTo enter "
			        "interactive mode, launch the program without any "
			        "argument.\n------\n";
		}
		return 0;
	}
	cout << "Genshin Impact Material Calculator (GIMC)\n"
	     << config.path << "\n";
	while (true) {
		cout << ">>> ";
		getline(cin, str);
		Action action = execute_input(str);
		cout << "\n";
		if (action == action_exit) break;
	}
	return 0;
}