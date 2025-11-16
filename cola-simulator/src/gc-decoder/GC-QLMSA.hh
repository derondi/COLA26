#ifndef _GC_QLMSA_HH_
#define _GC_QLMSA_HH_

#include "../inc/include.hh"
#include "GC-DecoderBase.hh"
#include "../gc-encoder/GC-EncoderBase.hh"

using namespace std;

class GC_QLMSA : public GC_DecoderBase
{
private:
    int _round; // decoder max iter
    int _round_local; // local decoder max iter
    int _round_global; // global decoder max iter
    int quan_bit_in_decoder;
    int quan_bit_in_initllr;
    int max_abs_val_in_decoder;
    int range_size_in_decoder;
    vector<int> _norm_quan_index;
    vector<vector<vector<int>>> column_to_column_index_local_1;
    vector<vector<vector<int>>> column_to_column_index_local_2;
    vector<vector<vector<int>>> column_to_column_index_local_t;
    vector<vector<vector<int>>> column_to_column_index_global;

public:
    GC_QLMSA(GC_EncoderBase &encoderInstance, string param, double alapha);
    void init_norm_quan_index(double alpha);
    void init_column_to_column_index_local_1(GC_EncoderBase &encoder);
    void init_column_to_column_index_local_2(GC_EncoderBase &encoder);
    void init_column_to_column_index_local_t(GC_EncoderBase &encoder);
    void init_column_to_column_index_global(GC_EncoderBase &encoder);
    int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);

protected:
    GC_EncoderBase &encoder;
};

#endif