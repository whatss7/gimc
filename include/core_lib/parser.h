#ifndef GIMC_PARSER_H
#define GIMC_PARSER_H

#include <string>

enum Action { action_exit, action_skip, action_none };
struct ParseResult {
	Action action;
	std::string msg;
	ParseResult(Action action, std::string msg) : action(action), msg(msg) {}
};

ParseResult execute_command(std::string str, bool first_command);

extern std::string last_executed;

#endif // GIMC_PARSER_H