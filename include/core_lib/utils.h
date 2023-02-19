#ifndef GIMC_UTILS_H
#define GIMC_UTILS_H

#include <string>
#include <random>

std::string getNo(int num);
void clearscr();
std::string ftos(double value, int prec);
bool try_stoi(std::string s, int &out);
bool get_boolean(const std::string &str, bool &result);

double randf(std::default_random_engine &rng);
double randf();

std::string trim(std::string s);

#endif // GIMC_UTILS_H