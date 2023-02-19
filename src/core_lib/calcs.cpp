#include "core_lib/calcs.h"
#include "core_lib/config.h"
#include "core_lib/utils.h"
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
using namespace std;

double calc(const vector<int> &vals, double rate, int depth) {
	double result = 0;

	double dyn_rate = 1;
	if (depth >= 0) dyn_rate = pow(1 / rate, depth);

	int max_counted = 0;
	if (depth >= 0) max_counted = max(int(vals.size()) - 1 - depth, 0);

	for (int i = vals.size() - 1; i >= max_counted; i--) {
		result += vals[i] * dyn_rate;
		dyn_rate = dyn_rate * rate;
	}
	return result;
}

double sim_calc(const vector<int> &vals, const vector<SimModel> &rules,
                int depth) {
	depth += 1;
	vector<int> nvals;
	for (int i = vals.size(); i < depth; i++) {
		nvals.push_back(0);
	}
	for (int i = max(int(vals.size() - depth), 0); i < vals.size(); i++) {
		nvals.push_back(vals[i]);
	}
	for (int i = nvals.size() - 1; i >= 1; i--) {
		for (int ri = 0; ri < rules.size(); ri++) {
			const SimModel &r = rules[ri];
			while (nvals[i] >= r.price) {
				int craft = nvals[i] / r.price;
				nvals[i - 1] += craft * r.result;
				nvals[i] -= craft * (r.price - r.ret);
			}
		}
	}
	return calc(nvals, 3, depth - 1);
}

double prob_calc(const vector<int> &vals, const ProbModel &rule, int depth,
                 double *prob_ptr) {
	depth += 1;
	bool det = true;
	const double eps = 1e-5;
	int succ = 0;
	ProbModel r = rule;

	if (fabs(rule.ex_prob) < eps) r.ex = 0;
	else if (fabs(rule.ex_prob - 1) >= eps) det = false;
	else r.result += r.ex;

	if (fabs(rule.ret_prob) < eps) r.ret = 0;
	else if (fabs(rule.ret_prob - 1) >= eps) det = false;

	vector<int> nvals_pre;
	default_random_engine rng(0x42);
	for (int i = vals.size(); i < depth; i++) {
		nvals_pre.push_back(0);
	}
	for (int i = max(int(vals.size() - depth), 0); i < vals.size(); i++) {
		nvals_pre.push_back(vals[i]);
	}

	if (det) {
		vector<int> nvals = nvals_pre;
		for (int i = nvals.size() - 1; i >= 1; i--) {
			while (nvals[i] >= r.price) {
				int craft = nvals[i] / r.price;
				nvals[i - 1] += craft * r.result;
				nvals[i] -= craft * (r.price - r.ret);
			}
		}
		if (prob_ptr) {
			if (nvals[0] >= config.pto_num) *prob_ptr = 1;
			else *prob_ptr = 0;
		}
		return nvals[0];
	} else {
		double result = 0;
		for (int i = 0; i < config.prob_repeat; i++) {
			vector<int> nvals = nvals_pre;
			for (int i = nvals.size() - 1; i >= 1; i--) {
				while (nvals[i] >= r.price) {
					nvals[i - 1] += r.result;
					nvals[i] -= r.price;
					if (randf(rng) < r.ret_prob) {
						nvals[i] += r.ret;
					}
					if (randf(rng) < r.ex_prob) {
						nvals[i - 1] += r.ex;
					}
				}
			}
			if (nvals[0] >= config.pto_num) succ += 1;
			result += nvals[0];
		}
		if (prob_ptr) *prob_ptr = double(succ) / config.prob_repeat;
		return result / config.prob_repeat;
	}
}

void output_info(ostream &os, const string &title, const vector<int> &vals,
                 int depth) {
	using CalcHandle =
	    std::function<string(const vector<int> &vals, int depth)>;
	CalcHandle norml = nullptr;
	CalcHandle retnm = nullptr;
	CalcHandle retex = nullptr;
	CalcHandle dblnm = nullptr;
	CalcHandle dblex = nullptr;
	if (config.mode == mode_pto) {
		static const auto f = [](const vector<int> &vals, int depth, const ProbModel &rule) {
			double prob, result;
			result = prob_calc(vals, rule, depth, &prob);
			return ftos(result, 2) + "/" + ftos(prob * 100, 2) + "%";
		};
		norml = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1), depth), 0);
		};
		retnm = [](const vector<int> &vals, int depth) {
			return f(vals, depth, ProbModel(3, 1, 1, 0.25));
		};
		retex = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 1, 1), depth), 0);
		};
		dblnm = [](const vector<int> &vals, int depth) {
			return f(vals, depth, ProbModel(3, 1, 0, 0, 1, 0.1));
		};
		dblex = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 0, 0, 1, 1), depth), 0);
		};
	} else if (config.mode == mode_prob) {
		norml = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1), depth), 0);
		};
		retnm = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 1, 0.25), depth), 2);
		};
		retex = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 1, 1), depth), 0);
		};
		dblnm = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 0, 0, 1, 0.1), depth), 2);
		};
		dblex = [](const vector<int> &vals, int depth) {
			return ftos(prob_calc(vals, ProbModel(3, 1, 0, 0, 1, 1), depth), 0);
		};
	} else if (config.mode == mode_sim) {
		norml = [](const vector<int> &vals, int depth) {
			return ftos(sim_calc(vals, {SimModel(3, 1)}, depth), 2);
		};
		retnm = [](const vector<int> &vals, int depth) {
			return ftos(
			    sim_calc(vals, {SimModel(12, 4, 1), SimModel(3, 1)}, depth), 2);
		};
		retex = [](const vector<int> &vals, int depth) {
			return ftos(sim_calc(vals, {SimModel(3, 1, 1)}, depth), 2);
		};
		dblnm = [](const vector<int> &vals, int depth) {
			return ftos(
			    sim_calc(vals, {SimModel(30, 11), SimModel(3, 1)}, depth), 2);
		};
		dblex = [](const vector<int> &vals, int depth) {
			return ftos(sim_calc(vals, {SimModel(3, 2)}, depth), 2);
		};
	} else {
		norml = [](const vector<int> &vals, int depth) {
			return ftos(calc(vals, 3, depth), 2);
		};
		retnm = [](const vector<int> &vals, int depth) {
			return ftos(calc(vals, 2.75, depth), 2);
		};
		retex = [](const vector<int> &vals, int depth) {
			return ftos(calc(vals, 2, depth), 2);
		};
		dblnm = [](const vector<int> &vals, int depth) {
			return ftos(calc(vals, 3.0 / 1.1, depth), 2);
		};
		dblex = [](const vector<int> &vals, int depth) {
			return ftos(calc(vals, 1.5, depth), 2);
		};
	}
	os << title << norml(vals, depth);
	os << "-" << retnm(vals, depth);
	if (config.detail) os << "(" << retex(vals, depth) << ")";
	os << "-" << dblnm(vals, depth);
	if (config.detail) os << "(" << dblex(vals, depth) << ")";
	os << "\n";
}