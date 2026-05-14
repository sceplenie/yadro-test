#pragma once

#include <random>
#include <vector>
#include <complex>

class AWGN {
public:
	explicit AWGN(double mean); // Математическое ожидание 

	// Добавляет AWGN к сигналу после модулятора с заданной дисперсией шума
	std::vector<std::complex<double>>
		ApplyNoise(const std::vector<std::complex<double>>& data, double variance) const;

private:
	double mean_ = 0.0; 
};