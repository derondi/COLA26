#ifndef _CONFIG_HH_
#define _CONFIG_HH_

#include "../inc/include.hh"
#include "../util/tinyxml2.h"

using namespace tinyxml2;

class Config
{
public:
    Config(std::string &filepath);
    ~Config();

    long long _information_bits;
    long long _parity_bits;
    std::string _gc_ldpc_encoder_name;
    std::string _gc_ldpc_encoder_param;
    std::string _gc_ldpc_decoder_name;
    std::string _gc_ldpc_decoder_param;
    std::string _gc_failure_model_name;
    std::string _gc_failure_model_param;
    std::string _ldpc_encoder_name;
    std::string _ldpc_encoder_param;
    std::string _failure_model_name;
    std::string _failure_model_param;
    std::string _ldpc_decoder_name;
    std::string _ldpc_decoder_param;
    double _rber_rate;
    double _beta;
    double _alpha;
    long long _iteration;

    std::string _ssd_type;
    long long _ssd_capacity_GB;
    long long _ssd_page_KB;
    long long _ssd_init_pe_cycles;
    long long _ssd_init_retention_days;
    std::string _ssd_rber_model;
    std::string _ssd_errorlog_location;
};

#endif