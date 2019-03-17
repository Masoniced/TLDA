#pragma once
# include <vector>
# include <map>
# include <algorithm>
# include <string>


typedef std::map < std::string, std::vector<int> > SPARSE_LABEL;
typedef std::map<std::string, SPARSE_LABEL > SPARSE_DATA;
typedef std::map<std::string, int> COLUMN_INDEX;

extern SPARSE_DATA train_sparse_data, test_sparse_data;
extern COLUMN_INDEX column_index;

extern int read_process(std::string const path, std::string const name, int num_test);