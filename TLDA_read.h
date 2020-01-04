#ifndef _TLDA_READ_H
#define _TLDA_READ_H

#include <vector>
#include <unordered_map>
#include <string>
#include <map>
#include <fstream>
#include <cmath>

typedef std::unordered_map < std::string, std::vector<int> > SPARSE_LABEL;
typedef std::unordered_map <std::string, SPARSE_LABEL > SPARSE_DATA;
typedef std::unordered_map <std::string, int> COLUMN_INDEX;
typedef std::map<int, std::string> INDEX_COLUMN;

extern INDEX_COLUMN index_words;
extern int total_transaction_count;
extern SPARSE_DATA train_sparse_data;
extern COLUMN_INDEX column_index;

extern int read_process(std::string const path, std::string const name, int num_test);

#endif
