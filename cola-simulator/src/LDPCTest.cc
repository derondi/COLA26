#include "inc/include.hh"
#include "common/Config.hh"
#include "encoder/EncoderFactory.hh"
#include "encoder/EncoderBase.hh"
#include "failure/FailureFactory.hh"
#include "failure/FailureBase.hh"
#include "decoder/DecoderFactory.hh"
#include "decoder/DecoderBase.hh"

using namespace std;

void usage()
{
    cout << "Usage: ./LDPCTest" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        usage();
        return 0;
    }

    // 0. read configuration
    string confpath = "conf/normal/setting.xml";
    Config *conf = new Config(confpath);

    long long information_bits = conf->_information_bits;
    long long parity_bits = conf->_parity_bits;
    string ldpc_encoder_name = conf->_ldpc_encoder_name;
    string ldpc_encoder_param = conf->_ldpc_encoder_param;
    string failure_model_name = conf->_failure_model_name;
    string failure_model_param = conf->_failure_model_param;
    string ldpc_decoder_name = conf->_ldpc_decoder_name;
    string ldpc_decoder_param = conf->_ldpc_decoder_param;
    double rber_rate = conf->_rber_rate;
    long long iteration = conf->_iteration;
    double beta = conf->_beta;
    double alpha = conf->_alpha;
    int max_decode_iter = stoi(ldpc_decoder_param);
    int max_error_bit = 0;
    int max_error_bit_index = -1;
    int decode_page_num = 0; // page = 16kb, codeword = 4kb
    int codeword_fail = 0;
    int page_fail = 0;
    int codeword_fail_num[5] = {0, 0, 0, 0, 0}; // record the number of pages with 0,1,2,3,4 codeword fail
    //string errorlog_location = conf->_ssd_errorlog_location;

    cout << "--- ldpc simulator's configuration ---" << endl;
    cout << "information_bits: " << information_bits << endl;
    cout << "parity_bits: " << parity_bits << endl;
    cout << "ldpc_encoder_name: " << ldpc_encoder_name << endl;
    cout << "ldpc_encoder_param: " << ldpc_encoder_param << endl;
    cout << "failure_model_name: " << failure_model_name << endl;
    cout << "failure_model_param: " << failure_model_param << endl;
    cout << "ldpc_decoder_name: " << ldpc_decoder_name << endl;
    cout << "ldpc_decoder_param: " << ldpc_decoder_param << endl;
    cout << "rber_rate: " << rber_rate << endl;
    cout << "msa_beta: " << beta << endl;
    cout << "msa_alpha: " << alpha << endl;
    // number of ldpc simulations, not decoder maxIter
    // decoder maxIter is set in ldpc_decoder_param
    cout << "iteration: " << iteration << endl
         << endl;

    // 1. create encoder
    EncoderFactory *encoder_factory = new EncoderFactory();
    EncoderBase *encoder = encoder_factory->createEncoder(ldpc_encoder_name, ldpc_encoder_param);

    // 2. create failure model
    FailureFactory *failure_factory = new FailureFactory();
    FailureBase *model = failure_factory->createFailureModel(failure_model_name, failure_model_param);

    // 3. create decoder
    DecoderFactory *decoder_factory = new DecoderFactory();
    DecoderBase *decoder = decoder_factory->createDecoder(ldpc_decoder_name, ldpc_decoder_param, *encoder, alpha);

    // 4. prepare data buffer
    unsigned char *info_buf = (unsigned char *)calloc(information_bits, sizeof(unsigned char));
    unsigned char *parity_buf = (unsigned char *)calloc(parity_bits, sizeof(unsigned char));
    double *receive_buf = (double *)calloc(information_bits + parity_bits, sizeof(double));
    unsigned char *corrected_buf = (unsigned char *)calloc(information_bits + parity_bits, sizeof(unsigned char));

    long long ldpc_fail = 0, error_bit = 0;
    long long decode_iter_sum = 0;
    if (iteration % 4 == 0){
        decode_page_num = iteration / 4;
    }
    else {
        cout << "iteration should be multiple of 4" << endl;
        exit(0);
    }
    cout << "current Nano time seed: " << seed << endl;
    cout << "start " << iteration << " times ldpc simulations..." << endl;
    auto start = chrono::system_clock::to_time_t(chrono::system_clock::now());

    // 4.1.single tesr
    /*
    if (iteration == 1)
    {
        string error_path = errorlog_location;
        ifstream in(error_path, ios::in);
        if (!in.is_open())
        {
            cout << "open error!" << endl;
            exit(0);
        }
        // a.init buffer
        for (long long i = 0; i < information_bits; i++)
        {
            int temp;
            in >> temp;
            info_buf[i] = (unsigned char)temp;
        }
        for (long long i = 0; i < parity_bits; i++)
        {
            int temp;
            in >> temp;
            parity_buf[i] = (unsigned char)temp;
        }
        for (long long i = 0; i < information_bits + parity_bits; i++)
        {
            in >> receive_buf[i];
        }

        // c.decode information
        int decode_iter = decoder->decode(receive_buf, corrected_buf, encoder->getRowMat(), encoder->getColMat(), alpha, beta);

        // d.check
        bool right = true;
        vector<vector<int>> col_mat = encoder->getColMat();
        for (long long i = 0; i < information_bits + parity_bits; i++)
        {
            if (i < information_bits)
            {
                if (info_buf[i] != corrected_buf[i])
                {
                    right = false;
                    // cout <<"wrong place = " << i << endl;
                    error_bit++;
                }
            }
            else
            {
                if (parity_buf[i - information_bits] != corrected_buf[i])
                {
                    right = false;
                    // cout <<"wrong place = " << i << endl;
                    error_bit++;
                }
            }
        }
        cout << "error bit = " << error_bit << endl;

        // e.judge seccess or false
        if (!right)
            cout << "this test result is false!" << endl;
        else
            cout << "this test result is success!" << endl;
        return 0;
    }
    */

    // 5. random generate info bits and encode information
        // a. random generate info bits
        for (long long i = 0; i < information_bits; i++)
        {
            if (u(e) % 2 == 0)
                info_buf[i] = (unsigned char)0;
            else
                info_buf[i] = (unsigned char)1;
        }

        // b. encode information
        encoder->encode(info_buf, parity_buf);
        
    // 5. simulate multiple times [data storage and data access]
    for (long long iter = 0; iter < iteration; iter++)
    {
        if ((iter % (iteration / 4)) == (iteration / 4 - 1))
        {
            auto cur_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
            cout << "===> " << (100 * iter) / iteration + 1 << "\% complete, " << "cost " << cur_t - start << " s" << endl;
        }
        // a. generate failure
        model->generateFailure(info_buf, parity_buf, receive_buf);

        // b. decode information
        int decode_iter = decoder->decode(receive_buf, corrected_buf, encoder->getRowMat(), encoder->getColMat(), alpha, beta);

        // c. check whether the result is right and count errors (info + parity)
        if (decode_iter == -1)
        {
            codeword_fail++;
            decode_iter_sum += max_decode_iter;
            int this_error_bit = 0;
            for (long long i = 0; i < information_bits + parity_bits; i++)
            {
                if (i < information_bits)
                {
                    if (info_buf[i] != corrected_buf[i])
                        this_error_bit++;
                }
                else
                {
                    if (parity_buf[i - information_bits] != corrected_buf[i])
                        this_error_bit++;
                }
            }
            if (this_error_bit > max_error_bit)
            {
                max_error_bit = this_error_bit;
                max_error_bit_index = ldpc_fail;
            }
            error_bit += this_error_bit;
            /*
            // add error log
            if (ldpc_fail == 0)
            {
                string error_path = "errorlog/" + to_string(seed);
                string command;
                command = "mkdir -p " + error_path;
                if (-1 == system(command.c_str()))
                    cout << "system command error";
            }
            ofstream ErrorFile("errorlog/" + to_string(seed) + "/" + to_string(ldpc_fail));
            if (ErrorFile.is_open())
            {
                for (long long i = 0; i < information_bits; i++)
                    ErrorFile << (int)info_buf[i] << " ";
                ErrorFile << endl;
                for (long long i = 0; i < parity_bits; i++)
                    ErrorFile << (int)parity_buf[i] << " ";
                ErrorFile << endl;
                for (long long i = 0; i < information_bits + parity_bits; i++)
                    ErrorFile << receive_buf[i] << " ";
                ErrorFile.close();
            }
            else
            {
                cerr << "Failed to create file! target fileName = " << endl;
                exit(-1);
            }
            */
            ldpc_fail++;
        }
        else
            decode_iter_sum += decode_iter;
        
        if (iter % 4 == 3){
            if (codeword_fail > 0) {
                page_fail++;
                codeword_fail_num[codeword_fail]++;
                codeword_fail = 0;
            }
            else {
                codeword_fail_num[0]++;
            }
        }

        if (iter % 10000 == 0 && iter != 0)
        {
            cout << iter / 100000 << ":frame error rate (FER) = " << (double)ldpc_fail / (double)iter << endl;
            cout << iter / 100000 << ":uncorrectable bit error rate (UBER) = " << (double)error_bit / (double)(iter * (information_bits + parity_bits)) << endl;
        }
    }
    auto end = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    cout << "finish " << iteration << " times ldpc simulations, cost " << end - start << " s" << endl
         << endl;
    cout << "max error bit = " << max_error_bit << ", index = " << max_error_bit_index << endl;
    cout << "decode iter sum = " << decode_iter_sum << endl;
    cout << "avg iter num = " << (double)decode_iter_sum / iteration << endl;
    cout << "frame error number = " << ldpc_fail << endl;
    cout << "frame error rate (FER) = " << (double)ldpc_fail / (double)iteration << endl;
    cout << "uncorrectable bit error number = " << error_bit << endl;
    cout << "uncorrectable bit error rate (UBER) = " << (double)error_bit / (double)(iteration * (information_bits + parity_bits)) << endl;
    cout << "finish page number = " << decode_page_num << endl;
    cout << "fail page number = " << page_fail << endl;
    cout << "zero fail page number = " << codeword_fail_num[0] << endl;
    cout << "one fail page number = " << codeword_fail_num[1] << endl;
    cout << "two fail page number = " << codeword_fail_num[2] << endl;
    cout << "three fail page number = " << codeword_fail_num[3] << endl;
    cout << "four fail page number = " << codeword_fail_num[4] << endl;
    // 7. clean buffer
    delete conf;
    free(info_buf);
    free(parity_buf);
    delete encoder_factory;
    delete encoder;
    delete failure_factory;
    delete model;
    free(receive_buf);
    delete decoder_factory;
    delete decoder;
    free(corrected_buf);

    return 0;
}
