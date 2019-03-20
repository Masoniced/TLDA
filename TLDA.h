#pragma once
# include <vector>
# include <unordered_map>
# include <algorithm>
# include <string>
# include <numeric>


typedef std::unordered_map < std::string, std::vector<int> > SPARSE_LABEL;
typedef std::unordered_map <std::string, SPARSE_LABEL > SPARSE_DATA;
typedef std::unordered_map <std::string, int> COLUMN_INDEX;

extern SPARSE_DATA train_sparse_data;
extern COLUMN_INDEX column_index;

extern int read_process(std::string const path, std::string const name, int num_test);

class INI_PARA {
public:
	double topic_alpha = 0.1;
	double pi_alpha = 1.0;
	double pi_beta = 1.0;
	double feature_alpha = 0.01;
	double bg_alpha = 0.01;

	INI_PARA(double TOPIC_ALPHA, double PI_ALPHA, double PI_BETA, double FEATURE_ALPHA, double BG_ALPHA) :\
		topic_alpha(TOPIC_ALPHA), pi_alpha(PI_ALPHA), pi_beta(PI_BETA), feature_alpha(FEATURE_ALPHA), bg_alpha(BG_ALPHA) {};

	INI_PARA() {};

};


class TLDA {
private:

	class SAT {
	public:
		std::vector<int> z;
		std::vector< std::vector< std::vector<int> > > bg_z;

		SAT() {};
	};

	int num_topic;
	int num_feature;
	int num_iter;
	int num_test;

	double topic_alpha_sum;
	double feature_alpha_sum;
	double bg_alpha_sum;

	std::unordered_map<std::string, SAT> SAT_value;
	std::unordered_map<std::string, std::vector<int> > Topic_sat;
	std::unordered_map<std::string, int> sum_Topic_sat;
	std::vector< std::vector<int> > Topic_feature_sat;
	std::vector<int> sum_Topic_feature_sat;
	std::vector<int> Bg_feature_sat;
	std::vector<int> Bg_sat;
	INI_PARA initial;


public:
	// parameter initialization
	void read_initialization(SPARSE_DATA const &data);
	// update parameters
	double update(SPARSE_DATA const &data);
	// sampling function
	int sample_vec(std::vector<double> const &prob);
	//
	void calculate_z_pi(SPARSE_DATA const &data, std::string const &name, int row);
	//
	void deprocess(SPARSE_DATA const &data, std::string const &name, int row, std::vector<int> const &indptr, int mode);
	//
	void TLDA_MCMC(SPARSE_DATA const &data);
	//
	double logloss_calculate(SPARSE_DATA const &data);

	TLDA(int NUM_TOPIC, int NUM_FEATURE, int NUM_ITER, int NUM_TEST) : \
		num_topic(NUM_TOPIC), num_feature(NUM_FEATURE), num_iter(NUM_ITER), num_test(NUM_TEST) {};

	TLDA(int NUM_TOPIC, int NUM_FEATURE, int NUM_ITER, int NUM_TEST, INI_PARA INITIAL) : \
		num_topic(NUM_TOPIC), num_feature(NUM_FEATURE), num_iter(NUM_ITER), num_test(NUM_TEST), initial(INITIAL) {};

};
