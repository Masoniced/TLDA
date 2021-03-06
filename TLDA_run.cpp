#include "TLDA_process.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

void save_data(TLDA model, int mode) {
	ofstream myfile;
	myfile.open("result.txt");
	myfile << "Topic Feature Words: \r\n";
	myfile << "\r\n";
	for (auto it = model.Topic_feature_words.begin(); it != model.Topic_feature_words.end(); it++) {
		myfile << "Topic Number: " << it->first << "; \r\n";
		int word_length = model.Topic_feature_words.at(it->first).size();
		for (int i = 0; i < word_length; i++) {
			myfile << model.Topic_feature_words.at(it->first).at(i).c_str() << "; ";
		}
		if (mode == 1 || mode == 0) {
			myfile << "Topic Background Distance Correlation: " << model.discorr_bg_topic.at(it->first);
		}
		myfile << "\r\n";
	}

	int bg_word_length = model.Bg_feature_words.size();
	myfile << "\r\n";
	myfile << "Background Words: \r\n";
	myfile << "\r\n";
	for (int i = 0; i < bg_word_length; i++) {
		myfile << model.Bg_feature_words.at(i).c_str() << "; ";
	}
	myfile << "\r\n";
}

int main() {
	string path, type_name, burn_in, num_topic, mode, key_words, diss_length;
	cout << "Data Path Input: " << endl;
	getline(cin, path);
	cout << "Burn In Period: " << endl;
	getline(cin, burn_in);
	cout << "Setting Number of Topics: " << endl;
	getline(cin, num_topic);
	cout << "Setting Correlation Mode (1 for manual, 0 for default): " << endl;
	getline(cin, mode);
	int bi = stoi(burn_in, nullptr);
	int mode_i = stoi(mode, nullptr);
	int topic_n = stoi(num_topic, nullptr);
	type_name = "*.txt";
	int ret = read_process(path, type_name, 100);
	if (ret == -1) {
		cout<<"Can't read the text"<<endl;
		return -1;
	}
	TLDA model(topic_n, column_index.size(), bi, 1000);
	if (mode_i == 1) {
		cout << "Setting Number of Top Key Words in Results: " << endl;
		getline(cin, key_words);
		int num_key_words = stoi(key_words, nullptr);
		cout << "Setting Precision (number of words for analysis) of Distance Correlation in Results: " << endl;
		getline(cin, diss_length);
		int num_diss_corr = stoi(diss_length, nullptr);
		model.final_words_length = num_key_words;
		model.dis_corr_length = num_diss_corr;
		model.TLDA_MCMC(train_sparse_data, 1);
		save_data(model, 1);
	}else {
		model.TLDA_MCMC(train_sparse_data, mode_i);
		save_data(model, mode_i);
	}

	return 0;
}
