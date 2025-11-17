#ifndef CONFIG_H
#define CONFIG_H

#include "ECC_Parameter_Set.h"
#include "NVM_Transaction_Flash.h"
#include "Sim_Defs.h"
#include <cstdint>
#include <string>

using namespace std;

// Default config
const string PREFIX_PATH = "./conf/IRS_";

const uint64_t INFORMATION_BITS_8kb = 65536; // bits
const uint64_t PARITY_BITS_8kb = 8192;
const string ALPHA_PARAM_8kb = "0.83;0.43";
const string BETA_PARAM_8kb = "0.43;0.34";
const string SOFT_DECODER_PARAM_8kb = "10;10";
const string HARD_DECODER_PARAM_8kb = "100;100";
const string ENCODER_PARAM_8kb = "2;3;2;8;72;1024";

const uint64_t INFORMATION_BITS_4kb = 32768; // bits
const uint64_t PARITY_BITS_4kb = 4096;
const string ALPHA_PARAM_4kb = "0.83;0.43";
const string BETA_PARAM_4kb = "0.43;0.34";
const string SOFT_DECODER_PARAM_4kb = "10;10";
const string HARD_DECODER_PARAM_4kb = "100;100";
const string ENCODER_PARAM_4kb = "2;3;2;8;72;512";

const uint64_t INFORMATION_BITS_2kb = 16384;
const uint64_t PARITY_BITS_2kb = 2048;
const string ALPHA_PARAM_2kb = "0.83;0.43";
const string BETA_PARAM_2kb = "0.43;0.34";
const string SOFT_DECODER_PARAM_2kb = "10;10";
const string HARD_DECODER_PARAM_2kb = "100;100";
const string ENCODER_PARAM_2kb = "2;3;2;8;72;256";

const uint64_t INFORMATION_BITS_1kb = 8192;
const uint64_t PARITY_BITS_1kb = 1024;
const string ALPHA_PARAM_1kb = "0.83;0.43";
const string BETA_PARAM_1kb = "0.43;0.34";
const string SOFT_DECODER_PARAM_1kb = "10;10";
const string HARD_DECODER_PARAM_1kb = "100;100";
const string ENCODER_PARAM_1kb = "2;3;2;8;72;128";

const uint16_t SSD_INIT_CODEWORD_TYPE_IN_KB = 4; // 1kb, 2kb, 4kb, 8kb, 16kb
const bool ENABLE_FAILURE_AWARE_READ_RETRY = false; // failure aware ecc
const bool ENABLE_FAILURE_AWARE_DECODING = false; // smart reading
const uint16_t ENABLE_OPTIMAL_CODELENGTH = 0; // 0 for Uniform-ECC, 1 for Refresh, 2 for RBER-based, 3 for LAC
const bool ENABLE_BYPASS_READ = false; // skip hard decoding
const uint64_t SSD_INIT_PE_CYCLES = 10000;
const double SSD_INIT_RETENTION_DAYS = 0;
const uint64_t SSD_INIT_READ_TIMES = 0;

// FAST19
// TLC
const double RBER_THETA = 1.48E-03;
const double RBER_ALPHA = 3.90E-10;
const double RBER_BETA = 6.28E-05;
const double RBER_GAMMA = 3.73E-09;
const double RBER_K = 2.05;
const double RBER_M = 0.14;
const double RBER_N = 0.54;
const double RBER_P = 0.33;
const double RBER_Q = 1.71;

// MLC
// const double RBER_THETA = 8.34E-05;
// const double RBER_ALPHA = 3.30E-11;
// const double RBER_BETA = 5.56E-19;
// const double RBER_GAMMA = 6.26E-13;
// const double RBER_K = 1.71;
// const double RBER_M = 2.49;
// const double RBER_N = 3.33;
// const double RBER_P = 1.76;
// const double RBER_Q = 0.47;

// ref: https://docs.amd.com/v/u/en-US/pb052-ldpc , 12.4 Gb/s
// Unit: GB/s
const double ENCODING_THROUGHOUT = 12.4 / 8;

// Smart decoding critical point
const double critical_point_1kb[] = {
    0.0068091278076171866,
    0.006236312866210937,
    0.0059517364501953125,
    0.005793960571289062,
    0.005691574096679686,
    0.005618026733398437,
    0.005561569213867187,
    0.0055168609619140625,
    0.005480697631835937,
    0.005452621459960937,
    0.005430496215820313,
    0.0054129486083984366,
    0.005399063110351562,
    0.0053882293701171865,
    0.005379684448242187,
    0.005373275756835936,
};

const double critical_point_2kb[] = {
    0.006449325561523438,
    0.0060596160888671865,
    0.005871932983398437,
    0.005765426635742187,
    0.005695999145507813,
    0.005646865844726562,
    0.0056102447509765625,
    0.005582321166992187,
};

const double critical_point_4kb[] = {
    0.006233566284179688,
    0.005978286743164063,
    0.005862625122070314,
    0.005800521850585938,
};

const double critical_point_8kb[] = {
    0.005948379516601564,
    0.005785110473632813,
};

// Skip hard decode 
inline bool failure_aware_decoding_skip(double rber, int ncodeword, CodeWordType CodewordType)
{
    if (ncodeword < 1 || ncodeword > 16 || rber == 0)
    {
        PRINT_ERROR("n_info_bits < 1 or n_info_bits > 16!!! or rber == 0");
        exit(-1);
    }
    if (CodewordType == CodeWordType::_1kb){
        return rber > critical_point_1kb[ncodeword - 1];
    }
    else if(CodewordType == CodeWordType::_2kb){
        return rber > critical_point_2kb[ncodeword - 1];
    }
    else if(CodewordType == CodeWordType::_4kb){
        return rber > critical_point_4kb[ncodeword - 1];
    }
    else if(CodewordType == CodeWordType::_8kb){
        return rber > critical_point_8kb[ncodeword - 1];
    }
    else{
       return false;
    }
}

inline bool try_failure_aware_decoding(SSD_Components::NVM_Transaction_Flash *transaction, int ncodeword, CodeWordType CodewordType)
{
    if (transaction->decode_type == SSD_Components::DECODE_TYPE::HARD && ECC_Parameter_Set::enable_failure_aware_decoding &&
        failure_aware_decoding_skip(transaction->rber, ncodeword, CodewordType))
    {
        transaction->decode_type = SSD_Components::DECODE_TYPE::FIRST_SOFT;
        return true;
    }

    return false;
}

inline bool try_optimal_codelength(NVM::FlashMemory::Page *target_page, int ncodeword, CodeWordType CodewordType)
{
    // To be implemented
    return false;
}

inline bool try_bypass_read(SSD_Components::NVM_Transaction_Flash *transaction)
{
    if (transaction->decode_type == SSD_Components::DECODE_TYPE::HARD && ECC_Parameter_Set::enable_bypass_read)
    {
        transaction->decode_type = SSD_Components::DECODE_TYPE::FIRST_SOFT;
        return true;
    }
}

#endif