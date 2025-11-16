#include "inc/include.hh"
#include "common/Config.hh"
#include "encoder/EncoderFactory.hh"
#include "encoder/EncoderBase.hh"
#include "gc-encoder/GC-EncoderFactory.hh"
#include "gc-encoder/GC-EncoderBase.hh"
#include "gc-failure/GC-FailureFactory.hh"
#include "gc-failure/GC-FailureBase.hh"
#include "decoder/DecoderFactory.hh"
#include "decoder/DecoderBase.hh"
#include "gc-decoder/GC-DecoderFactory.hh"
#include "gc-decoder/GC-DecoderBase.hh"

using namespace std;

void usage()
{
    cout << "Usage: ./Global-GC-LDPCTest" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        usage();
        return 0;
    }

    // 0. read configuration
    string confpath = "conf/gc/setting.xml";
    Config *conf = new Config(confpath);

    long long information_bits = conf->_information_bits;
    long long parity_bits = conf->_parity_bits;
    string gc_ldpc_encoder_name = conf->_gc_ldpc_encoder_name;
    string gc_ldpc_encoder_param = conf->_gc_ldpc_encoder_param;
    string gc_ldpc_decoder_name = conf->_gc_ldpc_decoder_name;
    string gc_ldpc_decoder_param = conf->_gc_ldpc_decoder_param;
    string gc_failure_model_name = conf->_gc_failure_model_name;
    string gc_failure_model_param = conf->_gc_failure_model_param;
    double rber_rate = conf->_rber_rate;
    long long iteration = conf->_iteration;
    double beta = conf->_beta;
    double alpha = conf->_alpha;

    int max_error_bit = 0;
    int max_error_bit_index = -1;

    vector<string> v;
    splitString(gc_ldpc_encoder_param, v, ";");
    int local_matrix_number = stoi(v[0]);
    int _m_local = stoi(v[1]);
    int _m_global = stoi(v[2]);
    int _m = stoi(v[3]);
    int _n = stoi(v[4]);
    int _N = stoi(v[5]);
    int M_local = _m_local * _N;
    int N_local = (_n / local_matrix_number) * _N;
    int M_global = _m_global * _N;
    int M = _m * _N;
    int N = _n * _N;

    vector<string> q;
    splitString(gc_ldpc_decoder_param, q, ";");
    int max_decode_iter_global = stoi(q[1]);

    // 1. decide the GC structure
    // when local_matrix_number = 2 
    if(local_matrix_number == 2)
    {
        cout << "--- gc-ldpc simulator's configuration ---" << endl;
        cout << "local_matrix_number: " << local_matrix_number << endl;
        cout << "first_local_information_bits: " << N_local - M_local << endl;
        cout << "first_local_parity_bits: " << M_local << endl;
        cout << "second_local_information_bits: " << N_local - M_local - M_global << endl;
        cout << "second_local_parity_bits: " << M_local << endl;
        cout << "global_parity_bits: " << M_global << endl;
        cout << "ldpc_encoder_name: " << gc_ldpc_encoder_name << endl;
        cout << "ldpc_encoder_param: " << gc_ldpc_encoder_param << endl;
        cout << "failure_model_name: " << gc_failure_model_name << endl;
        cout << "failure_model_param: " << gc_failure_model_param << endl;
        cout << "ldpc_decoder_name: " << gc_ldpc_decoder_name << endl;
        cout << "ldpc_decoder_param: " << gc_ldpc_decoder_param << endl;
        cout << "rber_rate: " << rber_rate << endl;
        cout << "msa_beta: " << beta << endl;
        cout << "msa_alpha: " << alpha << endl;
        cout << "iteration: " << iteration << endl
        << endl;

        // 2. create encoder
       // EncoderFactory *encoder_factory = new EncoderFactory();
        //EncoderBase *encoder = encoder_factory->createEncoder(gc_ldpc_encoder_name, gc_ldpc_encoder_param);

        GC_EncoderFactory *encoder_factory = new GC_EncoderFactory();
        GC_EncoderBase *encoder = encoder_factory->createGCEncoder(gc_ldpc_encoder_name, gc_ldpc_encoder_param);

        // 3. create failure model
        GC_FailureFactory *failure_factory = new GC_FailureFactory();
        GC_FailureBase *model = failure_factory->createGCFailureModel(gc_failure_model_name, gc_failure_model_param);

        // 4. create decoder
        GC_DecoderFactory *decoder_factory = new GC_DecoderFactory();
        GC_DecoderBase *decoder = decoder_factory->createGCDecoder(gc_ldpc_decoder_name, gc_ldpc_decoder_param, *encoder, alpha);

        // 5. prepare data buffer
        unsigned char *s1 = (unsigned char *)calloc(N_local - M_local, sizeof(unsigned char));
        unsigned char *p1 = (unsigned char *)calloc(M_local, sizeof(unsigned char));
        unsigned char *c1 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *s2 = (unsigned char *)calloc(N_local - M_local - M_global, sizeof(unsigned char));
        unsigned char *p2 = (unsigned char *)calloc(M_local, sizeof(unsigned char));
        unsigned char *c2 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *p0 = (unsigned char *)calloc(M_global, sizeof(unsigned char));
        double *receive_buf = (double *)calloc(N, sizeof(double));
        double *receive_buf_1 = (double *)calloc(N_local, sizeof(double));
        double *receive_buf_2 = (double *)calloc(N_local, sizeof(double));
        unsigned char *corrected_buf = (unsigned char *)calloc(N, sizeof(unsigned char));
        unsigned char *corrected_buf_1 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *corrected_buf_2 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *c = (unsigned char *)calloc(N, sizeof(unsigned char));

        long long ldpc_fail = 0, error_bit = 0;
        long long decode_iter_sum = 0, global_decode_iter_sum = 0;
        cout << "current Nano time seed: " << seed << endl;
        cout << "start " << iteration << " times ldpc simulations..." << endl;
        auto start = chrono::system_clock::to_time_t(chrono::system_clock::now());

        // 6. random generate info bits and encode information
            // a. random generate info bits
            for (long long i = 0; i < N_local - M_local; i++)
            {
                if (u(e) % 2 == 0){
                    s1[i] = (unsigned char)0;
                }
                else{
                    s1[i] = (unsigned char)1;
                }
            }
            for (long long i = 0; i <  N_local - M_local - M_global; i++)
            {
                if (u(e) % 2 == 0)
                    s2[i] = (unsigned char)0;
                else
                    s2[i] = (unsigned char)1;
            }

            // b. encode the 1st infomation bit
            encoder->encode1(s1, p1);

            // c. combine the info and parity, get c1 and first part of c
            for( int i = 0; i < N_local; i++){
                if( i < N_local - M_local){
                    c1[i] = s1[i];
                }
                else{
                    c1[i] = p1[i - (N_local - M_local)];
                }
                c[i] = c1[i];
            }

            // d. encode the last information bit
            encoder->encode_2local(c1, s2, p2, p0);

            // e. combine the code word, to get c2 and second part of c
            for (int i = 0; i < N_local - M_local - M_global; i++){
                c2[i] = s2[i];
                c[i + N_local] = c2[i];
            }
            for (int i = N_local - M_local - M_global; i < N_local - M_global; i++){
                c2[i] = p2[i - (N_local - M_local - M_global)];
                c[i + N_local] = c2[i];
            }
            for (int i = N_local - M_global; i < N_local; i++){
                c2[i] = p0[i - (N_local - M_global)];
                c[i + N_local] = c2[i];
            }

        // 7. simulate multiple times [data storage and data access]
        for (long long iter = 0; iter < iteration; iter++)
        {
            int decode_iter_local_max = 0;
            if ((iter % (iteration / 4)) == (iteration / 4 - 1))
            {
                auto cur_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
                cout << "===> " << (100 * iter) / iteration + 1 << "\% complete, " << "cost " << cur_t - start << " s" << endl;
            }

            // a. genarate failure
            model->GCgenerateFailure(c, receive_buf, N);

            // b. decode information only using global matrix
            int decode_iter_global = decoder->decode_global(receive_buf, corrected_buf, encoder->getGlobalRowMat(), encoder->getGlobalColMat(), alpha, beta);

            // c. check whether the result is right and count errors (info + parity)
            if (decode_iter_global == -1)
            {
                decode_iter_sum += max_decode_iter_global;
                int this_error_bit = 0;
                for (int i = 0; i < N_local - M_local; i++)
                {
                    if (s1[i] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N_local - M_local; i < N_local; i++)
                {
                    if (p1[i - N_local + M_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N_local; i < N - M_local - M_global; i++)
                {
                    if (s2[i - N_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N - M_local - M_global; i < N - M_global; i++)
                {
                    if (p2[i - N + M_local + M_global] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N - M_global; i < N; i++)
                {
                    if (p0[i - N + M_global] != corrected_buf[i])
                        this_error_bit++;
                }
                if (this_error_bit > max_error_bit)
                {
                    max_error_bit = this_error_bit;
                    max_error_bit_index = ldpc_fail;
                }
                error_bit += this_error_bit;
                ldpc_fail++;
            }
            else{
                decode_iter_sum += decode_iter_global;
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
        cout << "global decode iter sum = " << decode_iter_sum << endl;
        cout << "global avg iter num = " << (double)decode_iter_sum / iteration << endl;
        cout << "frame error number = " << ldpc_fail << endl;
        cout << "frame error rate (FER) = " << (double)ldpc_fail / (double)iteration << endl;
        cout << "uncorrectable bit error number = " << error_bit << endl;
        cout << "uncorrectable bit error rate (UBER) = " << (double)error_bit / (double)(iteration * N) << endl;

        // 8. clean buffer
        delete conf;
        free(s1);
        free(p1);
        free(c1);
        free(s2);
        free(p2);
        free(c2);
        free(p0);
        delete encoder_factory;
        delete encoder;
        delete failure_factory;
        delete model;
        free(receive_buf);
        free(receive_buf_1);
        free(receive_buf_2);
        delete decoder_factory;
        delete decoder;
        free(corrected_buf);
        free(corrected_buf_1);
        free(corrected_buf_2);
        free(c);
    }

    // when local_matrix_number = 3
    else if(local_matrix_number == 3){
        cout << "--- gc-ldpc simulator's configuration ---" << endl;
        cout << "local_matrix_number: " << local_matrix_number << endl;
        cout << "first_local_information_bits: " << N_local - M_local << endl;
        cout << "first_local_parity_bits: " << M_local << endl;
        cout << "second_local_information_bits: " << N_local - M_local << endl;
        cout << "second_local_parity_bits: " << M_local << endl;
        cout << "third_local_information_bits: " << N_local - M_local - M_global << endl;
        cout << "third_local_parity_bits: " << M_local << endl;
        cout << "global_parity_bits: " << M_global << endl;
        cout << "ldpc_encoder_name: " << gc_ldpc_encoder_name << endl;
        cout << "ldpc_encoder_param: " << gc_ldpc_encoder_param << endl;
        cout << "failure_model_name: " << gc_failure_model_name << endl;
        cout << "failure_model_param: " << gc_failure_model_param << endl;
        cout << "ldpc_decoder_name: " << gc_ldpc_decoder_name << endl;
        cout << "ldpc_decoder_param: " << gc_ldpc_decoder_param << endl;
        cout << "rber_rate: " << rber_rate << endl;
        cout << "msa_beta: " << beta << endl;
        cout << "msa_alpha: " << alpha << endl;
        cout << "iteration: " << iteration << endl
        << endl;

        // 2. create encoder
        GC_EncoderFactory *encoder_factory = new GC_EncoderFactory();
        GC_EncoderBase *encoder = encoder_factory->createGCEncoder(gc_ldpc_encoder_name, gc_ldpc_encoder_param);

        // 3. create failure model
        GC_FailureFactory *failure_factory = new GC_FailureFactory();
        GC_FailureBase *model = failure_factory->createGCFailureModel(gc_failure_model_name, gc_failure_model_param);

        // 4. create decoder
        GC_DecoderFactory *decoder_factory = new GC_DecoderFactory();
        GC_DecoderBase *decoder = decoder_factory->createGCDecoder(gc_ldpc_decoder_name, gc_ldpc_decoder_param, *encoder, alpha);

        // 5. prepare data buffer
        unsigned char *s1 = (unsigned char *)calloc(N_local - M_local, sizeof(unsigned char));
        unsigned char *p1 = (unsigned char *)calloc(M_local, sizeof(unsigned char));
        unsigned char *c1 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *s2 = (unsigned char *)calloc(N_local - M_local, sizeof(unsigned char));
        unsigned char *p2 = (unsigned char *)calloc(M_local, sizeof(unsigned char));
        unsigned char *c2 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *s3 = (unsigned char *)calloc(N_local - M_local - M_global, sizeof(unsigned char));
        unsigned char *p3 = (unsigned char *)calloc(M_local, sizeof(unsigned char));
        unsigned char *p0 = (unsigned char *)calloc(M_global, sizeof(unsigned char));
        unsigned char *c3 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        double *receive_buf = (double *)calloc(N, sizeof(double));
        double *receive_buf_1 = (double *)calloc(N_local, sizeof(double));
        double *receive_buf_2 = (double *)calloc(N_local, sizeof(double));
        double *receive_buf_3 = (double *)calloc(N_local, sizeof(double));
        unsigned char *corrected_buf = (unsigned char *)calloc(N, sizeof(unsigned char));
        unsigned char *corrected_buf_1 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *corrected_buf_2 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *corrected_buf_3 = (unsigned char *)calloc(N_local, sizeof(unsigned char));
        unsigned char *c = (unsigned char *)calloc(N, sizeof(unsigned char));

        long long ldpc_fail = 0, error_bit = 0;
        long long decode_iter_sum = 0, global_decode_iter_sum = 0;
        cout << "current Nano time seed: " << seed << endl;
        cout << "start " << iteration << " times ldpc simulations..." << endl;
        auto start = chrono::system_clock::to_time_t(chrono::system_clock::now());

         // 6. random generate info bits and encode information
            // a. random generate info bits
            for (long long i = 0; i < N_local - M_local; i++)
            {
                if (u(e) % 2 == 0){
                    s1[i] = (unsigned char)0;
                }
                else{
                    s1[i] = (unsigned char)1;
                }
            }
            for (long long i = 0; i < N_local - M_local; i++)
            {
                if (u(e) % 2 == 0){
                    s2[i] = (unsigned char)0;
                }
                else{
                    s2[i] = (unsigned char)1;
                }
            }
            for (long long i = 0; i <  N_local - M_local - M_global; i++)
            {
                if (u(e) % 2 == 0){
                     s3[i] = (unsigned char)0;
                }
                else{
                      s3[i] = (unsigned char)1;
                }
            }

            // b. encode the info s1 and s2
            encoder->encode1(s1,p1);
            encoder->encode2(s2,p2);

            // c. comine the info s1,p2 and parity p1,p2
            for( int i = 0; i < N_local; i++){
                if( i < N_local - M_local){
                    c1[i] = s1[i];
                }
                else{
                    c1[i] = p1[i - (N_local - M_local)];
                }
                c[i] = c1[i];
            }
             for( int i = 0; i < N_local; i++){
                if( i < N_local - M_local){
                    c2[i] = s2[i];
                }
                else{
                    c2[i] = p2[i - (N_local - M_local)];
                }
                c[i + N_local] = c2[i];
            }

            // d. encode the info s3
            encoder->encode_3local(c1, c2, s3, p3, p0);

            // e. combine the info s3 and parity p3,p0
            for (int i = 0; i < N_local - M_local - M_global; i++){
                c3[i] = s3[i];
                c[i + 2 * N_local] = c3[i];
            }
            for (int i = N_local - M_local - M_global; i < N_local - M_global; i++){
                c3[i] = p3[i - (N_local - M_local - M_global)];
                c[i + 2 * N_local] = c3[i];
            }
            for (int i = N_local - M_global; i < N_local; i++){
                c3[i] = p0[i - (N_local - M_global)];
                c[i + 2 * N_local] = c3[i];
            }

        // 7. simulate multiple times [data storage and data access]
        for (long long iter = 0; iter < iteration; iter++)
        {
            int decode_iter_local_max = 0;
            if ((iter % (iteration / 4)) == (iteration / 4 - 1))
            {
                auto cur_t = chrono::system_clock::to_time_t(chrono::system_clock::now());
                cout << "===> " << (100 * iter) / iteration + 1 << "\% complete, " << "cost " << cur_t - start << " s" << endl;
            }

            // a. genarate failure
            model->GCgenerateFailure(c, receive_buf, N);

            // d. decode information using global matrix
            int decode_iter_global = decoder->decode_global(receive_buf, corrected_buf, encoder->getGlobalRowMat(), encoder->getGlobalColMat(), alpha, beta);

            // c. check whether the result is right and count errors (info + parity)
            if (decode_iter_global == -1)
            {
                decode_iter_sum += max_decode_iter_global;
                int this_error_bit = 0;
                for (int i = 0; i < N_local - M_local; i++)
                {
                    if (s1[i] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N_local - M_local; i < N_local; i++)
                {
                    if (p1[i - N_local + M_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = N_local; i < 2 * N_local - M_local; i++)
                {
                    if (s2[i - N_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = 2 * N_local - M_local; i < 2 * N_local; i++)
                {
                    if (p2[i - 2 * N_local + M_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = 2 * N_local; i < 3 * N_local - M_local - M_global; i++)
                {
                    if (s3[i - 2 * N_local] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = 3 * N_local - M_local - M_global; i < 3 * N_local - M_global; i++)
                {
                    if (p3[i - 3 * N_local + M_local + M_global] != corrected_buf[i])
                        this_error_bit++;
                }
                for (int i = 3 * N_local - M_global; i < 3 * N_local; i++)
                {
                    if (p0[i - 3 * N_local + M_global] != corrected_buf[i])
                        this_error_bit++;
                }
                if (this_error_bit > max_error_bit)
                {
                    max_error_bit = this_error_bit;
                    max_error_bit_index = ldpc_fail;
                }
                error_bit += this_error_bit;
                ldpc_fail++;
            }
            else{
                decode_iter_sum += decode_iter_global;
            }

            if (iter % 10000 == 0 && iter != 0)
            {
                cout << iter / 100000 << ":frame error rate (FER) = " << (double)ldpc_fail / (double)iter << endl;
                cout << iter / 100000 << ":uncorrectable bit error rate (UBER) = " << (double)error_bit / (double)(iter * N) << endl;
            }
        }
        auto end = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        cout << "finish " << iteration << " times ldpc simulations, cost " << end - start << " s" << endl
         << endl;
        cout << "max error bit = " << max_error_bit << ", index = " << max_error_bit_index << endl;
        cout << "global decode iter sum = " << decode_iter_sum << endl;
        cout << "global avg iter num = " << (double)decode_iter_sum / iteration << endl;
        cout << "frame error number = " << ldpc_fail << endl;
        cout << "frame error rate (FER) = " << (double)ldpc_fail / (double)iteration << endl;
        cout << "uncorrectable bit error number = " << error_bit << endl;
        cout << "uncorrectable bit error rate (UBER) = " << (double)error_bit / (double)(iteration * N) << endl;

        // 8. clean buffer
        delete conf;
        free(s1);
        free(p1);
        free(c1);
        free(s2);
        free(p2);
        free(c2);
        free(s3);
        free(p3);
        free(p0);
        free(c3);
        delete encoder_factory;
        delete encoder;
        delete failure_factory;
        delete model;
        free(receive_buf);
        free(receive_buf_1);
        free(receive_buf_2);
        free(receive_buf_3);
        delete decoder_factory;
        delete decoder;
        free(corrected_buf);
        free(corrected_buf_1);
        free(corrected_buf_2);
        free(corrected_buf_3);
        free(c);
    }

    // input wrong param
    else{
        cout << "wrong gc-ldpc encoder param" << endl;
        cout << "support 2 local matrix or 3 local matrix only" << endl;
        return -1;
    }

    return 0;
}