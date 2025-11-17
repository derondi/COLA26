#include "ECCEngine.h"
#include "Config.h"
#include "ECC_Parameter_Set.h"
#include "Engine.h"
#include "FailureModel.h"
#include "Flash_Parameter_Set.h"
#include "NVM_PHY_ONFI_NVDDR2.h"
#include "NVM_Transaction.h"
#include "NVM_Transaction_Flash.h"
#include "NVM_Transaction_Flash_RD.h"
#include "NVM_Transaction_Flash_WR.h"
#include "ONFI_Channel_NVDDR2.h"
#include "Sim_Defs.h"
#include "Sim_Object.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

namespace SSD_Components
{

ECCEngine *ECCEngine::_my_instance;

void ECCEngine::Report_results_in_XML(std::string name_prefix, Utils::XmlWriter &xmlwriter)
{
    xmlwriter.Write_attribute_string("total_read_transaction", std::to_string(total_read_transaction));
    xmlwriter.Write_attribute_string("total_read_request_read_transaction",
                                     std::to_string(total_read_request_read_transation));
    xmlwriter.Write_attribute_string("total_partial_read_transaction", std::to_string(total_partial_read_transaction));
    auto write = [&](int idx) {
        std::string tag;
        if (idx == 0)
        {
            tag = "hard";
        }
        else if (idx == 1)
        {
            tag = "first_soft";
        }
        else
        {
            tag = "second_soft";
        }
        xmlwriter.Write_open_tag(tag + "_decode");

        std::string attr = "decode_times";
        std::string val = std::to_string(decode_times[idx]);
        xmlwriter.Write_attribute_string(attr, val);

        attr = "local_decode_iteration";
        val = std::to_string(local_decode_iteration[idx]);
        xmlwriter.Write_attribute_string(attr, val);

        attr = "global_decode_iteration";
        val = std::to_string(global_decode_iteration[idx]);
        xmlwriter.Write_attribute_string(attr, val);

        //attr = "decode_iteration";
        //val = std::to_string(decode_iteration[idx]);
        //xmlwriter.Write_attribute_string(attr, val);

        attr = "tECC";
        val = std::to_string(decode_tECC[idx]);
        xmlwriter.Write_attribute_string(attr, val);

        attr = "tECC_tDMA_overlap";
        val = std::to_string(decode_tECC_tDMA_overlap[idx]);
        xmlwriter.Write_attribute_string(attr, val);

        attr = "overlap_ratio";
        val = std::to_string((double)decode_tECC_tDMA_overlap[idx] / (decode_tECC[idx]+1));
        xmlwriter.Write_attribute_string(attr, val);

        xmlwriter.Write_close_tag();
    };

    auto tag = ID();
    xmlwriter.Write_open_tag(tag);

    std::string attr = "total_decode_times";
    std::string val = std::to_string(total_decode_times);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "total_tECC";
    val = std::to_string(decode_tECC[0] + decode_tECC[1] + decode_tECC[2]);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "total_tECC_tDMA_overlap";
    val = std::to_string(decode_tECC_tDMA_overlap[0] + decode_tECC_tDMA_overlap[1] + decode_tECC_tDMA_overlap[2]);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "overlap_ratio";
    val = std::to_string((double)(decode_tECC_tDMA_overlap[0] + decode_tECC_tDMA_overlap[1] + decode_tECC_tDMA_overlap[2]) /
                         (decode_tECC[0] + decode_tECC[1] + decode_tECC[2]));
    xmlwriter.Write_attribute_string(attr, val);

    attr = "failure";
    val = std::to_string(decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "one_codeword_failure";
    val = std::to_string(onecodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "two_codeword_failure";
    val = std::to_string(twocodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "three_codeword_failure";
    val = std::to_string(threecodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "four_codeword_failure";
    val = std::to_string(fourcodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "five_codeword_failure";
    val = std::to_string(fivecodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "six_codeword_failure";
    val = std::to_string(sixcodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "seven_codeword_failure";
    val = std::to_string(sevencodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    attr = "eight_codeword_failure";
    val = std::to_string(eightcodeword_decode_failure);
    xmlwriter.Write_attribute_string(attr, val);

    write(0);
    write(1);
    write(2);

    xmlwriter.Write_close_tag();
}

void ECCEngine::Setup_triggers()
{
    LOG();
    Sim_Object::Setup_triggers();
}

void ECCEngine::Validate_simulation_config()
{
}

void ECCEngine::Start_simulation()
{
}

void ECCEngine::Execute_simulator_event(MQSimEngine::Sim_Event *ev)
{

    NVM_Transaction_Flash *transaction = (NVM_Transaction_Flash *)ev->Parameters;
    auto channel_id = transaction->Address.ChannelID;

    switch ((ECCSimEventType)ev->Type)
    {
    // Decode
    case ECCSimEventType::DECODE_SUCCEED:
    case ECCSimEventType::DECODE_FAILED:
        // succeed
        if ((ECCSimEventType)ev->Type == ECCSimEventType::DECODE_SUCCEED)
        {
            if (((NVM_Transaction_Flash_RD *)transaction)->RelatedWrite != NULL)
            {
                // This is a partial read
                total_partial_read_transaction += 1;
                ((NVM_Transaction_Flash_RD *)transaction)->RelatedWrite->RelatedRead = NULL;
                encode();
            }
            else
            {
                total_read_request_read_transation += 1;
            }
            total_read_transaction += 1;
            broadcastTransactionServicedSignal(transaction);
        }
        // failed
        else
        {
            LOG("decode failed %d times", (int)transaction->decode_type + 1);
            if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
            {
                // third read decode failed
                if (((NVM_Transaction_Flash_RD *)transaction)->RelatedWrite != NULL)
                {
                    // This is a partial read
                    total_partial_read_transaction += 1;

                    ((NVM_Transaction_Flash_RD *)transaction)->RelatedWrite->RelatedRead = NULL;
                    encode();
                }
                else
                {
                    total_read_request_read_transation += 1;
                }

                total_read_transaction += 1;
                broadcastTransactionServicedSignal(transaction);
            }
            else if (transaction->decode_type == DECODE_TYPE::HARD)
            {
                transaction->decode_type = DECODE_TYPE::FIRST_SOFT;
                tsu->Prepare_for_transaction_submit();
                tsu->Submit_transaction(transaction);
                tsu->Schedule();
            }
            else if (transaction->decode_type == DECODE_TYPE::FIRST_SOFT)
            {
                transaction->decode_type = DECODE_TYPE::SECOND_SOFT;
                tsu->Prepare_for_transaction_submit();
                tsu->Submit_transaction(transaction);
                tsu->Schedule();
            }
            else
            {
                assert(0);
            }
        }

        // ECC decode done, set ECC engine idle.
        decode_status[channel_id] = ECCEngineStatus::IDLE;

        if (!decode_waiting_queues[channel_id].empty())
        {
            // Do decode
            auto elem = decode_waiting_queues[channel_id].front();
            decode_waiting_queues[channel_id].pop();

            handle_read_data_from_PHY(elem);
        }
        break;

    // Encode
    case ECCSimEventType::ENCODE_DATA_ARRIVED:
        LOG("encode");
        encode_status[channel_id] = ECCEngineStatus::IDLE;
        tsu->Prepare_for_transaction_submit();
        tsu->Submit_transaction(transaction);
        tsu->Schedule();

        encode();
        break;
    }
}

void ECCEngine::broadcastTransactionServicedSignal(NVM_Transaction_Flash *transaction)
{
    LOG();

    for (auto const &it : connected_transaction_serviced_handlers)
    {
        (*it)(transaction);
    }

    delete transaction;
}

void ECCEngine::ConnectToTransactionServicedSignal(TransactionServicedHandlerType function)
{
    connected_transaction_serviced_handlers.push_back(function);
}

/**
 *     ┌─────────┬─────────┬─────────┬─────────┐
 *     │  tDMA   │  tDMA   │  tDMA   │  tDMA   │
 *     └─────────┼─────────┴─┬─────┬─┼─────────┴┬────────┐
 *               │   tECC    │tECC │ │  tECC    │ tECC   │
 *               └───────────┴─────┘ └──────────┴────────┘
 *
 */
// Decoding
void ECCEngine::handle_read_data_from_PHY(NVM_Transaction_Flash *transaction)
{
    // judge the codowordtype
    ECCMetadata metadata = ((NVM_PHY_ONFI_NVDDR2 *)_my_instance->flash_controller)->GetECCMetadata(transaction->Address.ChannelID,
        transaction->Address.ChipID, transaction->Address.DieID, transaction->Address.PlaneID, transaction->Address.BlockID,
        transaction->Address.PageID);
    // get codeword type and codeword number(the type that info bits and parity bits are stored)
    CodeWordType CodewordType = metadata.CodewordType;
    uint16_t CodewordNumber = metadata.CodewordNumber;

    // std::string str;
    // int i = 0;
    // uint64_t total = 0;
    // for (const auto &q : _my_instance->waiting_queues)
    // {
    //     str += std::to_string(i) + " -> " + std::to_string(q.size()) + ", ";
    //     total += q.size();
    //     i++;
    // }
    // LOG("ECC Engine total: %lu, %s", total, str.c_str());

    uint64_t channel_id = transaction->Address.ChannelID;

    if (_my_instance->decode_status[channel_id] == ECCEngineStatus::BUSY)
    {
        // The ECC Engine is busy, push transaction to the waiting list.
        _my_instance->decode_waiting_queues[channel_id].push(transaction);
        LOG("waiting...");

        return;
    }

    const auto factor = transaction->decode_type == DECODE_TYPE::HARD ? 1 : 2;
    // Set ECC engine busy and then start doing ECC
    _my_instance->decode_status[channel_id] = ECCEngineStatus::BUSY;
    // Sometime the `Data_and_metadata_size_in_byte` maybe 0, so `n_pages` maybe 0
    const uint64_t ncodeword = transaction->nocodeword(CodewordNumber);
    //const uint64_t ncodeword = std::max(1, transaction->ncodeword());
    const uint64_t tDMA_per_codeword = 
	    NVDDR2DataOutTransferTime((double)Flash_Parameter_Set::Page_Capacity / ((8.0 / 9.0) * metadata.CodewordNumber),
                                  ((NVM_PHY_ONFI_NVDDR2 *)_my_instance->flash_controller)->Get_channel(channel_id)) * 
        factor;
    const uint64_t datasize_per_codeword = (double)Flash_Parameter_Set::Page_Capacity / metadata.CodewordNumber;
    const uint64_t data_and_metadata_size_in_byte = transaction->Data_and_metadata_size_in_byte;
    /*
    const uint64_t tDMA_per_codeword =
        NVDDR2DataOutTransferTime((double)ECC_Parameter_Set::information_bits / 8 +
                                      (double)ECC_Parameter_Set::parity_bits / 8,
                                  ((NVM_PHY_ONFI_NVDDR2 *)_my_instance->flash_controller)->Get_channel(channel_id)) *
        factor;
    */
    // 1. get rber
    //    1) 1st read use hard decision decoder, 2nd and 3rd read use soft decision decoder
    //    2) rber(1st read) == rber(2nd read)
    //    3) rber(2nd read) = 2 * rber(3rd read)
    double rber = transaction->rber;

    // Total ECC time
    uint64_t total_tECC = 0;
    uint64_t total_iter = 0;
    uint64_t total_local_iter = 0;
    uint64_t total_global_iter = 0;

    // Next info bit arrive time
    uint64_t next_codeword_arrive_time = Simulator->Time();
    uint64_t timestamp = next_codeword_arrive_time;
    uint64_t decode_start_time = Simulator->Time();
    uint64_t is_failed = false;
    uint64_t codeword_fail_time = 0;
    uint64_t final_codeword_fail_time = 0;

    // start to do the docoding for 1kb, 2kb, 4kb, 8kb (16kb not used)
    switch (CodewordType)
    {
    // start to do the decoding for 1kb
    case CodeWordType::_1kb:
    {
    // if rber >= 0.008 and first read, skipping
    if (rber >= 0.009 && transaction->decode_type == DECODE_TYPE::HARD)
    { 
        // 1kb codeword hard decode max 8us
        uint64_t tECC = 8 * 1000;
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        total_local_iter = 0;
        total_global_iter = 200 * ncodeword;
        goto end;
    }
    
    // if rber >= 0.020 and first soft decode, skipping soft decode
    if (rber >= 0.022 && transaction->decode_type == DECODE_TYPE::FIRST_SOFT)
    {
        // 1kb codeword soft decode max 12us
        uint64_t tECC = 12 * 1000;
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        total_local_iter = 0;
        total_global_iter = 20 * ncodeword;
        goto end;
    }

    // if third read, rber /= 2
    if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
    {
        rber /= 2;
    }

    for (uint64_t i = 0; i < ncodeword; ++i)
    {
        //1. prepare data buffer for gc
        LOG("next info bits arrive time=%lu, timestamp=%lu", next_info_bits_arrive_time, timestamp);
        uint64_t tECC = 0;
        uint64_t local_tECC = 0;
        uint64_t global_tECC = 0;
        int local_iterations = 0; // not used for global only
        int global_iterations = -1;

        // 2. create the failure model
        Gaussian failure_model = Gaussian(ECC_Parameter_Set::information_bits_1kb, ECC_Parameter_Set::parity_bits_1kb, rber);

        // 3. generate the failures
        failure_model.generateFailure(_my_instance->code_buf_total_1kb.data(), _my_instance->receive_buf_total_1kb.data(), _my_instance->N_1kb);

        // 4. desemble the codeword, no need for gloabl only
        /*
        for (uint64_t j = 0; j < _my_instance->N_1kb; j++)
        {
            if (j < _my_instance->N_local_1kb)
            {
                _my_instance->receive_buf_1_1kb[j] = _my_instance->receive_buf_total_1kb[j];
            }
            else
            {
                _my_instance->receive_buf_2_1kb[j - _my_instance->N_local_1kb] = _my_instance->receive_buf_total_1kb[j];
            }
        }
        */
        // 5. decode
        // TODO(iteration latency config)
        if (transaction->decode_type == DECODE_TYPE::HARD)
        // do hard decode
        {
            // 5.1 local matrix hard decode, no need for global only
            /*
            LOG("hard bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.0035, skip local matrix hard decode, only do local matrix hard decode when rber < 0.0035
            if(rber <= 0.0055)
            {
                decode_iter_local_1 = _my_instance->hard_decision_decode_local_1(channel_id, CodewordType);
                decode_iter_local_2 = _my_instance->hard_decision_decode_local_t(channel_id, CodewordType);
            }

            // 5.2 global matrix hard decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("hard decode local failed");
                LOG("start global matrxi decoding...");
                local_iterations = 100;
                global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
            }
            else
            {
                LOG("hard decode local succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */
            // start global decode directly
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
            // 5.3 check decode result and compute tECC
            if (global_iterations == -1) // means global decode failed
            {
                LOG("global matrix decode failed");
                // https://files.futurememorystorage.com/proceedings/2019/08-07-Wednesday/20190807_CTRL-202-1_Kuo.pdf
                local_tECC = 0;
                global_tECC = 8 * 1000;
                tECC = local_tECC + global_tECC;
                // TODO
                //total_local_iter += 100;
                total_local_iter = 0;
                total_global_iter += 200;
                codeword_fail_time++;
            }
            else
            {
                LOG("global matrix decode succeed");
                local_tECC = 0;
                global_tECC = static_cast<uint64_t>(static_cast<double>(8000) / 200 * global_iterations);
                tECC = local_tECC + global_tECC;
                total_local_iter = 0;
                total_global_iter += global_iterations;
            }
        }
        else
        // do soft decode
        {
            // 5.1 second read or third read, use soft bit decode
            /*
            LOG("soft bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.012, skip local matrix soft decode, only do local matrix soft decode when rber < 0.012
            if (rber < 0.0155)
            {
                int decode_iter_local_1 = _my_instance->soft_decision_decode_local_1(channel_id, CodewordType);
                int decode_iter_local_2 = _my_instance->soft_decision_decode_local_t(channel_id, CodewordType);
            }

            // 5.2 global matrix soft decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("soft local decode failed");
                LOG("start global matrxi decoding...");
                local_iterations = 10;
                global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            }
            else 
            {
                LOG("soft local decode succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            if (global_iterations == -1)
            {
                LOG("global decode failed");
                // TODO
                local_tECC = 0;
                global_tECC = 12 * 1000;
                tECC = local_tECC + global_tECC;
                total_local_iter = 0;
                total_global_iter += 20;
                codeword_fail_time++;
                if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
                {
                    _my_instance->decode_failure++;
                    final_codeword_fail_time++;
                }
            }
            else
            {
                LOG("decode succeed");
                //local_tECC = local_iterations * 93.75;
                local_tECC = 0;
                global_tECC = global_iterations * 600;
                tECC = local_tECC + global_tECC;
                //total_local_iter += local_iterations;
                total_local_iter = 0;
                total_global_iter += global_iterations;
            }
        }

        LOG("rber=%lf,  local_iter = %d , global_iter = %d , tECC=%luns", rber, local_iterations, global_iterations, tECC);

        total_tECC += tECC;

        is_failed = is_failed || (global_iterations != -1 ? false : true);

        if (i == ncodeword - 1)
        {
            // Last info bits
            timestamp += tECC;
        }
        else
        {
            // Update next info bits arrive time
            next_codeword_arrive_time += tDMA_per_codeword;
            // Update next do ECC time
            timestamp = std::max(timestamp + tECC, next_codeword_arrive_time);
        }
    }
    goto end;
    }

    // start to do the decoding for 2kb
    case CodeWordType::_2kb:
        {
        
        // if rber >= 0.008 and first read, skipping
        if (rber >= 0.009 && transaction->decode_type == DECODE_TYPE::HARD)
        {
            // 2kb hard decode max 16 us
            uint64_t tECC = 16 * 1000;
            total_tECC = tECC * ncodeword;
            is_failed = true;
            if (tDMA_per_codeword > tECC)
            {
                timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
            }
            else
            {
                timestamp += total_tECC;
            }
            // TODO
            total_local_iter = 0;
            total_global_iter = 200 * ncodeword;
            goto end;
        }
        
        // if rber >= 0.020 and first soft decode, skipping soft decode
        if (rber >= 0.0205 && transaction->decode_type == DECODE_TYPE::FIRST_SOFT)
        {
            // 2kb soft decode max 24 us
            uint64_t tECC =  24 * 1000;
            total_tECC = tECC * ncodeword;
            is_failed = true;
            if (tDMA_per_codeword > tECC)
            {
                timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
            }
            else
            {
                timestamp += total_tECC;
            }
            // TODO
            total_local_iter = 0;
            total_global_iter = 20 * ncodeword;
            goto end;
        }
    
        // if third read, rber /= 2
        if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
        {
            rber /= 2;
        }
    
        for (uint64_t i = 0; i < ncodeword; ++i)
        {
            //1. prepare data buffer for gc
            LOG("next info bits arrive time=%lu, timestamp=%lu", next_info_bits_arrive_time, timestamp);
            uint64_t tECC = 0;
            uint64_t local_tECC = 0;
            uint64_t global_tECC = 0;
            int local_iterations = 0; // no need for global only
            int global_iterations = -1;
    
            // 2. create the failure model
            Gaussian failure_model = Gaussian(ECC_Parameter_Set::information_bits_2kb, ECC_Parameter_Set::parity_bits_2kb, rber);
    
            // 3. generate the failures
            failure_model.generateFailure(_my_instance->code_buf_total_2kb.data(), _my_instance->receive_buf_total_2kb.data(), _my_instance->N_2kb);
    
            // 4. desemble the codeword
            // no need for global only
            /*
            for (uint64_t j = 0; j < _my_instance->N_2kb; j++)
            {
                if (j < _my_instance->N_local_2kb)
                {
                    _my_instance->receive_buf_1_2kb[j] = _my_instance->receive_buf_total_2kb[j];
                }
                else
                {
                    _my_instance->receive_buf_2_2kb[j - _my_instance->N_local_2kb] = _my_instance->receive_buf_total_2kb[j];
                }
            }
            */
            // 5. decode
            // TODO(iteration latency config)
            if (transaction->decode_type == DECODE_TYPE::HARD)
            // do hard decode
            {
                /*
                // 5.1 local matrix hard decode
                LOG("hard bit decoding...");
                LOG("start local matrxi decoding...");
                int decode_iter_local_1 = -1;
                int decode_iter_local_2 = -1;
                // if  rber >= 0.0035, skip local matrix hard decode, only do local matrix hard decode when rber < 0.0035
                if(rber <= 0.0045)
                {
                    decode_iter_local_1 = _my_instance->hard_decision_decode_local_1(channel_id, CodewordType);
                    decode_iter_local_2 = _my_instance->hard_decision_decode_local_t(channel_id, CodewordType);
                }
    
                // 5.2 global matrix hard decode
                if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
                {
                    LOG("hard decode local failed");
                    LOG("start global matrxi decoding...");
                    local_iterations = 100;
                    global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
                }
                else
                {
                    LOG("hard decode local succeed");
                    local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                    global_iterations = 0; // used to show that global decode is not used
                }
                */
                LOG("start global matrxi decoding...");
                global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
                // 5.3 check decode result and compute tECC
                if (global_iterations == -1) // means global decode failed
                {
                    LOG("global matrix decode failed");
                    // https://files.futurememorystorage.com/proceedings/2019/08-07-Wednesday/20190807_CTRL-202-1_Kuo.pdf
                    //local_tECC = 3 * 1000;
                    local_tECC = 0;
                    //global_tECC = 8 * 1000;
                    global_tECC = 16 * 1000;
                    tECC = local_tECC + global_tECC;
                    // TODO
                    //total_local_iter += 100;
                    total_local_iter = 0;
                    total_global_iter += 200;
                    codeword_fail_time++;
                }
                else
                {
                    LOG("global matrix decode succeed");
                    //local_tECC = static_cast<uint64_t>(static_cast<double>(3000) / 100 * local_iterations);
                    local_tECC = 0;
                    global_tECC = static_cast<uint64_t>(static_cast<double>(16000) / 200 * global_iterations);
                    tECC = local_tECC + global_tECC;
                    //total_local_iter += local_iterations;
                    total_local_iter = 0;
                    total_global_iter += global_iterations;
                }
            }
            else
            // do soft decode
            {
                // 5.1 econd read or third read, use soft bit decode
                /*
                LOG("soft bit decoding...");
                LOG("start local matrxi decoding...");
                int decode_iter_local_1 = -1;
                int decode_iter_local_2 = -1;
                // if  rber >= 0.012, skip local matrix soft decode, only do local matrix soft decode when rber < 0.012
                if (rber < 0.01425)
                {
                    int decode_iter_local_1 = _my_instance->soft_decision_decode_local_1(channel_id, CodewordType);
                    int decode_iter_local_2 = _my_instance->soft_decision_decode_local_t(channel_id, CodewordType);
                }
    
                // 5.2 global matrix soft decode
                if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
                {
                    LOG("soft local decode failed");
                    LOG("start global matrxi decoding...");
                    local_iterations = 10;
                    global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
                }
                else 
                {
                    LOG("soft local decode succeed");
                    local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                    global_iterations = 0; // used to show that global decode is not used
                }
                */
                LOG("start global matrxi decoding...");
                global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
                if (global_iterations == -1)
                {
                    LOG("global decode failed");
                    // TODO
                    //local_tECC = 1.875 * 1000;
                    local_tECC = 0;
                    //global_tECC = 5 * 1000;
                    global_tECC = 24 * 1000;
                    tECC = local_tECC + global_tECC;
                    //total_local_iter += 10;
                    total_local_iter = 0;
                    total_global_iter += 20;
                    codeword_fail_time++;
                    if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
                    {
                        _my_instance->decode_failure++;
                        final_codeword_fail_time++;
                    }
                }
                else
                {
                    LOG("decode succeed");
                    //local_tECC = local_iterations * 187.5;
                    local_tECC = 0;
                    global_tECC = global_iterations * 1200;
                    tECC = local_tECC + global_tECC;
                    //total_local_iter += local_iterations;
                    total_local_iter = 0;
                    total_global_iter += global_iterations;
                }
            }
    
            LOG("rber=%lf,  local_iter = %d , global_iter = %d , tECC=%luns", rber, local_iterations, global_iterations, tECC);
    
            total_tECC += tECC;
    
            is_failed = is_failed || (global_iterations != -1 ? false : true);
    
            if (i == ncodeword - 1)
            {
                // Last info bits
                timestamp += tECC;
            }
            else
            {
                // Update next info bits arrive time
                next_codeword_arrive_time += tDMA_per_codeword;
                // Update next do ECC time
                timestamp = std::max(timestamp + tECC, next_codeword_arrive_time);
            }
        }
        goto end;
    }

    // start to do the decoding for 4kb
    case CodeWordType::_4kb:
    {
    
    // if rber >= 0.008 and first read, skipping
    if (rber >= 0.0085 && transaction->decode_type == DECODE_TYPE::HARD)
    {
        // 4kb hard decode max 32 us
        uint64_t tECC = 32 * 1000; // Normal
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        total_local_iter = 0;
        total_global_iter = 200 * ncodeword;
        goto end;
    }
    
    // if rber >= 0.020 and first soft decode, skipping soft decode
    if (rber >= 0.0195 && transaction->decode_type == DECODE_TYPE::FIRST_SOFT)
    {
        // 4kb soft decode max 48 us
        uint64_t tECC = 48 * 1000;
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        //total_iter = 20 * ncodeword;
        total_local_iter = 0;
        total_global_iter = 20 * ncodeword;
        goto end;
    }

    // if third read, rber /= 2
    if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
    {
        rber /= 2;
    }

    for (uint64_t i = 0; i < ncodeword; ++i)
    {
        //1. prepare data buffer for gc
        LOG("next info bits arrive time=%lu, timestamp=%lu", next_info_bits_arrive_time, timestamp);
        uint64_t tECC = 0;
        uint64_t local_tECC = 0;
        uint64_t global_tECC = 0;
        int local_iterations = 0; // no need for global only
        int global_iterations = -1;

        // 2. create the failure model
        Gaussian failure_model = Gaussian(ECC_Parameter_Set::information_bits_4kb, ECC_Parameter_Set::parity_bits_4kb, rber);

        // 3. generate the failures
        failure_model.generateFailure(_my_instance->code_buf_total_4kb.data(), _my_instance->receive_buf_total_4kb.data(), _my_instance->N_4kb);

        // 4. desemble the codeword
        // no need for global only
        /*
        for (uint64_t j = 0; j < _my_instance->N_4kb; j++)
        {
            if (j < _my_instance->N_local_4kb)
            {
                _my_instance->receive_buf_1_4kb[j] = _my_instance->receive_buf_total_4kb[j];
            }
            else
            {
                _my_instance->receive_buf_2_4kb[j - _my_instance->N_local_4kb] = _my_instance->receive_buf_total_4kb[j];
            }
        }
        */
        // 5. decode
        // TODO(iteration latency config)
        if (transaction->decode_type == DECODE_TYPE::HARD)
        // do hard decode
        {
            // 5.1 local matrix hard decode
            /*
            LOG("hard bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.004, skip local matrix hard decode, only do local matrix hard decode when rber < 0.004
            if(rber <= 0.004)
            {
                decode_iter_local_1 = _my_instance->hard_decision_decode_local_1(channel_id, CodewordType);
                decode_iter_local_2 = _my_instance->hard_decision_decode_local_t(channel_id, CodewordType);
            }
            // 5.2 global matrix hard decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("hard decode local failed");
                LOG("start global matrxi decoding...");
                local_iterations = 100;
                global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
            }
            else
            {
                LOG("hard decode local succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */

            // global only
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);

            // 5.3 check decode result and compute tECC
            if (global_iterations == -1) // means global decode failed
            {
                LOG("global matrix decode failed");
                // https://files.futurememorystorage.com/proceedings/2019/08-07-Wednesday/20190807_CTRL-202-1_Kuo.pdf
                /*
                local_tECC = 6 * 1000;
                global_tECC = 16 * 1000;
                tECC = local_tECC + global_tECC;
                // TODO
                total_local_iter += 100;
                total_global_iter += 100;
                codeword_fail_time++;
                */
                local_tECC = 0;
                global_tECC = 32 * 1000;
                tECC = local_tECC + global_tECC;
                // TODO
                total_local_iter = 0;
                total_global_iter += 200;
                codeword_fail_time++;
            }
            else
            {
                LOG("global matrix decode succeed");
                //local_tECC = static_cast<uint64_t>(static_cast<double>(6000) / 100 * local_iterations);
                local_tECC = 0;
                global_tECC = static_cast<uint64_t>(static_cast<double>(32000) / 200 * global_iterations);
                tECC = local_tECC + global_tECC;
                total_local_iter += local_iterations;
                total_global_iter += global_iterations;
            }
        }
        else
        // do soft decode
        {
            // 5.1 econd read or third read, use soft bit decode
            /*
            LOG("soft bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.012, skip local matrix soft decode, only do local matrix soft decode when rber < 0.012
            if (rber < 0.012)
            {
                int decode_iter_local_1 = _my_instance->soft_decision_decode_local_1(channel_id, CodewordType);
                int decode_iter_local_2 = _my_instance->soft_decision_decode_local_t(channel_id, CodewordType);
            }
            
            // 5.2 global matrix soft decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("soft local decode failed");
                LOG("start global matrxi decoding...");
                local_iterations = 10;
                global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            }
            else 
            {
                LOG("soft local decode succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */
            
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            

            if (global_iterations == -1)
            {
                LOG("global decode failed");
                // TODO
                //local_tECC = 3.75 * 1000;
                local_tECC = 0;
                global_tECC = 48 * 1000;
                tECC = local_tECC + global_tECC;
                //total_local_iter += 10;
                total_local_iter = 0;
                total_global_iter += 20;
                codeword_fail_time++;
                if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
                {
                    _my_instance->decode_failure++;
                    final_codeword_fail_time++;
                }
            }
            else
            {
                LOG("decode succeed");
                //local_tECC = local_iterations * 375;
                local_tECC = 0;
                global_tECC = global_iterations * 2.4 * 1000;
                tECC = local_tECC + global_tECC;
                total_local_iter += local_iterations;
                total_global_iter += global_iterations;
            }
        }

        LOG("rber=%lf,  local_iter = %d , global_iter = %d , tECC=%luns", rber, local_iterations, global_iterations, tECC);

        total_tECC += tECC;

        is_failed = is_failed || (global_iterations != -1 ? false : true);

        if (i == ncodeword - 1)
        {
            // Last info bits
            timestamp += tECC;
        }
        else
        {
            // Update next info bits arrive time
            next_codeword_arrive_time += tDMA_per_codeword;
            // Update next do ECC time
            timestamp = std::max(timestamp + tECC, next_codeword_arrive_time);
        }
    }
    goto end;
}

    // start to do the decoding for 8kb
    case CodeWordType::_8kb:{
        // if rber >= 0.008 and first read, skipping
    if (rber >= 0.009 && transaction->decode_type == DECODE_TYPE::HARD)
    {
        // 8kb hard decode max 64us
        uint64_t tECC = 64 * 1000;
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        total_local_iter = 0;
        total_global_iter = 200 * ncodeword;
        goto end;
    }
    
    // if rber >= 0.020 and first soft decode, skipping soft decode
    if (rber >= 0.02 && transaction->decode_type == DECODE_TYPE::FIRST_SOFT)
    {
        // 8kb soft decode max 96 us
        uint64_t tECC = 96 * 1000;
        total_tECC = tECC * ncodeword;
        is_failed = true;
        if (tDMA_per_codeword > tECC)
        {
            timestamp = Simulator->Time() + tDMA_per_codeword * (ncodeword - 1) + tECC;
        }
        else
        {
            timestamp += total_tECC;
        }
        // TODO
        total_local_iter = 0;
        total_global_iter = 20 * ncodeword;
        goto end;
    }

    // if third read, rber /= 2
    if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
    {
        rber /= 2;
    }

    for (uint64_t i = 0; i < ncodeword; ++i)
    {
        //1. prepare data buffer for gc
        LOG("next info bits arrive time=%lu, timestamp=%lu", next_info_bits_arrive_time, timestamp);
        uint64_t tECC = 0;
        uint64_t local_tECC = 0;
        uint64_t global_tECC = 0;
        int local_iterations = 0; // no need for global only
        int global_iterations = -1;

        // 2. create the failure model
        Gaussian failure_model = Gaussian(ECC_Parameter_Set::information_bits_8kb, ECC_Parameter_Set::parity_bits_8kb, rber);

        // 3. generate the failures
        failure_model.generateFailure(_my_instance->code_buf_total_8kb.data(), _my_instance->receive_buf_total_8kb.data(), _my_instance->N_8kb);

        // 4. desemble the codeword
        // no need for global only
        /*
        for (uint64_t j = 0; j < _my_instance->N_8kb; j++)
        {
            if (j < _my_instance->N_local_8kb)
            {
                _my_instance->receive_buf_1_8kb[j] = _my_instance->receive_buf_total_8kb[j];
            }
            else
            {
                _my_instance->receive_buf_2_8kb[j - _my_instance->N_local_8kb] = _my_instance->receive_buf_total_8kb[j];
            }
        }
        */
        // 5. decode
        // TODO(iteration latency config)
        if (transaction->decode_type == DECODE_TYPE::HARD)
        // do hard decode
        {
            /*
            // 5.1 local matrix hard decode
            LOG("hard bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.004, skip local matrix hard decode, only do local matrix hard decode when rber < 0.004
            if(rber <= 0.004)
            {
                decode_iter_local_1 = _my_instance->hard_decision_decode_local_1(channel_id, CodewordType);
                decode_iter_local_2 = _my_instance->hard_decision_decode_local_t(channel_id, CodewordType);
            }

            // 5.2 global matrix hard decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("hard decode local failed");
                LOG("start global matrxi decoding...");
                local_iterations = 100;
                global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
            }
            else
            {
                LOG("hard decode local succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->hard_decision_decode_global(channel_id, CodewordType);
            // 5.3 check decode result and compute tECC
            if (global_iterations == -1) // means global decode failed
            {
                LOG("global matrix decode failed");
                // https://files.futurememorystorage.com/proceedings/2019/08-07-Wednesday/20190807_CTRL-202-1_Kuo.pdf
                //local_tECC = 6 * 1000;
                local_tECC = 0;
                global_tECC = 64 * 1000;
                tECC = local_tECC + global_tECC;
                // TODO
                //total_local_iter += 100;
                total_local_iter = 0;
                total_global_iter += 200;
                codeword_fail_time++;
            }
            else
            {
                LOG("global matrix decode succeed");
                //local_tECC = static_cast<uint64_t>(static_cast<double>(6000) / 100 * local_iterations);
                local_tECC = 0;
                global_tECC = static_cast<uint64_t>(static_cast<double>(64000) / 200 * global_iterations);
                tECC = local_tECC + global_tECC;
                //total_local_iter += local_iterations;
                total_local_iter = 0;
                total_global_iter += global_iterations;
            }
        }
        else
        // do soft decode
        {
            // 5.1 econd read or third read, use soft bit decode
            /*
            LOG("soft bit decoding...");
            LOG("start local matrxi decoding...");
            int decode_iter_local_1 = -1;
            int decode_iter_local_2 = -1;
            // if  rber >= 0.012, skip local matrix soft decode, only do local matrix soft decode when rber < 0.012
            if (rber < 0.012)
            {
                int decode_iter_local_1 = _my_instance->soft_decision_decode_local_1(channel_id, CodewordType);
                int decode_iter_local_2 = _my_instance->soft_decision_decode_local_t(channel_id, CodewordType);
            }

            // 5.2 global matrix soft decode
            if (decode_iter_local_1 == -1 || decode_iter_local_2 == -1)
            {
                LOG("soft local decode failed");
                LOG("start global matrxi decoding...");
                local_iterations = 10;
                global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            }
            else 
            {
                LOG("soft local decode succeed");
                local_iterations = std::max(decode_iter_local_1, decode_iter_local_2);
                global_iterations = 0; // used to show that global decode is not used
            }
            */
            LOG("start global matrxi decoding...");
            global_iterations = _my_instance->soft_decision_decode_global(channel_id, CodewordType);
            if (global_iterations == -1)
            {
                LOG("global decode failed");
                // TODO
                //local_tECC = 3.75 * 1000;
                local_tECC = 0;
                global_tECC = 96 * 1000;
                tECC = local_tECC + global_tECC;
                //total_local_iter += 10;
                total_local_iter = 0;
                total_global_iter += 20;
                codeword_fail_time++;
                if (transaction->decode_type == DECODE_TYPE::SECOND_SOFT)
                {
                    _my_instance->decode_failure++;
                    final_codeword_fail_time++;
                }
            }
            else
            {
                LOG("decode succeed");
                //local_tECC = local_iterations * 375;
                local_tECC = 0;
                global_tECC = global_iterations * 4.8 *1000;
                tECC = local_tECC + global_tECC;
                //total_local_iter += local_iterations;
                total_local_iter = 0;
                total_global_iter += global_iterations;
            }
        }

        LOG("rber=%lf,  local_iter = %d , global_iter = %d , tECC=%luns", rber, local_iterations, global_iterations, tECC);

        total_tECC += tECC;

        is_failed = is_failed || (global_iterations != -1 ? false : true);

        if (i == ncodeword - 1)
        {
            // Last info bits
            timestamp += tECC;
        }
        else
        {
            // Update next info bits arrive time
            next_codeword_arrive_time += tDMA_per_codeword;
            // Update next do ECC time
            timestamp = std::max(timestamp + tECC, next_codeword_arrive_time);
        }
    }
    goto end;
    }
    
    // start to do the decoding for 16kb
    case CodeWordType::_16kb:{
        // TODO
        break;
    }
    
    default:{
        PRINT_ERROR("Invalid codeword type");
        break;
    }
}

end:
    // Update stat
    if(final_codeword_fail_time == 1){_my_instance->onecodeword_decode_failure++;}
    if(final_codeword_fail_time == 2){_my_instance->twocodeword_decode_failure++;}
    if(final_codeword_fail_time == 3){_my_instance->threecodeword_decode_failure++;}
    if(final_codeword_fail_time == 4){_my_instance->fourcodeword_decode_failure++;}
    if(final_codeword_fail_time == 5){_my_instance->fivecodeword_decode_failure++;}
    if(final_codeword_fail_time == 6){_my_instance->sixcodeword_decode_failure++;}
    if(final_codeword_fail_time == 7){_my_instance->sevencodeword_decode_failure++;}
    if(final_codeword_fail_time == 8){_my_instance->eightcodeword_decode_failure++;}
    if(codeword_fail_time > 0 && codeword_fail_time < ncodeword && ECC_Parameter_Set::enable_failure_aware_read_retry){
    //cout << "datasize_per_codeword: " << datasize_per_codeword << endl;
    //cout << "data_and_metadata_size_in_byte: " << data_and_metadata_size_in_byte << endl;
    //cout << "codeword_fail_time = " << codeword_fail_time << endl;
    //cout << "new datasize" << data_and_metadata_size_in_byte - (ncodeword - codeword_fail_time) * datasize_per_codeword;
    transaction->Data_and_metadata_size_in_byte = data_and_metadata_size_in_byte - (ncodeword - codeword_fail_time) * datasize_per_codeword;
    }
    _my_instance->total_decode_times += ncodeword;                               // update total decode times
    _my_instance->decode_times[(int)transaction->decode_type] += ncodeword;      // update decode times
    _my_instance->decode_tECC[(int)transaction->decode_type] += total_tECC;
    _my_instance->decode_tECC_tDMA_overlap[(int)transaction->decode_type] += total_tECC + tDMA_per_codeword * (ncodeword - 1) - (timestamp - decode_start_time);
    _my_instance->local_decode_iteration[(int)transaction->decode_type] += total_local_iter;
    _my_instance->global_decode_iteration[(int)transaction->decode_type] += total_global_iter;    
    _my_instance->decode_iteration[(int)transaction->decode_type] += total_iter; // update iteration

    auto type = is_failed ? ECCSimEventType::DECODE_FAILED : ECCSimEventType::DECODE_SUCCEED;
    Simulator->Register_sim_event(timestamp, _my_instance, (void *)transaction, (int)type);
}

int ECCEngine::soft_decision_decode_local_1(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
            return soft_decision_decoder_1kb.decode_local_1(receive_buf_1_1kb.data(), corrected_buf_1_1kb.data(), _my_instance->encoder_1kb.getRowMat1(),
                                        _my_instance->encoder_1kb.getColMat1(), _my_instance->alpha_local_1kb,
                                        _my_instance->beta_local_1kb);
        case CodeWordType::_2kb:
            return soft_decision_decoder_2kb.decode_local_1(receive_buf_1_2kb.data(), corrected_buf_1_2kb.data(), _my_instance->encoder_2kb.getRowMat1(),
                                        _my_instance->encoder_2kb.getColMat1(), _my_instance->alpha_local_2kb,
                                        _my_instance->beta_local_2kb);
        case CodeWordType::_4kb:
            return soft_decision_decoder_4kb.decode_local_1(receive_buf_1_4kb.data(), corrected_buf_1_4kb.data(), _my_instance->encoder_4kb.getRowMat1(),
                                        _my_instance->encoder_4kb.getColMat1(), _my_instance->alpha_local_4kb,
                                        _my_instance->beta_local_4kb);
        case CodeWordType::_8kb:
            return soft_decision_decoder_8kb.decode_local_1(receive_buf_1_8kb.data(), corrected_buf_1_8kb.data(), _my_instance->encoder_8kb.getRowMat1(),
                                        _my_instance->encoder_8kb.getColMat1(), _my_instance->alpha_local_8kb,
                                        _my_instance->beta_local_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
}

int ECCEngine::hard_decision_decode_local_1(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
            return hard_decision_decoder_1kb.decode_local_1(receive_buf_1_1kb.data(), corrected_buf_1_1kb.data(), _my_instance->encoder_1kb.getRowMat1(),
                                        _my_instance->encoder_1kb.getColMat1(), _my_instance->alpha_local_1kb,
                                        _my_instance->beta_local_1kb);
        case CodeWordType::_2kb:
            return hard_decision_decoder_2kb.decode_local_1(receive_buf_1_2kb.data(), corrected_buf_1_2kb.data(), _my_instance->encoder_2kb.getRowMat1(),
                                        _my_instance->encoder_2kb.getColMat1(), _my_instance->alpha_local_2kb,
                                        _my_instance->beta_local_2kb);
        case CodeWordType::_4kb:
            return hard_decision_decoder_4kb.decode_local_1(receive_buf_1_4kb.data(), corrected_buf_1_4kb.data(), _my_instance->encoder_4kb.getRowMat1(),
                                        _my_instance->encoder_4kb.getColMat1(), _my_instance->alpha_local_4kb,
                                        _my_instance->beta_local_4kb);
        case CodeWordType::_8kb:
            return hard_decision_decoder_8kb.decode_local_1(receive_buf_1_8kb.data(), corrected_buf_1_8kb.data(), _my_instance->encoder_8kb.getRowMat1(),
                                        _my_instance->encoder_8kb.getColMat1(), _my_instance->alpha_local_8kb,
                                        _my_instance->beta_local_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
}

int ECCEngine::soft_decision_decode_local_2(uint64_t, CodeWordType CodewordType)
{return -1;}

int ECCEngine::hard_decision_decode_local_2(uint64_t, CodeWordType CodewordType)
{return -1;}

int ECCEngine::soft_decision_decode_local_t(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
        return soft_decision_decoder_1kb.decode_local_t(receive_buf_2_1kb.data(), corrected_buf_2_1kb.data(), _my_instance->encoder_1kb.getTRowMat(),
                                    _my_instance->encoder_1kb.getTColMat(), _my_instance->alpha_local_1kb,
                                    _my_instance->beta_local_1kb);
        case CodeWordType::_2kb:
        // TODO
        return soft_decision_decoder_2kb.decode_local_t(receive_buf_2_2kb.data(), corrected_buf_2_2kb.data(), _my_instance->encoder_2kb.getTRowMat(),
                                    _my_instance->encoder_2kb.getTColMat(), _my_instance->alpha_local_2kb,
                                    _my_instance->beta_local_2kb);
        case CodeWordType::_4kb:
            return soft_decision_decoder_4kb.decode_local_t(receive_buf_2_4kb.data(), corrected_buf_2_4kb.data(), _my_instance->encoder_4kb.getTRowMat(),
                                        _my_instance->encoder_4kb.getTColMat(), _my_instance->alpha_local_4kb,
                                        _my_instance->beta_local_4kb);
        case CodeWordType::_8kb:
            return soft_decision_decoder_8kb.decode_local_t(receive_buf_2_8kb.data(), corrected_buf_2_8kb.data(), _my_instance->encoder_8kb.getTRowMat(),
                                        _my_instance->encoder_8kb.getTColMat(), _my_instance->alpha_local_8kb,
                                        _my_instance->beta_local_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
    // TODO
}

int ECCEngine::hard_decision_decode_local_t(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
        return hard_decision_decoder_1kb.decode_local_t(receive_buf_2_1kb.data(), corrected_buf_2_1kb.data(), _my_instance->encoder_1kb.getTRowMat(),
                                    _my_instance->encoder_1kb.getTColMat(), _my_instance->alpha_local_1kb,
                                    _my_instance->beta_local_1kb);
        case CodeWordType::_2kb:
        // TODO
        return hard_decision_decoder_2kb.decode_local_t(receive_buf_2_2kb.data(), corrected_buf_2_2kb.data(), _my_instance->encoder_2kb.getTRowMat(),
                                    _my_instance->encoder_2kb.getTColMat(), _my_instance->alpha_local_2kb,
                                    _my_instance->beta_local_2kb);
        case CodeWordType::_4kb:
            return hard_decision_decoder_4kb.decode_local_t(receive_buf_2_4kb.data(), corrected_buf_2_4kb.data(), _my_instance->encoder_4kb.getTRowMat(),
                                        _my_instance->encoder_4kb.getTColMat(), _my_instance->alpha_local_4kb,
                                        _my_instance->beta_local_4kb);
        case CodeWordType::_8kb:
            return hard_decision_decoder_8kb.decode_local_t(receive_buf_2_8kb.data(), corrected_buf_2_8kb.data(), _my_instance->encoder_8kb.getTRowMat(),
                                        _my_instance->encoder_8kb.getTColMat(), _my_instance->alpha_local_8kb,
                                        _my_instance->beta_local_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
}

int ECCEngine::soft_decision_decode_global(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
        return soft_decision_decoder_1kb.decode_global(receive_buf_total_1kb.data(), corrected_buf_total_1kb.data(), _my_instance->encoder_1kb.getGlobalRowMat(),
                                    _my_instance->encoder_1kb.getGlobalColMat(), _my_instance->alpha_global_1kb,
                                    _my_instance->beta_global_1kb);
        case CodeWordType::_2kb:
        // TODO
        return soft_decision_decoder_2kb.decode_global(receive_buf_total_2kb.data(), corrected_buf_total_2kb.data(), _my_instance->encoder_2kb.getGlobalRowMat(),
                                    _my_instance->encoder_2kb.getGlobalColMat(), _my_instance->alpha_global_2kb,
                                    _my_instance->beta_global_2kb);
        case CodeWordType::_4kb:
            return soft_decision_decoder_4kb.decode_global(receive_buf_total_4kb.data(), corrected_buf_total_4kb.data(), _my_instance->encoder_4kb.getGlobalRowMat(),
                                        _my_instance->encoder_4kb.getGlobalColMat(), _my_instance->alpha_global_4kb,
                                        _my_instance->beta_global_4kb);
        case CodeWordType::_8kb:
            return soft_decision_decoder_8kb.decode_global(receive_buf_total_8kb.data(), corrected_buf_total_8kb.data(), _my_instance->encoder_8kb.getGlobalRowMat(),
                                        _my_instance->encoder_8kb.getGlobalColMat(), _my_instance->alpha_global_8kb,
                                        _my_instance->beta_global_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
}

int ECCEngine::hard_decision_decode_global(uint64_t, CodeWordType CodewordType)
{
    switch(CodewordType)
    {
        case CodeWordType::_1kb:
            return hard_decision_decoder_1kb.decode_global(receive_buf_total_1kb.data(), corrected_buf_total_1kb.data(), _my_instance->encoder_1kb.getGlobalRowMat(),
                                        _my_instance->encoder_1kb.getGlobalColMat(), _my_instance->alpha_global_1kb,
                                        _my_instance->beta_global_1kb);
        case CodeWordType::_2kb:
            // TODO
            return hard_decision_decoder_2kb.decode_global(receive_buf_total_2kb.data(), corrected_buf_total_2kb.data(), _my_instance->encoder_2kb.getGlobalRowMat(),
                                        _my_instance->encoder_2kb.getGlobalColMat(), _my_instance->alpha_global_2kb,
                                        _my_instance->beta_global_2kb);
        case CodeWordType::_4kb:
            return hard_decision_decoder_4kb.decode_global(receive_buf_total_4kb.data(), corrected_buf_total_4kb.data(), _my_instance->encoder_4kb.getGlobalRowMat(),
                                        _my_instance->encoder_4kb.getGlobalColMat(), _my_instance->alpha_global_4kb,
                                        _my_instance->beta_global_4kb);
        case CodeWordType::_8kb:
            return hard_decision_decoder_8kb.decode_global(receive_buf_total_8kb.data(), corrected_buf_total_8kb.data(), _my_instance->encoder_8kb.getGlobalRowMat(),
                                        _my_instance->encoder_8kb.getGlobalColMat(), _my_instance->alpha_global_8kb,
                                        _my_instance->beta_global_8kb);
        default:
            PRINT_ERROR("Invalid codeword type");
            break;
    }
}

void ECCEngine::submit_write_transactions(const NVM_Transaction_Flash *transaction)
{
    assert(transaction->Type == Transaction_Type::WRITE);
    encode_waiting_queues[transaction->Address.ChannelID].push_back(transaction);
}

void ECCEngine::encode()
{
    for (size_t channel_id = 0; channel_id < channel_count; ++channel_id)
    {
        if (encode_status[channel_id] == ECCEngineStatus::IDLE && !encode_waiting_queues[channel_id].empty())
        {
            const NVM_Transaction_Flash *transaction = NULL;

            decltype(encode_waiting_queues[channel_id].begin()) iter;
            // Find a write transaction that RelatedRead is NULL
            for (auto it = encode_waiting_queues[channel_id].begin(); it != encode_waiting_queues[channel_id].end();
                 it++)
            {
                if (((NVM_Transaction_Flash_WR *)(*it))->RelatedRead == NULL)
                {
                    LOG();
                    transaction = *it;
                    iter = it;
                    break;
                }
            }

            // There is no transaction
            if (transaction == NULL)
            {
                continue;
            }

            encode_waiting_queues[channel_id].erase(iter);
            encode_status[channel_id] = ECCEngineStatus::BUSY;

            auto n_pages = (transaction->Data_and_metadata_size_in_byte + Flash_Parameter_Set::Page_Capacity - 1) /
                           Flash_Parameter_Set::Page_Capacity;
            uint64_t tECC = n_pages * Flash_Parameter_Set::Page_Capacity / ENCODING_THROUGHOUT;
            // Register a event
            Simulator->Register_sim_event(Simulator->Time() + tECC, _my_instance, (void *)transaction,
                                          (int)ECCSimEventType::ENCODE_DATA_ARRIVED);
        }
    }
}

} // namespace SSD_Components
