#ifndef GIMC_CALCS_H
#define GIMC_CALCS_H

#include <ostream>
#include <vector>

struct SimModel {
	int price, result, ret;
	SimModel(int price, int result, int ret = 0)
	    : price(price), result(result), ret(ret) {}
};
struct ProbModel {
	int price, result, ret, ex;
	double ret_prob, ex_prob;
	ProbModel(int price, int result, int ret = 0, double ret_prob = 0,
	          int ex = 0, double ex_prob = 0)
	    : price(price), result(result), ret(ret), ret_prob(ret_prob), ex(ex),
	      ex_prob(ex_prob) {}
};

double calc(const std::vector<int> &vals, double rate, int depth);
double sim_calc(const std::vector<int> &vals,
                const std::vector<SimModel> &rules, int depth);
double prob_calc(const std::vector<int> &vals, const ProbModel &rule, int depth,
                 double *prob_ptr = nullptr);
void output_info(std::ostream &os, const std::string &title,
                 const std::vector<int> &vals, int depth);

#endif // GIMC_CALCS_H