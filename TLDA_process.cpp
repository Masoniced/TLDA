#include "TLDA_process.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <string>

using namespace std;


void TLDA::read_initialization(SPARSE_DATA const &data) {
	// initialization the class parameters
	Bg_feature_sat.assign(num_feature, 0);
	Topic_feature_sat.assign(num_topic, vector<int>(num_feature, 0));
	sum_Topic_feature_sat.assign(num_topic, 0);
	Bg_sat.assign(2, 0);

	topic_alpha_sum = initial.topic_alpha * num_topic;
	feature_alpha_sum = initial.feature_alpha * num_feature;
	bg_alpha_sum = initial.bg_alpha * num_feature;

	//initialize the topic probability
	vector<double> raw_prob(num_topic, 1.0 / num_topic);
	vector<double> sum_prob(num_topic);
	partial_sum(raw_prob.begin(), raw_prob.end(), sum_prob.begin());

	//initialize other parameters

	for (auto it = data.begin(); it != data.end(); it++) {

		SAT temp_SAT;
		vector<int> temp_Topic_sat(num_topic, 0);
		int transaction_length = it->second.at("indptr").size() - 1;

		for (int i = 0; i < transaction_length; i++) {

			int word_length = (int)(it->second.at("indptr").at(i + 1) - it->second.at("indptr").at(i));
			vector<int> indptr(word_length);
			iota(begin(indptr), end(indptr), it->second.at("indptr").at(i));
			int sample_z = sample_vec(sum_prob);

			if (sample_z == -1) {
				sample_z = num_topic - 1;
			}

			vector< vector<int> > temp_word_z;

			for (int j = 0; j < word_length; j++) {
				int same_word = (int)it->second.at("data").at(indptr.at(j));
				vector<int> word_z;
				for (int u = 0; u < same_word; u++) {
					random_device rd;
					mt19937 e2(rd());
					uniform_real_distribution<> dist(0, 1);
					double r = dist(e2);

					if (r > 0.5) {

						word_z.push_back(1);
						Bg_sat[0] += 1;
						Bg_feature_sat[(int)it->second.at("indices").at(indptr.at(j))] += 1;
					}
					else {
						word_z.push_back(0);
						//word_z.push_back(1);
						Bg_sat[1] += 1;
						int s = (int)it->second.at("indices").at(indptr.at(j));
						Topic_feature_sat[sample_z][(int)it->second.at("indices").at(indptr.at(j))] += 1;
					}
				}
				if (word_z.size() > 0) {
					temp_word_z.push_back(word_z);
				}
			}
			if (temp_word_z.size() > 0) {

				temp_SAT.bg_z.push_back(temp_word_z);
				temp_SAT.z.push_back(sample_z);
				temp_Topic_sat.at(sample_z) += 1;
			}
		}

		if (temp_SAT.z.size() > 0) {

			SAT_value.insert(unordered_map<string, SAT>::value_type(it->first, temp_SAT));
			Topic_sat.insert(unordered_map<string, vector<int> >::value_type(it->first, temp_Topic_sat));
			int sum_topic = accumulate(temp_Topic_sat.begin(), temp_Topic_sat.end(), 0);
			sum_Topic_sat.insert(unordered_map < string, int>::value_type(it->first, sum_topic));
		}
	}

	for (int s = 0; s < num_topic; s++) {
		int sum_feature = accumulate(Topic_feature_sat.at(s).begin(), Topic_feature_sat.at(s).end(), 0);
		sum_Topic_feature_sat.at(s) = sum_feature;
	}
}


int TLDA::sample_vec(vector<double> const &prob) {
	random_device rd;
	mt19937 e2(rd());
	uniform_real_distribution<> dist(0, 1);
	
	double pivot = dist(e2) * prob.back();

	auto const it = lower_bound(prob.begin(), prob.end(), pivot);
	if (it == prob.end()) { return -1; }
	int index = it - prob.begin();

	return index;
}


double TLDA::update(SPARSE_DATA const &data) {

	for (auto it = data.begin(); it != data.end(); it++) {

		int transaction_length = it->second.at("indptr").size() - 1;
		for (int i = 0; i < transaction_length; i++) {
			calculate_z_pi(data, it->first, i);
		}
	}

	double logloss = logloss_calculate(data);

	return logloss;
}


void TLDA::calculate_z_pi(SPARSE_DATA const &data, string const &name, int row) {

	//sample for z
	vector<double> temp_mat(num_topic, 0);

	int word_length = (int)(data.at(name).at("indptr").at(row + 1) - data.at(name).at("indptr").at(row));
	vector<int> temp_indptr(word_length);
	iota(begin(temp_indptr), end(temp_indptr), data.at(name).at("indptr").at(row));

	// Deprocess subtract
	deprocess(data, name, row, temp_indptr, 0);

	for (int i = 0; i < num_topic; i++) {

		double p1 = (initial.topic_alpha + 1.0*Topic_sat.at(name).at(i)) / (sum_Topic_sat.at(name) + topic_alpha_sum);
		double multiply_p = 1.0;

		for (int j = 0; j < word_length; j++) {

			int same_word = SAT_value.at(name).bg_z.at(row).at(j).size();
			for (int u = 0; u < same_word; u++) {
				if (SAT_value.at(name).bg_z.at(row).at(j).at(u) == 0) {
					multiply_p *= (initial.feature_alpha + 1.0*Topic_feature_sat.at(i).at(data.at(name).at("indices").at(temp_indptr.at(j)))) / \
						(sum_Topic_feature_sat.at(i) + feature_alpha_sum);
				}
			}
		}

		temp_mat.at(i) = p1 * multiply_p;
		vector<double> sum_temp_prob(num_topic);
		partial_sum(temp_mat.begin(), temp_mat.end(), sum_temp_prob.begin());

		int sample_temp_z = sample_vec(sum_temp_prob);
		if (sample_temp_z == -1) { 
			sample_temp_z = num_topic - 1; 
		}
		SAT_value.at(name).z.at(row) = sample_temp_z;

	}

	// sample for pi

	for (int k = 0; k < word_length; k++) {

		int same_word_length = SAT_value.at(name).bg_z.at(row).at(k).size();
		for (int m = 0; m < same_word_length; m++) {

			vector<double> sum_temp_pi_prob(2, 0);
			double p2 = ((1.0*Bg_sat.at(0) + initial.pi_alpha) / (Bg_sat.at(0) + Bg_sat.at(1) + initial.pi_alpha + initial.pi_beta)) * \
				((initial.bg_alpha + 1.0*Bg_feature_sat.at(data.at(name).at("indices").at(temp_indptr.at(k)))) / (bg_alpha_sum + Bg_sat.at(0)));

			double p3 = ((1.0*Bg_sat.at(1) + initial.pi_beta) / (Bg_sat.at(0) + Bg_sat.at(1) + initial.pi_alpha + initial.pi_beta)) * \
				(initial.feature_alpha + 1.0*Topic_feature_sat.at(SAT_value.at(name).z.at(row)).at(data.at(name).at("indices").at(temp_indptr.at(k)))) / \
				(sum_Topic_feature_sat.at(SAT_value.at(name).z.at(row)) + feature_alpha_sum);

			sum_temp_pi_prob.at(0) = p3;
			sum_temp_pi_prob.at(1) = p2 + p3;

			int sample_temp_pi = sample_vec(sum_temp_pi_prob);
			if (sample_temp_pi == -1) {
				sample_temp_pi = 1;
			}
			SAT_value.at(name).bg_z.at(row).at(k).at(m) = sample_temp_pi;
		}
	}
	// Deprocess add
	deprocess(data, name, row, temp_indptr, 1);
}


void TLDA::deprocess(SPARSE_DATA const &data, string const &name, int row, vector<int> const &indptr, int mode) {

	if (mode == 0) {

		Topic_sat.at(name).at(SAT_value.at(name).z.at(row)) -= 1;
		sum_Topic_sat.at(name) -= 1;
		int length = indptr.size();

		for (int i = 0; i < length; i++) {

			int same_word = data.at(name).at("data").at(indptr.at(i));
			for (int j = 0; j < same_word; j++) {

				if (SAT_value.at(name).bg_z.at(row).at(i).at(j) == 1) {
					Bg_sat[0] -= 1;
					Bg_feature_sat.at(data.at(name).at("indices").at(indptr.at(i))) -= 1;
				}
				else {
					Bg_sat[1] -= 1;
					Topic_feature_sat.at(SAT_value.at(name).z.at(row)).at(data.at(name).at("indices").at(indptr.at(i))) -= 1;
					sum_Topic_feature_sat.at(SAT_value.at(name).z.at(row)) -= 1;
				}
			}
		}
	}
	else {

		Topic_sat.at(name).at(SAT_value.at(name).z.at(row)) += 1;
		sum_Topic_sat.at(name) += 1;
		int length = indptr.size();

		for (int i = 0; i < length; i++) {

			int same_word = data.at(name).at("data").at(indptr.at(i));
			for (int j = 0; j < same_word; j++) {

				if (SAT_value.at(name).bg_z.at(row).at(i).at(j) == 1) {
					Bg_sat[0] += 1;
					Bg_feature_sat.at(data.at(name).at("indices").at(indptr.at(i))) += 1;
				}
				else {
					Bg_sat[1] += 1;
					Topic_feature_sat.at(SAT_value.at(name).z.at(row)).at(data.at(name).at("indices").at(indptr.at(i))) += 1;
					sum_Topic_feature_sat.at(SAT_value.at(name).z.at(row)) += 1;
				}
			}
		}
	}
}


double TLDA::logloss_calculate(SPARSE_DATA const &data) {

	double logloss = 0.0;

	for (auto it = data.begin(); it != data.end(); it++) {

		int transaction_length = it->second.at("indptr").size() - 1;
		for (int i = 0; i < transaction_length; i++) {

			int word_length = it->second.at("indptr").at(i + 1) - it->second.at("indptr").at(i);
			vector<int> temp_indptr(word_length);
			iota(begin(temp_indptr), end(temp_indptr), it->second.at("indptr").at(i));
			double temp_topic_p = 0.0;
			for (int j = 0; j < word_length; j++) {

				int same_word = it->second.at("data").at(temp_indptr.at(j));
				double temp = 0.0;
				for (int k = 0; k < same_word; k++) {
					double key_value = 0.0;
					if (SAT_value.at(it->first).bg_z.at(i).at(j).at(k) == 1) {

						key_value = ((initial.topic_alpha + 1.0*Topic_sat.at(it->first).at(SAT_value.at(it->first).z.at(i))) / (sum_Topic_sat.at(it->first) + topic_alpha_sum)) * \
							((initial.bg_alpha + 1.0*Bg_feature_sat.at(data.at(it->first).at("indices").at(temp_indptr.at(j)))) / (bg_alpha_sum + Bg_sat.at(0)));
					}
					else {

						key_value = ((initial.topic_alpha + 1.0*Topic_sat.at(it->first).at(SAT_value.at(it->first).z.at(i))) / (sum_Topic_sat.at(it->first) + topic_alpha_sum)) * \
							(initial.feature_alpha + 1.0*Topic_feature_sat.at(SAT_value.at(it->first).z.at(i)).at(data.at(it->first).at("indices").at(temp_indptr.at(j)))) / \
							(sum_Topic_feature_sat.at(SAT_value.at(it->first).z.at(i)) + feature_alpha_sum);
					}

					if (key_value == 0) {
						temp += 0;
					}
					else {
						temp += log(key_value);
					}
				}
				temp_topic_p += temp;
			}
			logloss += temp_topic_p;
		}
	}

	logloss = logloss / (Bg_sat[0] + Bg_sat[1]);

	return -logloss;
}


void TLDA::TLDA_MCMC(SPARSE_DATA const &data, int mode) {
	read_initialization(train_sparse_data);
	int iteration_count = 0;
	double logloss = 0.0;

	while (iteration_count < num_iter) {

		logloss = update(data);
		iteration_count++;
		printf("--Iteration %d --Perplexity %f \n", iteration_count, logloss);
	}
	get_final_parameters(mode);
}


void TLDA::get_final_parameters(int mode) {

	final_Topic_sat = Topic_sat;
	final_Topic_feature_sat = Topic_feature_sat;
	final_Bg_feature_sat = Bg_feature_sat;
	final_Bg_sat = Bg_sat;

	vector<size_t> temp_topic_feature_word(num_feature);
	vector<size_t> temp_bg_feature_word(num_feature);

	for (int i = 0; i < num_topic; i++) {
		temp_topic_feature_word = sort_indexes(final_Topic_feature_sat);
		vector<string> temp_word(final_words_length);
		for (int j = 0; j < final_words_length; j++) {
			temp_word.at(j) = index_words.at(temp_topic_feature_word.at(j));
		}

		Topic_feature_words.insert(unordered_map<int, vector<string>>::value_type(i, temp_word));
		if (mode == 1) {
			Distance_correlation<int> discorr(final_Topic_feature_sat.at(i), final_Bg_feature_sat);
			double dis_bg_topic = discorr.dist_corr();
			discorr_bg_topic.push_back(dis_bg_topic);
		}
	}

	temp_bg_feature_word = sort_indexes(Bg_feature_sat);
	for (int s = 0; s < final_words_length; s++) {
		string word = index_words.at(temp_bg_feature_word.at(s));
		Bg_feature_words.push_back(word);
	}
}

//int main() {
//	char path[] = "C:\\Users\\Mason\\Documents\\Project\\Data\\";
//	char name[] = "*.txt";
//	int ret;
//	ret = read_from_text(path, name);
//	int s = 1;
//	convert_sparse(300);
//	int ss = 1;
//}
