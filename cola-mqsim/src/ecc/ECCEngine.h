#ifndef ECC_ENGINE_H
#define ECC_ENGINE_H

#include "Decoder.h"
#include "ECC.h"
#include "ECC_Parameter_Set.h"
#include "Encoder.h"
#include "FlashTypes.h"
#include "NVM_PHY_ONFI.h"
#include "NVM_Transaction_Flash.h"
#include "Sim_Object.h"
#include "Sim_Reporter.h"
#include "TSU_Base.h"
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>
#include "Utils.h"

namespace SSD_Components
{

enum class ECCSimEventType : int
{
    DECODE_SUCCEED,
    DECODE_FAILED,
    ENCODE_DATA_ARRIVED,
};

enum class ECCEngineStatus : int
{
    BUSY,
    IDLE,
};

class ECCEngine : public MQSimEngine::Sim_Object, public MQSimEngine::Sim_Reporter
{
  public:
    ECCEngine(const sim_object_id_type &id, uint64_t channel_count, NVM_PHY_ONFI *flash_controller, TSU_Base *tsu,
              const std::string &encoder_param_8kb, const std::string &hard_decoder_param_8kb,
              const std::string &soft_decoder_param_8kb,
              const std::string &encoder_param_4kb, const std::string &hard_decoder_param_4kb,
              const std::string &soft_decoder_param_4kb,
              const std::string &encoder_param_2kb, const std::string &hard_decoder_param_2kb,
              const std::string &soft_decoder_param_2kb,
              const std::string &encoder_param_1kb, const std::string &hard_decoder_param_1kb,
              const std::string &soft_decoder_param_1kb)
        : Sim_Object(id), channel_count(channel_count), encode_status(channel_count, ECCEngineStatus::IDLE),
          decode_status(channel_count, ECCEngineStatus::IDLE), encode_waiting_queues(channel_count),
          decode_waiting_queues(channel_count), flash_controller(flash_controller), tsu(tsu), 
          encoder_8kb(encoder_param_8kb),
          hard_decision_decoder_8kb(hard_decoder_param_8kb),
          soft_decision_decoder_8kb(encoder_8kb, soft_decoder_param_8kb, ECC_Parameter_Set::msa_alpha_param_8kb),
          encoder_4kb(encoder_param_4kb),
          hard_decision_decoder_4kb(hard_decoder_param_4kb),
          soft_decision_decoder_4kb(encoder_4kb, soft_decoder_param_4kb, ECC_Parameter_Set::msa_alpha_param_4kb),
          encoder_2kb(encoder_param_2kb),
          hard_decision_decoder_2kb(hard_decoder_param_2kb),
          soft_decision_decoder_2kb(encoder_2kb, soft_decoder_param_2kb, ECC_Parameter_Set::msa_alpha_param_2kb),
          encoder_1kb(encoder_param_1kb),
          hard_decision_decoder_1kb(hard_decoder_param_1kb),
          soft_decision_decoder_1kb(encoder_1kb, soft_decoder_param_1kb, ECC_Parameter_Set::msa_alpha_param_1kb)
    {
        _my_instance = this;

        // get data buffer and decoding params for 8kb gc-ldpc 
        // 0. get alpha and beta
        vector<string> w_8kb;
        splitString(ECC_Parameter_Set::msa_beta_param_8kb, w_8kb, ";");
        beta_local_8kb = atof(w_8kb[0].c_str());
        beta_global_8kb = atof(w_8kb[1].c_str());
        vector<string> x_8kb;
        splitString(ECC_Parameter_Set::msa_alpha_param_8kb, x_8kb, ";");
        alpha_local_8kb = atof(x_8kb[0].c_str());
        alpha_global_8kb = atof(x_8kb[1].c_str());

        // 1. get parse param for GC
        vector<string> v_8kb;
        splitString(encoder_param_8kb, v_8kb, ";");
        _t_8kb = stoi(v_8kb[0]);
        _m_local_8kb = stoi(v_8kb[1]);
        _m_global_8kb = stoi(v_8kb[2]);
        _m_8kb = stoi(v_8kb[3]);
        _n_8kb = stoi(v_8kb[4]);
        _N_8kb = stoi(v_8kb[5]);
        _n_local_8kb = _n_8kb / _t_8kb;

        // 2. get the number of info bits, parity bits and codeword bits
        M_local_8kb = _m_local_8kb * _N_8kb;
        N_local_8kb = (_n_8kb / _t_8kb) * _N_8kb;
        M_global_8kb = _m_global_8kb * _N_8kb;
        M_8kb = _m_8kb * _N_8kb;
        N_8kb = _n_8kb * _N_8kb;

        // 3. prepare the data buffer
        info_buf_1_8kb.resize(N_local_8kb - M_local_8kb);
        parity_buf_1_8kb.resize(M_local_8kb);
        code_buf_1_8kb.resize(N_local_8kb);
        info_buf_2_8kb.resize(N_local_8kb - M_local_8kb - M_global_8kb);
        parity_buf_2_8kb.resize(M_local_8kb);
        code_buf_2_8kb.resize(N_local_8kb);
        parity_buf_0_8kb.resize(M_global_8kb);
        receive_buf_total_8kb.resize(N_8kb);
        receive_buf_1_8kb.resize(N_local_8kb);
        receive_buf_2_8kb.resize(N_local_8kb);
        corrected_buf_total_8kb.resize(N_8kb);
        corrected_buf_1_8kb.resize(N_local_8kb);
        corrected_buf_2_8kb.resize(N_local_8kb);
        code_buf_total_8kb.resize(N_8kb);

        // 4. random generate the info bits
        for (uint64_t i = 0; i < N_local_8kb - M_local_8kb; i++)
        {
            info_buf_1_8kb[i] = static_cast<unsigned char>(u(e) % 2);
        }
        for (uint64_t i = 0; i < N_local_8kb - M_local_8kb - M_global_8kb; i++)
        {
            info_buf_2_8kb[i] = static_cast<unsigned char>(u(e) % 2);
        }

        // 5. encode the first infomation bits
        encoder_8kb.encode1(info_buf_1_8kb.data(), parity_buf_1_8kb.data());

        // 6. combine the info and parity, get codeword 1 and first part of codeword
        for (uint64_t i = 0; i < N_local_8kb; i++){
            if (i < N_local_8kb - M_local_8kb)
            {
                code_buf_1_8kb[i] = info_buf_1_8kb[i];
            }
            else
            {
                code_buf_1_8kb[i] = parity_buf_1_8kb[i - (N_local_8kb - M_local_8kb)];
            }
            code_buf_total_8kb[i] = code_buf_1_8kb[i];
        }

        // 7. encode the last information bits
        encoder_8kb.encode_2local(code_buf_1_8kb.data(), info_buf_2_8kb.data(), parity_buf_2_8kb.data(), parity_buf_0_8kb.data());

        // 8. combine the info and parity, get codeword 2 and second part of codeword
        for (uint64_t i = 0; i < N_local_8kb - M_local_8kb - M_global_8kb; i++)
        {
            code_buf_2_8kb[i] = info_buf_2_8kb[i];
            code_buf_total_8kb[i + N_local_8kb] = code_buf_2_8kb[i];
        }
        for (uint64_t i = N_local_8kb - M_local_8kb - M_global_8kb; i < N_local_8kb - M_global_8kb; i++)
        {
            code_buf_2_8kb[i] = parity_buf_2_8kb[i - (N_local_8kb - M_local_8kb - M_global_8kb)];
            code_buf_total_8kb[i + N_local_8kb] = code_buf_2_8kb[i];
        }
        for (uint64_t i = N_local_8kb - M_global_8kb; i < N_local_8kb; i++)
        {
            code_buf_2_8kb[i] = parity_buf_0_8kb[i - (N_local_8kb - M_global_8kb)];
            code_buf_total_8kb[i + N_local_8kb] = code_buf_2_8kb[i];
        }

        // get data buffer and decoding params for 4kb gc-ldpc 
        // 0. get alpha and beta
        vector<string> w_4kb;
        splitString(ECC_Parameter_Set::msa_beta_param_4kb, w_4kb, ";");
        beta_local_4kb = atof(w_4kb[0].c_str());
        beta_global_4kb = atof(w_4kb[1].c_str());
        vector<string> x_4kb;
        splitString(ECC_Parameter_Set::msa_alpha_param_4kb, x_4kb, ";");
        alpha_local_4kb = atof(x_4kb[0].c_str());
        alpha_global_4kb = atof(x_4kb[1].c_str());

        // 1. get parse param for GC
        vector<string> v_4kb;
        splitString(encoder_param_4kb, v_4kb, ";");
        _t_4kb = stoi(v_4kb[0]);
        _m_local_4kb = stoi(v_4kb[1]);
        _m_global_4kb = stoi(v_4kb[2]);
        _m_4kb = stoi(v_4kb[3]);
        _n_4kb = stoi(v_4kb[4]);
        _N_4kb = stoi(v_4kb[5]);
        _n_local_4kb = _n_4kb / _t_4kb;

        // 2. get the number of info bits, parity bits and codeword bits
        M_local_4kb = _m_local_4kb * _N_4kb;
        N_local_4kb = (_n_4kb / _t_4kb) * _N_4kb;
        M_global_4kb = _m_global_4kb * _N_4kb;
        M_4kb = _m_4kb * _N_4kb;
        N_4kb = _n_4kb * _N_4kb;

        // 3. prepare the data buffer
        info_buf_1_4kb.resize(N_local_4kb - M_local_4kb);
        parity_buf_1_4kb.resize(M_local_4kb);
        code_buf_1_4kb.resize(N_local_4kb);
        info_buf_2_4kb.resize(N_local_4kb - M_local_4kb - M_global_4kb);
        parity_buf_2_4kb.resize(M_local_4kb);
        code_buf_2_4kb.resize(N_local_4kb);
        parity_buf_0_4kb.resize(M_global_4kb);
        receive_buf_total_4kb.resize(N_4kb);
        receive_buf_1_4kb.resize(N_local_4kb);
        receive_buf_2_4kb.resize(N_local_4kb);
        corrected_buf_total_4kb.resize(N_4kb);
        corrected_buf_1_4kb.resize(N_local_4kb);
        corrected_buf_2_4kb.resize(N_local_4kb);
        code_buf_total_4kb.resize(N_4kb);

        // 4. random generate the info bits
        for (uint64_t i = 0; i < N_local_4kb - M_local_4kb; i++)
        {
            info_buf_1_4kb[i] = static_cast<unsigned char>(u(e) % 2);
        }
        for (uint64_t i = 0; i < N_local_4kb - M_local_4kb - M_global_4kb; i++)
        {
            info_buf_2_4kb[i] = static_cast<unsigned char>(u(e) % 2);
        }

        // 5. encode the first infomation bits
        encoder_4kb.encode1(info_buf_1_4kb.data(), parity_buf_1_4kb.data());

        // 6. combine the info and parity, get codeword 1 and first part of codeword
        for (uint64_t i = 0; i < N_local_4kb; i++){
            if (i < N_local_4kb - M_local_4kb)
            {
                code_buf_1_4kb[i] = info_buf_1_4kb[i];
            }
            else
            {
                code_buf_1_4kb[i] = parity_buf_1_4kb[i - (N_local_4kb - M_local_4kb)];
            }
            code_buf_total_4kb[i] = code_buf_1_4kb[i];
        }

        // 7. encode the last information bits
        encoder_4kb.encode_2local(code_buf_1_4kb.data(), info_buf_2_4kb.data(), parity_buf_2_4kb.data(), parity_buf_0_4kb.data());

        // 8. combine the info and parity, get codeword 2 and second part of codeword
        for (uint64_t i = 0; i < N_local_4kb - M_local_4kb - M_global_4kb; i++)
        {
            code_buf_2_4kb[i] = info_buf_2_4kb[i];
            code_buf_total_4kb[i + N_local_4kb] = code_buf_2_4kb[i];
        }
        for (uint64_t i = N_local_4kb - M_local_4kb - M_global_4kb; i < N_local_4kb - M_global_4kb; i++)
        {
            code_buf_2_4kb[i] = parity_buf_2_4kb[i - (N_local_4kb - M_local_4kb - M_global_4kb)];
            code_buf_total_4kb[i + N_local_4kb] = code_buf_2_4kb[i];
        }
        for (uint64_t i = N_local_4kb - M_global_4kb; i < N_local_4kb; i++)
        {
            code_buf_2_4kb[i] = parity_buf_0_4kb[i - (N_local_4kb - M_global_4kb)];
            code_buf_total_4kb[i + N_local_4kb] = code_buf_2_4kb[i];
        }


        // get data buffer and decoding params for 2kb gc-ldpc 
        // 0. get alpha and beta
        vector<string> w_2kb;
        splitString(ECC_Parameter_Set::msa_beta_param_2kb, w_2kb, ";");
        beta_local_2kb = atof(w_2kb[0].c_str());
        beta_global_2kb = atof(w_2kb[1].c_str());
        vector<string> x_2kb;
        splitString(ECC_Parameter_Set::msa_alpha_param_2kb, x_2kb, ";");
        alpha_local_2kb = atof(x_2kb[0].c_str());
        alpha_global_2kb = atof(x_2kb[1].c_str());

        // 1. get parse param for GC
        vector<string> v_2kb;
        splitString(encoder_param_2kb, v_2kb, ";");
        _t_2kb = stoi(v_2kb[0]);
        _m_local_2kb = stoi(v_2kb[1]);
        _m_global_2kb = stoi(v_2kb[2]);
        _m_2kb = stoi(v_2kb[3]);
        _n_2kb = stoi(v_2kb[4]);
        _N_2kb = stoi(v_2kb[5]);
        _n_local_2kb = _n_2kb / _t_2kb;

        // 2. get the number of info bits, parity bits and codeword bits
        M_local_2kb = _m_local_2kb * _N_2kb;
        N_local_2kb = (_n_2kb / _t_2kb) * _N_2kb;
        M_global_2kb = _m_global_2kb * _N_2kb;
        M_2kb = _m_2kb * _N_2kb;
        N_2kb = _n_2kb * _N_2kb;

        // 3. prepare the data buffer
        info_buf_1_2kb.resize(N_local_2kb - M_local_2kb);
        parity_buf_1_2kb.resize(M_local_2kb);
        code_buf_1_2kb.resize(N_local_2kb);
        info_buf_2_2kb.resize(N_local_2kb - M_local_2kb - M_global_2kb);
        parity_buf_2_2kb.resize(M_local_2kb);
        code_buf_2_2kb.resize(N_local_2kb);
        parity_buf_0_2kb.resize(M_global_2kb);
        receive_buf_total_2kb.resize(N_2kb);
        receive_buf_1_2kb.resize(N_local_2kb);
        receive_buf_2_2kb.resize(N_local_2kb);
        corrected_buf_total_2kb.resize(N_2kb);
        corrected_buf_1_2kb.resize(N_local_2kb);
        corrected_buf_2_2kb.resize(N_local_2kb);
        code_buf_total_2kb.resize(N_2kb);

        // 4. random generate the info bits 
        for (uint64_t i = 0; i < N_local_2kb - M_local_2kb; i++)
        {
            info_buf_1_2kb[i] = static_cast<unsigned char>(u(e) % 2);
        }
        for (uint64_t i = 0; i < N_local_2kb - M_local_2kb - M_global_2kb; i++)
        {
            info_buf_2_2kb[i] = static_cast<unsigned char>(u(e) % 2);
        }

        // 5. encode the first infomation bits
        encoder_2kb.encode1(info_buf_1_2kb.data(), parity_buf_1_2kb.data());

        // 6. combine the info and parity, get codeword 1 and first part of codeword
        for (uint64_t i = 0; i < N_local_2kb; i++)
        {
            if (i < N_local_2kb - M_local_2kb)
            {
                code_buf_1_2kb[i] = info_buf_1_2kb[i];
            }
            else
            {
                code_buf_1_2kb[i] = parity_buf_1_2kb[i - (N_local_2kb - M_local_2kb)];
            }
            code_buf_total_2kb[i] = code_buf_1_2kb[i];
        }

        // 7. encode the last information bits
        encoder_2kb.encode_2local(code_buf_1_2kb.data(), info_buf_2_2kb.data(), parity_buf_2_2kb.data(), parity_buf_0_2kb.data());

        // 8. combine the info and parity, get codeword 2 and second part of codeword
        for (uint64_t i = 0; i < N_local_2kb - M_local_2kb - M_global_2kb; i++)
        {
            code_buf_2_2kb[i] = info_buf_2_2kb[i];
            code_buf_total_2kb[i + N_local_2kb] = code_buf_2_2kb[i];
        }
        for (uint64_t i = N_local_2kb - M_local_2kb - M_global_2kb; i < N_local_2kb - M_global_2kb; i++)
        {
            code_buf_2_2kb[i] = parity_buf_2_2kb[i - (N_local_2kb - M_local_2kb - M_global_2kb)];
            code_buf_total_2kb[i + N_local_2kb] = code_buf_2_2kb[i];
        }
        for (uint64_t i = N_local_2kb - M_global_2kb; i < N_local_2kb; i++)
        {
            code_buf_2_2kb[i] = parity_buf_0_2kb[i - (N_local_2kb - M_global_2kb)];
            code_buf_total_2kb[i + N_local_2kb] = code_buf_2_2kb[i];
        }


        // get data buffer and decoding params for 1kb gc-ldpc
        // 0. get alpha and beta
        vector<string> w_1kb;
        splitString(ECC_Parameter_Set::msa_beta_param_1kb, w_1kb, ";");
        beta_local_1kb = atof(w_1kb[0].c_str());
        beta_global_1kb = atof(w_1kb[1].c_str());
        vector<string> x_1kb;
        splitString(ECC_Parameter_Set::msa_alpha_param_1kb, x_1kb, ";");
        alpha_local_1kb = atof(x_1kb[0].c_str());
        alpha_global_1kb = atof(x_1kb[1].c_str());
        // 1. get parse param for GC
        vector<string> v_1kb;
        splitString(encoder_param_1kb, v_1kb, ";");
        _t_1kb = stoi(v_1kb[0]);
        _m_local_1kb = stoi(v_1kb[1]);
        _m_global_1kb = stoi(v_1kb[2]);
        _m_1kb = stoi(v_1kb[3]);
        _n_1kb = stoi(v_1kb[4]);
        _N_1kb = stoi(v_1kb[5]);
        _n_local_1kb = _n_1kb / _t_1kb;
        // 2. get the number of info bits, parity bits and codeword bits
        M_local_1kb = _m_local_1kb * _N_1kb;
        N_local_1kb = (_n_1kb / _t_1kb) * _N_1kb;
        M_global_1kb = _m_global_1kb * _N_1kb;
        M_1kb = _m_1kb * _N_1kb;
        N_1kb = _n_1kb * _N_1kb;
        // 3. prepare the data buffer
        info_buf_1_1kb.resize(N_local_1kb - M_local_1kb);
        parity_buf_1_1kb.resize(M_local_1kb);
        code_buf_1_1kb.resize(N_local_1kb);
        info_buf_2_1kb.resize(N_local_1kb - M_local_1kb - M_global_1kb);
        parity_buf_2_1kb.resize(M_local_1kb);
        code_buf_2_1kb.resize(N_local_1kb);
        parity_buf_0_1kb.resize(M_global_1kb);
        receive_buf_total_1kb.resize(N_1kb);
        receive_buf_1_1kb.resize(N_local_1kb);
        receive_buf_2_1kb.resize(N_local_1kb);
        corrected_buf_total_1kb.resize(N_1kb);
        corrected_buf_1_1kb.resize(N_local_1kb);
        corrected_buf_2_1kb.resize(N_local_1kb);
        code_buf_total_1kb.resize(N_1kb);
        // 4. random generate the info bits
        for (uint64_t i = 0; i < N_local_1kb - M_local_1kb; i++)
        {
            info_buf_1_1kb[i] = static_cast<unsigned char>(u(e) % 2);
        }
        for (uint64_t i = 0; i < N_local_1kb - M_local_1kb - M_global_1kb; i++)
        {
            info_buf_2_1kb[i] = static_cast<unsigned char>(u(e) % 2);
        }
        // 5. encode the first infomation bits
        encoder_1kb.encode1(info_buf_1_1kb.data(), parity_buf_1_1kb.data());
        // 6. combine the info and parity, get codeword 1 and first part of codeword
        for (uint64_t i = 0; i < N_local_1kb; i++)
        {
            if (i < N_local_1kb - M_local_1kb)
            {
                code_buf_1_1kb[i] = info_buf_1_1kb[i];
            }
            else
            {
                code_buf_1_1kb[i] = parity_buf_1_1kb[i - (N_local_1kb - M_local_1kb)];
            }
            code_buf_total_1kb[i] = code_buf_1_1kb[i];
        }
        // 7. encode the last information bits
        encoder_1kb.encode_2local(code_buf_1_1kb.data(), info_buf_2_1kb.data(), parity_buf_2_1kb.data(), parity_buf_0_1kb.data());
        // 8. combine the info and parity, get codeword 2 and second part of codeword
        for (uint64_t i = 0; i < N_local_1kb - M_local_1kb - M_global_1kb; i++)
        {
            code_buf_2_1kb[i] = info_buf_2_1kb[i];
            code_buf_total_1kb[i + N_local_1kb] = code_buf_2_1kb[i];
        }
        for (uint64_t i = N_local_1kb - M_local_1kb - M_global_1kb; i < N_local_1kb - M_global_1kb; i++)
        {
            code_buf_2_1kb[i] = parity_buf_2_1kb[i - (N_local_1kb - M_local_1kb - M_global_1kb)];
            code_buf_total_1kb[i + N_local_1kb] = code_buf_2_1kb[i];
        }
        for (uint64_t i = N_local_1kb - M_global_1kb; i < N_local_1kb; i++)
        {
            code_buf_2_1kb[i] = parity_buf_0_1kb[i - (N_local_1kb - M_global_1kb)];
            code_buf_total_1kb[i + N_local_1kb] = code_buf_2_1kb[i];
        }
        
        cout << "multi-codeword smart reading = " << ECC_Parameter_Set::enable_failure_aware_decoding << endl;
        cout << "codeword type = " << ECC_Parameter_Set::ssd_init_codeword_type_in_kb << endl;
        cout << "rber-aware encoding = " << ECC_Parameter_Set::enable_optimal_codelength << endl;
        cout << "failure aware ecc = " << ECC_Parameter_Set::enable_failure_aware_read_retry << endl;
        cout << "skip hard decoding = " << ECC_Parameter_Set::enable_bypass_read << endl;
    }

    void Setup_triggers();
    void Validate_simulation_config();
    void Start_simulation();
    void Execute_simulator_event(MQSimEngine::Sim_Event *);

    void Report_results_in_XML(std::string name_prefix, Utils::XmlWriter &xmlwriter);

    typedef void (*TransactionServicedHandlerType)(NVM_Transaction_Flash *);

    void ConnectToTransactionServicedSignal(TransactionServicedHandlerType);
    void broadcastTransactionServicedSignal(NVM_Transaction_Flash *);

    static void handle_read_data_from_PHY(NVM_Transaction_Flash *transaction);
    void submit_write_transactions(const NVM_Transaction_Flash *);

    void SetTSU(TSU_Base *tsu_)
    {
        tsu = tsu_;
    }

    int soft_decision_decode_local_1(uint64_t, CodeWordType CodewordType);
    int soft_decision_decode_local_2(uint64_t, CodeWordType CodewordType);
    int soft_decision_decode_local_t(uint64_t, CodeWordType CodewordType);
    int soft_decision_decode_global(uint64_t, CodeWordType CodewordType);
    int hard_decision_decode_local_1(uint64_t, CodeWordType CodewordType);
    int hard_decision_decode_local_2(uint64_t, CodeWordType CodewordType);
    int hard_decision_decode_local_t(uint64_t, CodeWordType CodewordType);
    int hard_decision_decode_global(uint64_t, CodeWordType CodewordType);

    // Encode write transaction
    void encode();
    static void try_encode()
    {
        _my_instance->encode();
    }

  private:
    static ECCEngine *_my_instance;
    NVM_PHY_ONFI *flash_controller;
    // When decoding failed, submit a transaction to tsu.
    TSU_Base *tsu;
    uint64_t channel_count;

    // ECC engine status
    std::vector<ECCEngineStatus> encode_status;
    std::vector<ECCEngineStatus> decode_status;
    // Per Channel/ECC per encode queue
    std::vector<std::vector<const NVM_Transaction_Flash *>> encode_waiting_queues;
    std::vector<std::queue<NVM_Transaction_Flash *>> decode_waiting_queues;

    // Encoder
    // for 8kb
    IRS encoder_8kb;
    // Hard decision decoder
    GDBF hard_decision_decoder_8kb;
    // Soft decision decoder
    QLMSA soft_decision_decoder_8kb;
    // for 4kb
    IRS encoder_4kb;
    // Hard decision decoder
    GDBF hard_decision_decoder_4kb;
    // Soft decision decoder
    QLMSA soft_decision_decoder_4kb;
    // for 2kb
    IRS encoder_2kb;
    // Hard decision decoder
    GDBF hard_decision_decoder_2kb;
    // Soft decision decoder
    QLMSA soft_decision_decoder_2kb;
    // for 1kb
    IRS encoder_1kb;
    // Hard decision decoder
    GDBF hard_decision_decoder_1kb;
    // Soft decision decoder
    QLMSA soft_decision_decoder_1kb;

    
    // Data Buffer
    // for 4kb
    vector<unsigned char> info_buf_1_8kb;
    vector<unsigned char> parity_buf_1_8kb;
    vector<unsigned char> code_buf_1_8kb;
    vector<unsigned char> info_buf_2_8kb;
    vector<unsigned char> parity_buf_2_8kb;
    vector<unsigned char> code_buf_2_8kb;
    vector<unsigned char> parity_buf_0_8kb;
    vector<double> receive_buf_total_8kb;
    vector<double> receive_buf_1_8kb;
    vector<double> receive_buf_2_8kb;
    vector<unsigned char> corrected_buf_total_8kb;
    vector<unsigned char> corrected_buf_1_8kb;
    vector<unsigned char> corrected_buf_2_8kb;
    vector<unsigned char> code_buf_total_8kb;
    // for 4kb
    vector<unsigned char> info_buf_1_4kb;
    vector<unsigned char> parity_buf_1_4kb;
    vector<unsigned char> code_buf_1_4kb;
    vector<unsigned char> info_buf_2_4kb;
    vector<unsigned char> parity_buf_2_4kb;
    vector<unsigned char> code_buf_2_4kb;
    vector<unsigned char> parity_buf_0_4kb;
    vector<double> receive_buf_total_4kb;
    vector<double> receive_buf_1_4kb;
    vector<double> receive_buf_2_4kb;
    vector<unsigned char> corrected_buf_total_4kb;
    vector<unsigned char> corrected_buf_1_4kb;
    vector<unsigned char> corrected_buf_2_4kb;
    vector<unsigned char> code_buf_total_4kb;
    // for 2kb
    vector<unsigned char> info_buf_1_2kb;
    vector<unsigned char> parity_buf_1_2kb;
    vector<unsigned char> code_buf_1_2kb;
    vector<unsigned char> info_buf_2_2kb;
    vector<unsigned char> parity_buf_2_2kb;
    vector<unsigned char> code_buf_2_2kb;
    vector<unsigned char> parity_buf_0_2kb;
    vector<double> receive_buf_total_2kb;
    vector<double> receive_buf_1_2kb;
    vector<double> receive_buf_2_2kb;
    vector<unsigned char> corrected_buf_total_2kb;
    vector<unsigned char> corrected_buf_1_2kb;
    vector<unsigned char> corrected_buf_2_2kb;
    vector<unsigned char> code_buf_total_2kb;
    // for 1kb
    vector<unsigned char> info_buf_1_1kb;
    vector<unsigned char> parity_buf_1_1kb;
    vector<unsigned char> code_buf_1_1kb;
    vector<unsigned char> info_buf_2_1kb;
    vector<unsigned char> parity_buf_2_1kb;
    vector<unsigned char> code_buf_2_1kb;
    vector<unsigned char> parity_buf_0_1kb;
    vector<double> receive_buf_total_1kb;
    vector<double> receive_buf_1_1kb;
    vector<double> receive_buf_2_1kb;
    vector<unsigned char> corrected_buf_total_1kb;
    vector<unsigned char> corrected_buf_1_1kb;
    vector<unsigned char> corrected_buf_2_1kb;
    vector<unsigned char> code_buf_total_1kb;

    // Matrix Params
    // for 8kb
    int _m_8kb;     // row of exponent p matrix
    int _n_8kb;     // column of exponent p matrix
    int _N_8kb;     // lifting factor of exponent p matrix
    int _t_8kb; //the number of Hlocal matrix
    int _m_local_8kb; // row of local exponent p matrix
    int _m_global_8kb; // row of global exponent p matrix
    int _n_local_8kb; // column of local exponent p matrix
    int M_local_8kb; // number of local parity bits
    int N_local_8kb; // number of local codeword bits
    int M_global_8kb; // number of global parity bits
    int M_8kb; // number of total parity bits
    int N_8kb; // number of total codeword bits
    // for 4kb
    int _m_4kb;     // row of exponent p matrix
    int _n_4kb;     // column of exponent p matrix
    int _N_4kb;     // lifting factor of exponent p matrix
    int _t_4kb; //the number of Hlocal matrix
    int _m_local_4kb; // row of local exponent p matrix
    int _m_global_4kb; // row of global exponent p matrix
    int _n_local_4kb; // column of local exponent p matrix
    int M_local_4kb; // number of local parity bits
    int N_local_4kb; // number of local codeword bits
    int M_global_4kb; // number of global parity bits
    int M_4kb; // number of total parity bits
    int N_4kb; // number of total codeword bits
    // for 2kb
    int _m_2kb;     // row of exponent p matrix
    int _n_2kb;     // column of exponent p matrix
    int _N_2kb;     // lifting factor of exponent p matrix
    int _t_2kb; //the number of Hlocal matrix
    int _m_local_2kb; // row of local exponent p matrix
    int _m_global_2kb; // row of global exponent p matrix
    int _n_local_2kb; // column of local exponent p matrix
    int M_local_2kb; // number of local parity bits
    int N_local_2kb; // number of local codeword bits
    int M_global_2kb; // number of global parity bits
    int M_2kb; // number of total parity bits
    int N_2kb; // number of total codeword bits
    // for 1kb
    int _m_1kb;     // row of exponent p matrix
    int _n_1kb;     // column of exponent p matrix
    int _N_1kb;     // lifting factor of exponent p matrix
    int _t_1kb; //the number of Hlocal matrix
    int _m_local_1kb; // row of local exponent p matrix
    int _m_global_1kb; // row of global exponent p matrix
    int _n_local_1kb; // column of local exponent p matrix 
    int M_local_1kb; // number of local parity bits
    int N_local_1kb; // number of local codeword bits
    int M_global_1kb; // number of global parity bits
    int M_1kb; // number of total parity bits
    int N_1kb; // number of total codeword bits

    // soft decode params
    // for 4kb
    double alpha_local_8kb; // alpha for local lmsa decode
    double alpha_global_8kb; // alpha for global lmsa decode
    double beta_local_8kb; // beta for local lmsa decode
    double beta_global_8kb; // beta for global lmsa decode
    // for 4kb
    double alpha_local_4kb; // alpha for local lmsa decode
    double alpha_global_4kb; // alpha for global lmsa decode
    double beta_local_4kb; // beta for local lmsa decode
    double beta_global_4kb; // beta for global lmsa decode
    // for 2kb
    double alpha_local_2kb; // alpha for local lmsa decode
    double alpha_global_2kb; // alpha for global lmsa decode
    double beta_local_2kb; // beta for local lmsa decode
    double beta_global_2kb; // beta for global lmsa decode
    /// for 1kb
    double alpha_local_1kb; // alpha for local lmsa decode
    double alpha_global_1kb; // alpha for global lmsa decode
    double beta_local_1kb; // beta for local lmsa decode
    double beta_global_1kb; // beta for global lmsa decode

    // Call GC and WL Unit, Address Mapping Unit, Data Cache Manager
    std::vector<TransactionServicedHandlerType> connected_transaction_serviced_handlers;

    // TODO(Stat)
    uint64_t total_decode_times = 0;
    uint64_t decode_times[3]{};
    uint64_t decode_iteration[3]{};
    uint64_t local_decode_iteration[3]{};
    uint64_t global_decode_iteration[3]{};
    uint64_t decode_tECC[3]{};
    uint64_t decode_tECC_tDMA_overlap[3]{};
    uint64_t decode_failure = 0;
    uint64_t onecodeword_decode_failure = 0;
    uint64_t twocodeword_decode_failure = 0;
    uint64_t threecodeword_decode_failure = 0;
    uint64_t fourcodeword_decode_failure = 0;
    uint64_t fivecodeword_decode_failure = 0;
    uint64_t sixcodeword_decode_failure = 0;
    uint64_t sevencodeword_decode_failure = 0;
    uint64_t eightcodeword_decode_failure = 0;
    uint64_t total_read_transaction = 0;
    uint64_t total_read_request_read_transation = 0;
    uint64_t total_partial_read_transaction = 0;
};

} // namespace SSD_Components
#endif