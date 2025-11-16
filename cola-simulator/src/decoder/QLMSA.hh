#ifndef _QLMSA_HH_
#define _QLMSA_HH_

#include "../inc/include.hh"
#include "DecoderBase.hh"
#include "../encoder/EncoderBase.hh"

using namespace std;

class QLMSA : public DecoderBase
{
private:
    int _round; // decoder max iter
    int quan_bit_in_decoder;
    int quan_bit_in_initllr;
    int max_abs_val_in_decoder;
    int range_size_in_decoder;
    vector<int>
        _norm_quan_index;
    vector<vector<vector<int>>> column_to_column_index;

public:
    QLMSA(EncoderBase &encoderInstance, string param, double alapha);
    void init_norm_quan_index(double alpha);
    void init_column_to_column_index(EncoderBase &encoder);
    int decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);

protected:
    EncoderBase &encoder;
};

#endif