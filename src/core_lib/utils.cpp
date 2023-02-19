#include "core_lib/utils.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

string getNo(int num) {
	if (num == 4) return "EC/WM/4th: ";
	if (num == 3) return "CM/TM/3rd: ";
	ostringstream os;
	os << setw(4) << num;
	if ((num % 100) / 10 == 1) os << "th";
	else if (num % 10 == 1) os << "st";
	else if (num % 10 == 2) os << "nd";
	else if (num % 10 == 3) os << "rd";
	else os << "th";
	os << " ? : ";
	return os.str();
}

string ftos(double value, int prec) {
	ostringstream os;
	os << fixed << setprecision(prec) << value;
	return os.str();
}

bool try_stoi(string s, int &out) {
	int result = 0;
	for (int i = 0; i < s.length(); i++) {
		if (!isdigit(s[i])) return false;
		result = result * 10 + s[i] - '0';
	}
	out = result;
	return true;
}

void clearscr() {
	if (system(nullptr)) {
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif
	} else {
		cout << "\033[2J\033[1;1H";
	}
}

bool get_boolean(const string &str, bool &result) {
	const vector<string> trues = {"yes", "y", "true", "t", "enable", "on", "1"};
	const vector<string> falses = {"no",      "n",   "false", "f",
	                               "disable", "off", "0"};
	for (int i = 0; i < trues.size(); i++) {
		if (trues[i] == str) {
			result = true;
			return true;
		}
	}
	for (int i = 0; i < falses.size(); i++) {
		if (falses[i] == str) {
			result = false;
			return true;
		}
	}
	return false;
}

static default_random_engine
    internal_rng(chrono::system_clock::now().time_since_epoch().count());

double randf(default_random_engine &rng) {
	double result = double(rng() - rng.min()) / (rng.max() - rng.min());
	return result;
}

double randf() { return randf(internal_rng); }

string trim(string s){
	int i = 0, j = s.length() - 1;
	for(; i < s.length() && isblank(s[i]); i++);
	for(; j > i && isblank(s[j]); j--);
	return s.substr(i, j - i + 1);
}