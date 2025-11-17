#include "ECC_Parameter_Set.h"
#include "Config.h"
#include "Sim_Defs.h"
#include "XMLWriter.h"
#include "rapidxml/rapidxml.hpp"
#include <cstdlib>
#include <cstring>
#include <string>

uint64_t ECC_Parameter_Set::information_bits_8kb = INFORMATION_BITS_8kb;
uint64_t ECC_Parameter_Set::parity_bits_8kb = PARITY_BITS_8kb;
std::string ECC_Parameter_Set::ldpc_encoder_param_8kb = ENCODER_PARAM_8kb;
std::string ECC_Parameter_Set::ldpc_soft_decoder_param_8kb = SOFT_DECODER_PARAM_8kb;
std::string ECC_Parameter_Set::ldpc_hard_decoder_param_8kb = HARD_DECODER_PARAM_8kb;
std::string ECC_Parameter_Set::msa_alpha_param_8kb = ALPHA_PARAM_8kb;
std::string ECC_Parameter_Set::msa_beta_param_8kb = BETA_PARAM_8kb;

uint64_t ECC_Parameter_Set::information_bits_4kb = INFORMATION_BITS_4kb;
uint64_t ECC_Parameter_Set::parity_bits_4kb = PARITY_BITS_4kb;
std::string ECC_Parameter_Set::ldpc_encoder_param_4kb = ENCODER_PARAM_4kb;
std::string ECC_Parameter_Set::ldpc_soft_decoder_param_4kb = SOFT_DECODER_PARAM_4kb;
std::string ECC_Parameter_Set::ldpc_hard_decoder_param_4kb = HARD_DECODER_PARAM_4kb;
std::string ECC_Parameter_Set::msa_alpha_param_4kb = ALPHA_PARAM_4kb;
std::string ECC_Parameter_Set::msa_beta_param_4kb = BETA_PARAM_4kb;

uint64_t ECC_Parameter_Set::information_bits_2kb = INFORMATION_BITS_2kb;
uint64_t ECC_Parameter_Set::parity_bits_2kb = PARITY_BITS_2kb;
std::string ECC_Parameter_Set::ldpc_encoder_param_2kb = ENCODER_PARAM_2kb;
std::string ECC_Parameter_Set::ldpc_soft_decoder_param_2kb = SOFT_DECODER_PARAM_2kb;
std::string ECC_Parameter_Set::ldpc_hard_decoder_param_2kb = HARD_DECODER_PARAM_2kb;
std::string ECC_Parameter_Set::msa_alpha_param_2kb = ALPHA_PARAM_2kb;
std::string ECC_Parameter_Set::msa_beta_param_2kb = BETA_PARAM_2kb;

uint64_t ECC_Parameter_Set::information_bits_1kb = INFORMATION_BITS_1kb;
uint64_t ECC_Parameter_Set::parity_bits_1kb = PARITY_BITS_1kb;
std::string ECC_Parameter_Set::ldpc_encoder_param_1kb = ENCODER_PARAM_1kb;
std::string ECC_Parameter_Set::ldpc_soft_decoder_param_1kb = SOFT_DECODER_PARAM_1kb;
std::string ECC_Parameter_Set::ldpc_hard_decoder_param_1kb = HARD_DECODER_PARAM_1kb;
std::string ECC_Parameter_Set::msa_alpha_param_1kb = ALPHA_PARAM_1kb;
std::string ECC_Parameter_Set::msa_beta_param_1kb = BETA_PARAM_1kb;

uint16_t ECC_Parameter_Set::ssd_init_codeword_type_in_kb = SSD_INIT_CODEWORD_TYPE_IN_KB;
bool ECC_Parameter_Set::enable_failure_aware_read_retry = ENABLE_FAILURE_AWARE_READ_RETRY;
uint64_t ECC_Parameter_Set::ssd_init_pe_cycles = SSD_INIT_PE_CYCLES;
double ECC_Parameter_Set::ssd_init_retention_days = SSD_INIT_RETENTION_DAYS;
uint64_t ECC_Parameter_Set::ssd_init_read_times = SSD_INIT_READ_TIMES;
bool ECC_Parameter_Set::enable_failure_aware_decoding = ENABLE_FAILURE_AWARE_DECODING;
uint16_t ECC_Parameter_Set::enable_optimal_codelength = ENABLE_OPTIMAL_CODELENGTH;
bool ECC_Parameter_Set::enable_bypass_read = ENABLE_BYPASS_READ;

void ECC_Parameter_Set::XML_serialize(Utils::XmlWriter &xmlwriter)
{
}

void ECC_Parameter_Set::XML_deserialize(rapidxml::xml_node<> *node)
{
    try
    {
        for (auto param = node->first_node(); param; param = param->next_sibling())
        {
            if (strcmp(param->name(), "information_bits_8kb") == 0)
            {
                information_bits_8kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "parity_bits_8kb") == 0)
            {
                parity_bits_8kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ldpc_encoder_param_8kb") == 0)
            {
                ldpc_encoder_param_8kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_soft_decoder_param_8kb") == 0)
            {
                ldpc_soft_decoder_param_8kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_hard_decoder_param_8kb") == 0)
            {
                ldpc_hard_decoder_param_8kb = param->value();
            }
            else if (strcmp(param->name(), "msa_alpha_param_8kb") == 0)
            {
                msa_alpha_param_8kb = param->value();
            }
            else if (strcmp(param->name(), "msa_beta_param_8kb") == 0)
            {
                msa_beta_param_8kb = param->value();
            }
            else if (strcmp(param->name(), "information_bits_4kb") == 0)
            {
                information_bits_4kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "parity_bits_4kb") == 0)
            {
                parity_bits_4kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ldpc_encoder_param_4kb") == 0)
            {
                ldpc_encoder_param_4kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_soft_decoder_param_4kb") == 0)
            {
                ldpc_soft_decoder_param_4kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_hard_decoder_param_4kb") == 0)
            {
                ldpc_hard_decoder_param_4kb = param->value();
            }
            else if (strcmp(param->name(), "msa_alpha_param_4kb") == 0)
            {
                msa_alpha_param_4kb = param->value();
            }
            else if (strcmp(param->name(), "msa_beta_param_4kb") == 0)
            {
                msa_beta_param_4kb = param->value();
            }
            else if (strcmp(param->name(), "information_bits_2kb") == 0)
            {
                information_bits_2kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "parity_bits_2kb") == 0)
            {
                parity_bits_2kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ldpc_encoder_param_2kb") == 0)
            {
                ldpc_encoder_param_2kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_soft_decoder_param_2kb") == 0)
            {
                ldpc_soft_decoder_param_2kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_hard_decoder_param_2kb") == 0)
            {
                ldpc_hard_decoder_param_2kb = param->value();
            }
            else if (strcmp(param->name(), "msa_alpha_param_2kb") == 0)
            {
                msa_alpha_param_2kb = param->value();
            }
            else if (strcmp(param->name(), "msa_beta_param_2kb") == 0)
            {
                msa_beta_param_2kb = param->value();
            }
            else if (strcmp(param->name(), "information_bits_1kb") == 0)
            {
                information_bits_1kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "parity_bits_1kb") == 0)
            {
                parity_bits_1kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ldpc_encoder_param_1kb") == 0)
            {
                ldpc_encoder_param_1kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_soft_decoder_param_1kb") == 0)
            {
                ldpc_soft_decoder_param_1kb = param->value();
            }
            else if (strcmp(param->name(), "ldpc_hard_decoder_param_1kb") == 0)
            {
                ldpc_hard_decoder_param_1kb = param->value();
            }
            else if (strcmp(param->name(), "msa_alpha_param_1kb") == 0)
            {
                msa_alpha_param_1kb = param->value();
            }
            else if (strcmp(param->name(), "msa_beta_param_1kb") == 0)
            {
                msa_beta_param_1kb = param->value();
            }
            else if (strcmp(param->name(), "ssd_init_codeword_type_in_kb") == 0)
            {
                ssd_init_codeword_type_in_kb = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ssd_init_pe_cycles") == 0)
            {
                ssd_init_pe_cycles = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "ssd_init_retention_days") == 0)
            {
                ssd_init_retention_days = std::stod(param->value());
            }
            else if (strcmp(param->name(), "ssd_init_read_times") == 0)
            {
                ssd_init_read_times = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "enable_failure_aware_read_retry") == 0)
            {
                enable_failure_aware_read_retry = std::stoull(param->value());
            }
            else if (strcmp(param->name(), "enable_failure_aware_decoding") == 0)
            {
                enable_failure_aware_decoding = std::stoi(param->value()) == 0 ? false : true;
            }
            else if (strcmp(param->name(), "enable_optimal_codelength") == 0)
            {
                enable_optimal_codelength = std::stoul(param->value()) == 0 ? false : true;
            }
             else if (strcmp(param->name(), "enable_bypass_read") == 0)
            {
                enable_bypass_read = std::stoi(param->value()) == 0 ? false : true;
            }
            else
            {
                PRINT_ERROR("Unknown xml name");
            }
        }
    }
    catch (...)
    {
        PRINT_ERROR("Error in the ECC Parameter Set")
    }
}