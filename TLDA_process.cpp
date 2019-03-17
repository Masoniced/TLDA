# include "TLDA_read.h"
# include <iostream>

using namespace std;

class SAT {
public:
	vector<int> z;
	vector< vector<int> > bg_z;

	SAT() {};
};

class INITIAL {
public:
	double Topic_alpha = 0.1;
	double Bg_alpha = 1.0;
	double Bg_beta = 1.0;
	double Topic_feature_alpha = 0.01;
	double Bg_feature_alpha = 0.01;

	INITIAL(double TOPIC_ALPHA, double BG_ALPHA, double BG_BETA, double TOPIC_FEATURE_ALPHA, double BG_FEATURE_ALPHA) :\
		Topic_alpha(TOPIC_ALPHA), Bg_alpha(BG_ALPHA), Bg_beta(BG_BETA), Topic_feature_alpha(TOPIC_FEATURE_ALPHA), \
		Bg_feature_alpha(BG_FEATURE_ALPHA) {};

	INITIAL() {};

};


class TLDA {
private:

	int Topic_num;
	int Feature_num;
	int num_iter;

	map<string, SAT> SAT_data;
	map<string, vector<int> > Topic_sat;
	map<string, int> sum_Topic_sat;
	vector< vector<int> > Topic_feature_sat;
	vector<int> sum_Topic_feature_sat;
	vector< vector<int> > Bg_feature_sat;
	vector<int> sum_Bg_feature_sat;
	vector<int> Bg_sat;
	INITIAL initial;


public:
	vector<int> s;

};


int main() {
	string path = "C:\\Users\\Mason\\Documents\\Project\\Data\\";
	string name = "*.txt";

	int ret;
	ret = read_process(path, name, 200);
	int s = 1;

	return 0;
}