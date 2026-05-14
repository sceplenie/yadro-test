#include <cmath>
#include <iostream>
#include <chrono>

#include "awgn.h"

AWGN::AWGN(double mean) : mean_(mean) {}

std::vector<std::complex<double>>
AWGN::ApplyNoise(const std::vector<std::complex<double>>& tx_sig, double variance) const {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() ^ (uint64_t)std::random_device{}();
    std::mt19937 rng(seed);
    // Расчет среднеквадратичного отклонения
    double sigma = std::sqrt(variance / 2.0); 
    std::normal_distribution<double> noise(mean_, sigma); 

    // Добавление шума к каждой компоненте сигнала
    std::vector<std::complex<double>> ch_sig(tx_sig.size());
    for (size_t i = 0; i < tx_sig.size(); ++i) {
        const auto value = tx_sig[i] + std::complex<double>(noise(rng), noise(rng));
        ch_sig[i] = value;
    }

    return ch_sig;
}
