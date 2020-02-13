#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <io.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <algorithm>
#include "TLDA_read.h"

using namespace std;

typedef unordered_map <string, vector< vector<string> > > TOTAL_WORD;

const char dilimiter[] = " ,./~!@#$%^&*-=+\\:';?<>()";

SPARSE_DATA train_sparse_data;
COLUMN_INDEX column_index;
static TOTAL_WORD total_word;
static int total_transaction = 0;
static int total_count = 0;
INDEX_COLUMN index_words;

int read_from_text(string path, string type_name) {

	void read_data(string, string);
	string inPath = path + type_name;
	string name;

	DIR* dir = opendir(path.c_str());
	if (dir == NULL) {
		return -1;
	}

	struct dirent* d_ent = NULL;
	struct stat s;
	while ((d_ent = readdir(dir)) != NULL) {
		if ((strcmp(d_ent->d_name, ".") != 0) && (strcmp(d_ent->d_name, "..") != 0)) {
			stat(d_ent->d_name, &s);
			if (S_ISDIR(s.st_mode)) {
				string temp_path;
				if (path[path.length() - 1] == '/') {
					temp_path = path;
				}
				else {
					temp_path = path + string("/");
				}
				int ret = read_from_text(temp_path, type_name);
				if (ret == -1) {
					return -1;
				}
			}
			else {
				string temp_path;
				if (path[path.length() - 1] == '/') {
					temp_path = path;
				}
				else {
					temp_path = path + string("/");
				}
				read_data(temp_path, string(d_ent->d_name));
			}
		}
	}
	closedir(dir);
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

void check_in_total_word(const string token) {
	if (column_index.find(token) == column_index.end()) {
		column_index.insert(COLUMN_INDEX::value_type(token, (int)(column_index.size())));
		index_words.insert(INDEX_COLUMN::value_type((int)(index_words.size()), token));
	}
}

vector <string> split(string &line) {

	string words = line;
	vector<string> final_words;

	int length = words.size() + 1;
	unique_ptr<char> token(new char[length]);
	strcpy(token.get(), words.c_str());
	char *temp_token;
	temp_token = strtok(token.get(), dilimiter);
	while (temp_token != NULL)
	{
		string temp_word;
		check_upper(temp_token);
		temp_word = temp_token;
		if (temp_word.size() > 0) {
			final_words.push_back(temp_word);
			check_in_total_word(temp_word);

			total_count++;
		}

		temp_token = strtok(NULL, dilimiter);
	}

	return final_words;
}


void convert_sparse(int test_num) {

	int pivot = (int)round(total_transaction / test_num);
	int test_count = 0;

	for (TOTAL_WORD::iterator it = total_word.begin(); it != total_word.end(); it++) {

		// initialize train_data
		SPARSE_LABEL temp_train_sparse;
		vector<int> train_data;
		vector<int> train_indices;
		vector<int> train_indptr;
		vector<int> is_test;
		train_indptr.push_back(0);

		int length_transation = total_word.at(it->first).size();
		for (int i = 0; i < length_transation; i++) {

			int temp_word_length = it->second.at(i).size();
			vector<string> same_train_word;

			if (test_count == pivot) {
				is_test.push_back(1);
			}
			else {
				is_test.push_back(0);
			}
			for (int j = 0; j < temp_word_length; j++) {

				if (count(same_train_word.begin(), same_train_word.end(), it->second.at(i).at(j)) == 0) {

					train_data.push_back((int)count(it->second.at(i).begin(), it->second.at(i).end(), it->second.at(i).at(j)));
					train_indices.push_back(column_index.at(it->second.at(i).at(j)));
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
			temp_train_sparse["test"] = is_test;

			train_sparse_data.insert(SPARSE_DATA::value_type(it->first, temp_train_sparse));
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
