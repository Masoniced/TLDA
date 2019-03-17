# include <iostream>
# include <io.h>
# include <stdlib.h>
# include <cstring>
# include <fstream>
# include "TLDA_read.h"

using namespace std;

typedef map<string, vector< vector<string> > > TOTAL_WORD;

const char dilimiter[] = " ,./~!@#$%^&*-=+\\:';?<>()";

SPARSE_DATA train_sparse_data, test_sparse_data;
COLUMN_INDEX column_index;
static TOTAL_WORD total_word;
static int total_transaction = 0;
static int total_count = 0;

int read_from_text(string const path, string const name) {

	int read_data(string const, string const);
	long handle;
	struct _finddata_t fileinfo;
	string full_path = path + name;
	handle = _findfirst(full_path.c_str(), &fileinfo);
	if (handle == -1)
		return -1;
	do
	{
		int ret;
		string user = fileinfo.name;
		ret = read_data(path, user);

	} while (!_findnext(handle, &fileinfo));

	_findclose(handle);

	return 0;
}


int read_data(string const path, string const name) {

	vector<string> split(string &);
	string full_path = path + name;
	ifstream in(full_path);
	if (!in) {
		cerr << " Can't open input file." << endl;
		return -1;
	}

	string line;
	vector< vector<string> > temp_words;

	while (getline(in, line)) {
		vector<string> words;
		words = split(line);
		if (words.size() > 0) {
			temp_words.push_back(words);
			total_transaction++;
		}
	}
	in.close();

	if (temp_words.size() > 0) {
		total_word.insert(TOTAL_WORD::value_type(name, temp_words));
	}

	return 0;
}


vector <string> split(string &line) {

	void check_upper(char*);
	bool check_key(const string &);
	string words = line;
	vector<string> final_words;

	int length = words.size()+1;
	char * token(new char[length]);
	strcpy(token, words.c_str());
	char *temp_token;
	temp_token = strtok(token, dilimiter);
	while (temp_token != NULL)
	{
		string temp_word;
		check_upper(temp_token);
		temp_word = temp_token;
		if (temp_word.size() > 0) {
			final_words.push_back(temp_word);

			if (check_key(temp_word)) {
				column_index.insert( COLUMN_INDEX::value_type(temp_word, (int)(column_index.size() + 1) ) );
			}

			total_count++;
		}
		
		temp_token = strtok(NULL, dilimiter);
	}

	delete(token);
	return final_words;
}


void check_upper(char *token) {

	char c;
	int i = 0;
	while (token[i])
	{
		c = token[i];
		if (isupper(c)) {
			token[i] = tolower(c);
		}
		i++;
	}
}


bool check_key(const string &key_name) {

	bool ret;
	ret = (column_index.find(key_name) == column_index.end());
	return ret;
}


void convert_sparse(int test_num) {

	int pivot = (int) round(total_transaction / test_num);
	int test_count = 0;

	for (TOTAL_WORD::iterator it = total_word.begin(); it != total_word.end(); it++) {

		// initialize train_data
		SPARSE_LABEL temp_train_sparse;
		vector<int> train_data;
		vector<int> train_indices;
		vector<int> train_indptr;
		train_indptr.push_back(0);

		//initialize test data
		SPARSE_LABEL temp_test_sparse;
		vector<int> test_data;
		vector<int> test_indices;
		vector<int> test_indptr;
		test_indptr.push_back(0);

		int length_transation = total_word.at(it->first).size();
		for (int i = 0; i < length_transation; i++) {

			int temp_word_length = it->second.at(i).size();
			vector<string> same_train_word;

			if (test_count == pivot) {
				vector<string> same_test_word;
				for (int u = 0; u < temp_word_length; u++) {

					if (count(same_test_word.begin(), same_test_word.end(), it->second.at(i).at(u)) == 0) {

						test_data.push_back((int)count(it->second.at(i).begin(), it->second.at(i).end(), it->second.at(i).at(u)));
						test_indices.push_back(column_index.at(it->second.at(i).at(u)));
						same_test_word.push_back(it->second.at(i).at(u));
					}

				}
				test_indptr.push_back((int)(test_indptr.back() + same_train_word.size()));
			}
			for (int j = 0; j < temp_word_length; j++) {

				if (count(same_train_word.begin(), same_train_word.end(), it->second.at(i).at(j)) == 0) {

					train_data.push_back( (int) count(it->second.at(i).begin(), it->second.at(i).end(), it->second.at(i).at(j)) );
					train_indices.push_back( column_index.at(it->second.at(i).at(j)) );
					same_train_word.push_back(it->second.at(i).at(j));
				}

			}
			train_indptr.push_back((int)(train_indptr.back() + same_train_word.size()));
			test_count++;
		}

		if (train_data.size() > 0) {
			temp_train_sparse["data"] = train_data;
			temp_train_sparse["indices"] = train_indices;
			temp_train_sparse["indptr"] = train_indptr;

			train_sparse_data.insert(SPARSE_DATA::value_type(it->first, temp_train_sparse));
		}

		if (test_data.size() > 0) {
			temp_test_sparse["data"] = test_data;
			temp_test_sparse["indices"] = test_indices;
			temp_test_sparse["indptr"] = test_indptr;

			test_sparse_data.insert(SPARSE_DATA::value_type(it->first, temp_test_sparse));
		}
	}
}


int read_process(string const path, string const name, int num_test) {
	int ret;
	ret = read_from_text(path, name);
	if (ret != -1) {
		convert_sparse(num_test);
		return 0;
	}
	else {
		cerr << " Can't read input file." << endl;
		return -1;
	}
}
