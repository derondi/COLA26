#ifndef ECC_PARAMETER_SET_H
#define ECC_PARAMETER_SET_H

#include "Parameter_Set_Base.h"
#include "XMLWriter.h"
#include "rapidxml/rapidxml.hpp"
#include <cstdint>

class ECC_Parameter_Set : public Parameter_Set_Base
{
  public:
    static uint64_t information_bits_8kb;
    static uint64_t parity_bits_8kb;
    static std::string ldpc_encoder_param_8kb;
    static std::string ldpc_soft_decoder_param_8kb;
    static std::string ldpc_hard_decoder_param_8kb;
    static std::string msa_alpha_param_8kb;
    static std::string msa_beta_param_8kb;

    static uint64_t information_bits_4kb;
    static uint64_t parity_bits_4kb;
    static std::string ldpc_encoder_param_4kb;
    static std::string ldpc_soft_decoder_param_4kb;
    static std::string ldpc_hard_decoder_param_4kb;
    static std::string msa_alpha_param_4kb;
    static std::string msa_beta_param_4kb;

    static uint64_t information_bits_2kb;
    static uint64_t parity_bits_2kb;
    static std::string ldpc_encoder_param_2kb;
    static std::string ldpc_soft_decoder_param_2kb;
    static std::string ldpc_hard_decoder_param_2kb;
    static std::string msa_alpha_param_2kb;
    static std::string msa_beta_param_2kb;

    static uint64_t information_bits_1kb;
    static uint64_t parity_bits_1kb;
    static std::string ldpc_encoder_param_1kb;
    static std::string ldpc_soft_decoder_param_1kb;
    static std::string ldpc_hard_decoder_param_1kb;
    static std::string msa_alpha_param_1kb;
    static std::string msa_beta_param_1kb;

    static uint16_t ssd_init_codeword_type_in_kb;
    static bool enable_failure_aware_read_retry;
    static uint64_t ssd_init_pe_cycles;
    static double ssd_init_retention_days;
    static uint64_t ssd_init_read_times;
    static bool enable_failure_aware_decoding;
    static uint16_t enable_optimal_codelength;
    static bool enable_bypass_read;

    void XML_serialize(Utils::XmlWriter &xmlwriter);
    void XML_deserialize(rapidxml::xml_node<> *node);
};

#endif