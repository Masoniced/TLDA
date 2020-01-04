#ifndef _TLDA_UTL_H
#define _TLDA_UTL_H

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

//Tools

template<typename T> class Distance_correlation {
private:
	std::vector<T> x;
	std::vector<T> y;
public:
	std::vector<std::vector<double>> x_dis_mat;
	std::vector<std::vector<double>> y_dis_mat;
	double final_value;

	double dist_corr();
	Distance_correlation(std::vector<T> X, std::vector<T> Y) : x(X), y(Y) {};
};

template<typename T>
double Distance_correlation<T>::dist_corr()
{
	if (x.size() != y.size()) {
		return -1.0;
	}
	else {
		x_dis_mat.assign(x.size(), std::vector<double>(x.size(), 0));
		y_dis_mat.assign(y.size(), std::vector<double>(y.size(), 0));
		for (size_t i = 0; i < x.size(); i++) {
			for (size_t j = 0; j < y.size(); j++) {
				if ((x_dis_mat.at(i).at(j) == 0) | (x_dis_mat.at(j).at(i) == 0)) {
					x_dis_mat.at(i).at(j) = (double)fabs(x.at(i) - x.at(j));
					x_dis_mat.at(j).at(i) = (double)fabs(x.at(j) - x.at(i));

					y_dis_mat.at(i).at(j) = (double)fabs(y.at(i) - y.at(j));
					y_dis_mat.at(j).at(i) = (double)fabs(y.at(j) - y.at(i));
				}
			}
		}

		int length = x.size();

		std::vector<double> mean_horizontal_x(length);
		std::vector<double> mean_horizontal_y(length);
		std::vector<double> mean_vertical_x(length, 0);
		std::vector<double> mean_vertical_y(length, 0);
		double mean_x;
		double mean_y;

		std::for_each(x_dis_mat.begin(), x_dis_mat.end(),
			[&, length](std::vector<double> &row) {
			std::transform(row.begin(), row.end(), mean_horizontal_x.begin(), mean_horizontal_x.begin(),
				[length](double d1, double d2) {return d1 / length + d2; });
		});

		std::for_each(y_dis_mat.begin(), y_dis_mat.end(),
			[&, length](std::vector<double> &row) {
			std::transform(row.begin(), row.end(), mean_horizontal_y.begin(), mean_horizontal_y.begin(),
				[length](double d1, double d2) {return d1 / length + d2; });
		});

		for (int i = 0; i < length; i++) {
			double temp_x = std::accumulate(x_dis_mat.at(i).begin(), x_dis_mat.at(i).end(), 0.0);
			mean_vertical_x.at(i) = 1.0 * temp_x / length;

			double temp_y = std::accumulate(y_dis_mat.at(i).begin(), y_dis_mat.at(i).end(), 0.0);
			mean_vertical_y.at(i) = 1.0 * temp_y / length;
		}

		mean_x = std::accumulate(mean_vertical_x.begin(), mean_vertical_x.end(), 0.0) / length;
		mean_y = std::accumulate(mean_vertical_y.begin(), mean_vertical_y.end(), 0.0) / length;
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				x_dis_mat.at(i).at(j) = (x_dis_mat.at(i).at(j) - mean_vertical_x.at(i) - mean_horizontal_x.at(j) + mean_x);
				y_dis_mat.at(i).at(j) = (y_dis_mat.at(i).at(j) - mean_vertical_y.at(i) - mean_horizontal_y.at(j) + mean_y);
			}
		}

		double decov2_xy = 0.0;
		double decov2_xx = 0.0;
		double decov2_yy = 0.0;

		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				decov2_xy += x_dis_mat.at(i).at(j) * y_dis_mat.at(i).at(j);
				decov2_xx += x_dis_mat.at(i).at(j) * x_dis_mat.at(i).at(j);
				decov2_yy += y_dis_mat.at(i).at(j) * y_dis_mat.at(i).at(j);
			}
		}

		double sqrt_xx = std::sqrt(decov2_xx);
		double sqrt_yy = std::sqrt(decov2_yy);
		final_value = std::sqrt(decov2_xy) / std::sqrt(sqrt_xx * sqrt_yy);

		return final_value;
	}
}


template<typename T, typename T2>
T extract(T2 const &full, T const &ind) {
	int num_indices = ind.size();
	T target(num_indices);
	for (int i = 0; i < num_indices; i++) {
		target[i] = full[ind[i]];
	}
	return target;
};


template<typename T>
std::vector<size_t> sort_indexes(T const &v) {
	//initialize original index locations
	int num_indices = v.size();
	std::vector<size_t> idx(num_indices);
	std::iota(idx.begin(), idx.end(), 0);

	std::sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) { return v[i1] > v[i2]; });

	return idx;
};


template<typename T1, typename T2>
struct less_second {
	typedef std::pair<T1, T2> type;
	bool operator()(type const &a, type const &b) const {
		return a.second < b.second;
	}
};

#endif

