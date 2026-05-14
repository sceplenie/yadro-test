#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>

#include "awgn.h"
#include "demodulator.h"
#include "modulator.h"
#include "utils.h"

struct ModulationMode {
    const int value;
    const std::string name;
};

const int FRAME_SIZE = 2400;   
const int MAX_ERRORS = 400;  
const double AWGN_MEAN = 0.0;   
const int MAX_BITS = static_cast<int>(std::pow(10, 5));     

const std::string BER_RESULTS_FILENAME = "ber_results.json";

// Вектор значений дисперсии шума
const std::vector<double> VARIANCES = {                                
    0.5, 0.48, 0.46, 0.44, 0.42, 0.4, 0.38, 0.36, 0.34, 0.32,
    0.3, 0.28, 0.26, 0.24, 0.22, 0.2, 0.18, 0.16, 0.14, 0.12, 0.1
};

const std::vector<ModulationMode> MODULATION_MODES = {             
    {1, "QPSK"},
    {2, "16QAM"},
    {3, "64QAM"},
};

int CountErrors(const std::vector<int>& input,                      
                const std::vector<int>& output,
                int number_of_bits) {
    int result = 0;
    for (int i = 0; i < number_of_bits; ++i)
        if (input[i] != output[i])
            ++result;
    return result;
}

                                                                    
std::pair<int, int> CalculateErrors(double variance, int modulation_mode, int frame_size) {
    int errors = 0;
    int bits = 0;

                                                                    
    while ((errors < MAX_ERRORS) && (bits < MAX_BITS)) {            
       
        const auto input = utils::GenerateRandomBits(frame_size);

        const auto modulator = ModulatorQAM(modulation_mode, frame_size, input);
        const auto tx_sig = modulator.TxSig();

        const auto awgn = AWGN(AWGN_MEAN);
        const auto ch_sig = awgn.ApplyNoise(tx_sig, variance);

        const auto demodulator = DemodulatorQAM(modulation_mode, modulator.GetNumberOfSymbols(), ch_sig);
        const auto output = demodulator.GetBits();

        errors += CountErrors(input, output, static_cast<int>(input.size()));
        bits += FRAME_SIZE;
    }

    return std::make_pair(errors, bits);
}

void WriteJson(const std::string& filename,                          
               const std::map<int, std::map<double, double>>& data) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing\n";
        return;
    }

    out << "{\n";
    bool first_mode = true;

    for (const auto& [mode, variance_map] : data) {
        if (!first_mode) out << ",\n";
        first_mode = false;

        out << "  \"" << mode << "\": {\n";

        bool first_variance = true;
        for (const auto& [variance, ratio] : variance_map) {
            if (!first_variance) out << ",\n";
            first_variance = false;

            out << "    \"" << variance << "\": " << std::setprecision(15) << ratio;
        }
        out << "\n  }";
    }
    out << "\n}\n";
}

int main() {
    std::map<int, std::map<double, double>> results;  

    for (const auto& modulation_mode : MODULATION_MODES) {
        std::cout << "Calculating for modulation mode: " << modulation_mode.name << std::endl;

        for (double variance : VARIANCES) {
            const auto err_bits = CalculateErrors(variance, modulation_mode.value, FRAME_SIZE);

            double ratio = 0.0;
            if (err_bits.second > 0) {
                ratio = static_cast<double>(err_bits.first) / err_bits.second;   
            }

            results[modulation_mode.value][variance] = ratio;

            std::cout << "\n\tVariance = " << variance
                << "\tErrors = " << err_bits.first
                << "\tBits = " << err_bits.second
                << "\tErrors/Bits ratio = " << ratio;
        }

        std::cout << std::endl;
    }

    WriteJson(BER_RESULTS_FILENAME, results);

    std::cout << "\nJSON written to: "
        << std::filesystem::absolute(BER_RESULTS_FILENAME)
        << std::endl;

    return 0;
}