#ifndef _TLDA_PROCESS_H
#define _TLDA_PROCESS_H

#include "TLDA_read.h"
#include "TLDA_utl.h"


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
	std::unordered_map<std::string, std::vector<int>> Topic_sat;
	std::unordered_map<std::string, int> sum_Topic_sat;
	std::vector<std::vector<int>> Topic_feature_sat;
	std::vector<int> sum_Topic_feature_sat;
	std::vector<int> Bg_feature_sat;
	std::vector<int> Bg_sat;
	INI_PARA initial;


public:
	// Final result word length and distance correlation length for analysis
	int final_words_length = 30;
	int dis_corr_length = 100;
	//Final Parameters
	std::unordered_map<std::string, std::vector<int>> final_Topic_sat;
	std::vector<std::vector<int>> final_Topic_feature_sat;
	std::vector<int> final_Bg_feature_sat;
	std::vector<int> final_Bg_sat;

	std::unordered_map<int, std::vector<std::string>> Topic_feature_words;
	std::vector<std::string> Bg_feature_words;
	std::vector<double> discorr_bg_topic;

	// Parameter initialization
	void read_initialization(SPARSE_DATA const &data);
	// Update parameters
	double update(SPARSE_DATA const &data);
	// Sampling function
	int sample_vec(std::vector<double> const &prob);
	// Sampling z and pi
	inline void calculate_z_pi(SPARSE_DATA const &data, std::string const &name, int row);
	// Dynamic updating private values
	void deprocess(SPARSE_DATA const &data, std::string const &name, int row, std::vector<int> const &indptr, int mode);
	// Main MCMC
	void TLDA_MCMC(SPARSE_DATA const &data, int mode);
	// Perplexity
	double logloss_calculate(SPARSE_DATA const &data);
	// Get parameters
	void get_final_parameters(int mode);

	TLDA(int NUM_TOPIC, int NUM_FEATURE, int NUM_ITER, int NUM_TEST) : \
		num_topic(NUM_TOPIC), num_feature(NUM_FEATURE), num_iter(NUM_ITER), num_test(NUM_TEST) {};

	TLDA(int NUM_TOPIC, int NUM_FEATURE, int NUM_ITER, int NUM_TEST, INI_PARA INITIAL) : \
		num_topic(NUM_TOPIC), num_feature(NUM_FEATURE), num_iter(NUM_ITER), num_test(NUM_TEST), initial(INITIAL) {};

};

#endif
