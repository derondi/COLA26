#ifndef DECODER_H
#define DECODER_H

#include "Encoder.h"
#include <string>
#include <vector>

using namespace std;

class DecoderBase
{
  public:
    DecoderBase();
    DecoderBase(string param);
    virtual ~DecoderBase();

    virtual int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    virtual int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta) = 0;
    bool check(unsigned char *corrected, vector<vector<int>> &_row_mat);
};

// Hard bit
class GDBF : public DecoderBase
{
  private:
    int _round_local;
    int _round_global;

  public:
    GDBF(string param);
    int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
    int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta);
};

class QLMSA : public DecoderBase
{
  private:
    int _round_local; // decoder max iter
    int _round_global;
    int quan_bit_in_decoder;
    int quan_bit_in_initllr;
    int max_abs_val_in_decoder;
    int range_size_in_decoder;
    vector<int> _norm_quan_index_local;
    vector<int> _norm_quan_index_global;
    vector<vector<vector<int>>> column_to_column_index_local_1;
    vector<vector<vector<int>>> column_to_column_index_local_2;
    vector<vector<vector<int>>> column_to_column_index_local_t;
    vector<vector<vector<int>>> column_to_column_index_global;

  public:
    QLMSA(EncoderBase &encoderInstance, string param, string alpha_param);
    void init_norm_quan_index_local(double alpha_local);
    void init_norm_quan_index_global(double alpha_global);
    void init_column_to_column_index_local_1(EncoderBase &encoder);
    void init_column_to_column_index_local_2(EncoderBase &encoder);
    void init_column_to_column_index_local_t(EncoderBase &encoder);
    void init_column_to_column_index_global(EncoderBase &encoder);
    int decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local);
    int decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local);
    int decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local);
    int decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_global, double beta_global);

  protected:
    EncoderBase &encoder;
};

#endif
