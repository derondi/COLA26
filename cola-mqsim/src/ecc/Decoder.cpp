#include "Decoder.h"
#include "Utils.h"

#include <cmath>
#include <cstring>

DecoderBase::DecoderBase()
{
}

DecoderBase::DecoderBase(string param)
{
}

DecoderBase::~DecoderBase()
{
}

// check corrected buf whether satisfy the H matrix restriction
bool DecoderBase::check(unsigned char *corrected, vector<vector<int>> &_row_mat)
{
    bool check = true;
    for (int i = 0; i < _row_mat.size(); i++)
    {
        int c = 0;
        for (int j = 0; j < _row_mat[i].size(); j++)
        {
            int colIdx = _row_mat[i][j];
            c += corrected[colIdx];
        }
        if (c % 2 == 1)
        {
            check = false;
            break;
        }
    }
    return check;
}

GDBF::GDBF(string param)
{
    // parse param for GDBF algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round_local = stoi(v[0]);
    _round_global = stoi(v[1]);
}

// Gradient Descent Bit Flipping
int GDBF::decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
    int row = _row_mat.size();
    int column = _column_mat.size();
    // result of syn check
    vector<int> syn_check(row);
    // energy list of bit
    vector<int> energylist(column);
    // bitlist
    vector<int> bitlist(column);
    // input y
    vector<int> y(column);
    // initialize bitlist using input y
    for (int i = 0; i < column; i++){
        y[i] = (receive[i] > 0) ? 0 : 1;
        bitlist[i] = y[i];
        corrected[i] = y[i];
    }
    if (check(corrected,_row_mat))
        return 0;
    int iter = 0;
    for (; iter < _round_local;){
        iter++;
        // compute the syn check result
        for (int i = 0; i < row; i++){
            syn_check[i] = 0;
            for(int j = 0;j < _row_mat[i].size();j++){
                int colIdx = _row_mat[i][j];
                syn_check[i] = syn_check[i]^bitlist[colIdx];
            }
        }
        // initialize flipping threshold
        int FlipThd = 0;
        // compute energy of bit
        for (int j = 0; j < column; j++){
            energylist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                energylist[j] = energylist[j] + syn_check[rowIdx];
            }
            int feadback = bitlist[j]^y[j];
            energylist[j] = energylist[j] + feadback;
            // update flip threshold
            if( FlipThd < energylist[j]){
                FlipThd = energylist[j];
                }
        }
        // do the bit flipping
        for (int i = 0; i < column; i++){
            if(FlipThd == energylist[i])
                bitlist[i] = 1^bitlist[i];
            corrected[i] = bitlist[i];
        }
        // check
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;
}

int GDBF::decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
    int row = _row_mat.size();
    int column = _column_mat.size();
    // result of syn check
    vector<int> syn_check(row);
    // energy list of bit
    vector<int> energylist(column);
    // bitlist
    vector<int> bitlist(column);
    // input y
    vector<int> y(column);
    // initialize bitlist using input y
    for (int i = 0; i < column; i++){
        y[i] = (receive[i] > 0) ? 0 : 1;
        bitlist[i] = y[i];
        corrected[i] = y[i];
    }
    if (check(corrected,_row_mat))
        return 0;
    int iter = 0;
    for (; iter < _round_local;){
        iter++;
        // compute the syn check result
        for (int i = 0; i < row; i++){
            syn_check[i] = 0;
            for(int j = 0;j < _row_mat[i].size();j++){
                int colIdx = _row_mat[i][j];
                syn_check[i] = syn_check[i]^bitlist[colIdx];
            }
        }
        // initialize flipping threshold
        int FlipThd = 0;
        // compute energy of bit
        for (int j = 0; j < column; j++){
            energylist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                energylist[j] = energylist[j] + syn_check[rowIdx];
            }
            int feadback = bitlist[j]^y[j];
            energylist[j] = energylist[j] + feadback;
            // update flip threshold
            if( FlipThd < energylist[j]){
                FlipThd = energylist[j];
                }
        }
        // do the bit flipping
        for (int i = 0; i < column; i++){
            if(FlipThd == energylist[i])
                bitlist[i] = 1^bitlist[i];
            corrected[i] = bitlist[i];
        }
        // check
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;
}

int GDBF::decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
    int row = _row_mat.size();
    int column = _column_mat.size();
    // result of syn check
    vector<int> syn_check(row);
    // energy list of bit
    vector<int> energylist(column);
    // bitlist
    vector<int> bitlist(column);
    // input y
    vector<int> y(column);
    // initialize bitlist using input y
    for (int i = 0; i < column; i++){
        y[i] = (receive[i] > 0) ? 0 : 1;
        bitlist[i] = y[i];
        corrected[i] = y[i];
    }
    if (check(corrected,_row_mat))
        return 0;
    int iter = 0;
    for (; iter < _round_local;){
        iter++;
        // compute the syn check result
        for (int i = 0; i < row; i++){
            syn_check[i] = 0;
            for(int j = 0;j < _row_mat[i].size();j++){
                int colIdx = _row_mat[i][j];
                syn_check[i] = syn_check[i]^bitlist[colIdx];
            }
        }
        // initialize flipping threshold
        int FlipThd = 0;
        // compute energy of bit
        for (int j = 0; j < column; j++){
            energylist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                energylist[j] = energylist[j] + syn_check[rowIdx];
            }
            int feadback = bitlist[j]^y[j];
            energylist[j] = energylist[j] + feadback;
            // update flip threshold
            if( FlipThd < energylist[j]){
                FlipThd = energylist[j];
                }
        }
        // do the bit flipping
        for (int i = 0; i < column; i++){
            if(FlipThd == energylist[i])
                bitlist[i] = 1^bitlist[i];
            corrected[i] = bitlist[i];
        }
        // check
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;
}

int GDBF::decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
    int row = _row_mat.size();
    int column = _column_mat.size();
    // result of syn check
    vector<int> syn_check(row);
    // energy list of bit
    vector<int> energylist(column);
    // bitlist
    vector<int> bitlist(column);
    // input y
    vector<int> y(column);
    // initialize bitlist using input y
    for (int i = 0; i < column; i++){
        y[i] = (receive[i] > 0) ? 0 : 1;
        bitlist[i] = y[i];
        corrected[i] = y[i];
    }
    if (check(corrected, _row_mat))
        return 0;
    int iter = 0;
    for (; iter < _round_global;){
        iter++;
        // compute the syn check result
        for (int i = 0; i < row; i++){
            syn_check[i] = 0;
            for(int j = 0;j < _row_mat[i].size();j++){
                int colIdx = _row_mat[i][j];
                syn_check[i] = syn_check[i]^bitlist[colIdx];
            }
        }
        // initialize flipping threshold
        int FlipThd = 0;
        // compute energy of bit
        for (int j = 0; j < column; j++){
            energylist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                energylist[j] = energylist[j] + syn_check[rowIdx];
            }
            int feadback = bitlist[j]^y[j];
            energylist[j] = energylist[j] + feadback;
            // update flip threshold
            if( FlipThd < energylist[j]){
                FlipThd = energylist[j];
                }
        }
        // do the bit flipping
        for (int i = 0; i < column; i++){
            if(FlipThd == energylist[i])
                bitlist[i] = 1^bitlist[i];
            corrected[i] = bitlist[i];
        }
        // check
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;
}

// QLMSA
QLMSA::QLMSA(EncoderBase &encoderInstance, string param, string alpha_param) : encoder(encoderInstance)
{
    // parse param for QLMSA algorithm
    quan_bit_in_initllr = 2;
    quan_bit_in_decoder = 4;
    max_abs_val_in_decoder = (1 << (quan_bit_in_decoder - 1)) - 1; // 2^(4-1) - 1
    range_size_in_decoder = (1 << quan_bit_in_decoder) - 1;        // 2^4 - 1
    int _t = encoder.getLocalMatrixNumber();
    vector<string> v;
    splitString(param, v, ";");
    _round_local = stoi(v[0]);
    _round_global = stoi(v[1]);
    vector<string> x;
    splitString(alpha_param, x, ";");
    double alpha_local = atof(x[0].c_str());
    double alpha_global = atof(x[1].c_str());
    //init_norm_quan_index(alpha);
    init_norm_quan_index_local(alpha_local);
    init_norm_quan_index_global(alpha_global);
    if (_t == 2){
        init_column_to_column_index_local_1(encoder);
    }
    if (_t == 3){
        init_column_to_column_index_local_1(encoder);
        init_column_to_column_index_local_2(encoder);
    }
    init_column_to_column_index_local_t(encoder);
    init_column_to_column_index_global(encoder);
}

void QLMSA::init_column_to_column_index_local_1(EncoderBase &encoder)
{
    int column = encoder.getBlockLengthLocal();
    vector<vector<int>> _row_mat = encoder.getRowMat1();
    vector<vector<int>> _column_mat = encoder.getColMat1();
    column_to_column_index_local_1.resize(column);
    for (int i = 0; i < column; i++)
    {
        column_to_column_index_local_1[i].resize(_column_mat[i].size());
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            column_to_column_index_local_1[i][j].resize(_row_mat[rowIdx].size());
            for (int k = 0; k < _row_mat[rowIdx].size(); k++)
            {
                int colIdx = _row_mat[rowIdx][k];
                if (i == colIdx)
                    continue;
                int g = 0;
                for (; g < _column_mat[colIdx].size();)
                {
                    if (_column_mat[colIdx][g] == rowIdx)
                    {
                        break;
                    }
                    g++;
                }
                column_to_column_index_local_1[i][j][k] = g;
            }
        }
    }
}

void QLMSA::init_column_to_column_index_local_2(EncoderBase &encoder)
{
    int column = encoder.getBlockLengthLocal();
    vector<vector<int>> _row_mat = encoder.getRowMat2();
    vector<vector<int>> _column_mat = encoder.getColMat2();
    column_to_column_index_local_2.resize(column);
    for (int i = 0; i < column; i++)
    {
        column_to_column_index_local_2[i].resize(_column_mat[i].size());
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            column_to_column_index_local_2[i][j].resize(_row_mat[rowIdx].size());
            for (int k = 0; k < _row_mat[rowIdx].size(); k++)
            {
                int colIdx = _row_mat[rowIdx][k];
                if (i == colIdx)
                    continue;
                int g = 0;
                for (; g < _column_mat[colIdx].size();)
                {
                    if (_column_mat[colIdx][g] == rowIdx)
                    {
                        break;
                    }
                    g++;
                }
                column_to_column_index_local_2[i][j][k] = g;
            }
        }
    }
}

void QLMSA::init_column_to_column_index_local_t(EncoderBase &encoder)
{
    int column = encoder.getBlockLengthLocal();
    vector<vector<int>> _row_mat = encoder.getTRowMat();
    vector<vector<int>> _column_mat = encoder.getTColMat();
    column_to_column_index_local_t.resize(column);
    for (int i = 0; i < column; i++)
    {
        column_to_column_index_local_t[i].resize(_column_mat[i].size());
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            column_to_column_index_local_t[i][j].resize(_row_mat[rowIdx].size());
            for (int k = 0; k < _row_mat[rowIdx].size(); k++)
            {
                int colIdx = _row_mat[rowIdx][k];
                if (i == colIdx)
                    continue;
                int g = 0;
                for (; g < _column_mat[colIdx].size();)
                {
                    if (_column_mat[colIdx][g] == rowIdx)
                    {
                        break;
                    }
                    g++;
                }
                column_to_column_index_local_t[i][j][k] = g;
            }
        }
    }
}

void QLMSA::init_column_to_column_index_global(EncoderBase &encoder)
{
    int column = encoder.getBlockLengthTotal();
    vector<vector<int>> _row_mat = encoder.getGlobalRowMat();
    vector<vector<int>> _column_mat = encoder.getGlobalColMat();
    column_to_column_index_global.resize(column);
    for (int i = 0; i < column; i++)
    {
        column_to_column_index_global[i].resize(_column_mat[i].size());
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            column_to_column_index_global[i][j].resize(_row_mat[rowIdx].size());
            for (int k = 0; k < _row_mat[rowIdx].size(); k++)
            {
                int colIdx = _row_mat[rowIdx][k];
                if (i == colIdx)
                    continue;
                int g = 0;
                for (; g < _column_mat[colIdx].size();)
                {
                    if (_column_mat[colIdx][g] == rowIdx)
                    {
                        break;
                    }
                    g++;
                }
                column_to_column_index_global[i][j][k] = g;
            }
        }
    }
}

void QLMSA::init_norm_quan_index_local(double alpha_local)
{
    _norm_quan_index_local.resize(range_size_in_decoder);
    for (int i = -max_abs_val_in_decoder; i <= max_abs_val_in_decoder; i++)
    {
        int tmp = i;
        if (tmp > 0)
            tmp = std::ceil(alpha_local * tmp);
        else if (tmp < 0)
            tmp = std::ceil(alpha_local * tmp) - 1;
        else
            tmp = 0;
        _norm_quan_index_local[i + max_abs_val_in_decoder] = tmp;
    }
}

void QLMSA::init_norm_quan_index_global(double alpha_global)
{
    _norm_quan_index_global.resize(range_size_in_decoder);
    for (int i = -max_abs_val_in_decoder; i <= max_abs_val_in_decoder; i++)
    {
        int tmp = i;
        if (tmp > 0)
            tmp = std::ceil(alpha_global * tmp);
        else if (tmp < 0)
            tmp = std::ceil(alpha_global * tmp) - 1;
        else
            tmp = 0;
        _norm_quan_index_global[i + max_abs_val_in_decoder] = tmp;
    }
}

// belief propagation algorithm (quantized layered min sum)
int QLMSA::decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local)
{
    int row = _row_mat.size();
    int column = _column_mat.size();

    vector<vector<int>> check_to_value_mat(column);
    vector<vector<int>> value_to_check_mat(column);
    vector<int> updated_llr(column);
    vector<int> llr_list(column);
    vector<int> signal_list;
    vector<int> min_list;

    // Init llr list
    for (int i = 0; i < column; i++)
    {
        if (receive[i] > beta_local)
            llr_list[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta_local)
            llr_list[i] = 1;
        else if (receive[i] > -beta_local && receive[i] <= 0)
            llr_list[i] = -1;
        else
            llr_list[i] = -4;
    }
    memcpy(&updated_llr[0], &llr_list[0], column * sizeof(int));

    // Init check to value mat & value to check mat
    for (int i = 0; i < column; i++)
    {
        check_to_value_mat[i].resize(_column_mat[i].size(), 0);
        value_to_check_mat[i].resize(_column_mat[i].size(), 0);
        for (int j = 0; j < _column_mat[i].size(); j++)
            value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
    }

    // Init siglnal list & min list
    signal_list.resize(row, 1);
    min_list.resize(row, max_abs_val_in_decoder);
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            if (value_to_check_mat[i][j] < 0)
                signal_list[rowIdx] *= -1;
            if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                min_list[rowIdx] = abs(value_to_check_mat[i][j]);
        }
    }

    int iter = 0;
    for (; iter < _round_local;)
    {
        iter++;
        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                int tmp = max_abs_val_in_decoder;
                if (abs(value_to_check_mat[i][j]) != min_list[rowIdx])
                    tmp = min_list[rowIdx];
                else
                {
                    for (int k = 0; k < _row_mat[rowIdx].size(); k++)
                    {
                        int colIdx = _row_mat[rowIdx][k];
                        if (i == colIdx)
                            continue;
                        int g = column_to_column_index_local_1[i][j][k];
                        int l1 = value_to_check_mat[colIdx][g];
                        tmp = min(abs(l1), tmp);
                        if (tmp == min_list[rowIdx])
                            break;
                    }
                    min_list[rowIdx] = tmp;
                }
                tmp *= signal_list[rowIdx];
                if (value_to_check_mat[i][j] < 0)
                    tmp *= -1;
                tmp = _norm_quan_index_local[tmp + max_abs_val_in_decoder];
                check_to_value_mat[i][j] = tmp;
            }
            // update llr
            updated_llr[i] = llr_list[i];
            for (int j = 0; j < _column_mat[i].size(); j++)
                updated_llr[i] = updated_llr[i] + check_to_value_mat[i][j];
            if (updated_llr[i] > max_abs_val_in_decoder)
                updated_llr[i] = max_abs_val_in_decoder;
            else if (updated_llr[i] < -max_abs_val_in_decoder)
                updated_llr[i] = -max_abs_val_in_decoder;

            // update value_to_check_mat & signal list & min list
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                    min_list[rowIdx] = abs(value_to_check_mat[i][j]);
            }
        }

        // hard-decision decoding
        for (int i = 0; i < column; i++)
        {
            if (updated_llr[i] > 0)
                corrected[i] = 0;
            else
                corrected[i] = 1;
        }
        // check whether decode correctly
        if (check(corrected, _row_mat))
        {
            return iter;
        }
    } // Iteration loop end
    return -1;
}

int QLMSA::decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local)
{
    int row = _row_mat.size();
    int column = _column_mat.size();

    vector<vector<int>> check_to_value_mat(column);
    vector<vector<int>> value_to_check_mat(column);
    vector<int> updated_llr(column);
    vector<int> llr_list(column);
    vector<int> signal_list;
    vector<int> min_list;

    // Init llr list
    for (int i = 0; i < column; i++)
    {
        if (receive[i] > beta_local)
            llr_list[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta_local)
            llr_list[i] = 1;
        else if (receive[i] > -beta_local && receive[i] <= 0)
            llr_list[i] = -1;
        else
            llr_list[i] = -4;
    }
    memcpy(&updated_llr[0], &llr_list[0], column * sizeof(int));

    // Init check to value mat & value to check mat
    for (int i = 0; i < column; i++)
    {
        check_to_value_mat[i].resize(_column_mat[i].size(), 0);
        value_to_check_mat[i].resize(_column_mat[i].size(), 0);
        for (int j = 0; j < _column_mat[i].size(); j++)
            value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
    }

    // Init siglnal list & min list
    signal_list.resize(row, 1);
    min_list.resize(row, max_abs_val_in_decoder);
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            if (value_to_check_mat[i][j] < 0)
                signal_list[rowIdx] *= -1;
            if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                min_list[rowIdx] = abs(value_to_check_mat[i][j]);
        }
    }

    int iter = 0;
    for (; iter < _round_local;)
    {
        iter++;
        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                int tmp = max_abs_val_in_decoder;
                if (abs(value_to_check_mat[i][j]) != min_list[rowIdx])
                    tmp = min_list[rowIdx];
                else
                {
                    for (int k = 0; k < _row_mat[rowIdx].size(); k++)
                    {
                        int colIdx = _row_mat[rowIdx][k];
                        if (i == colIdx)
                            continue;
                        int g = column_to_column_index_local_2[i][j][k];
                        int l1 = value_to_check_mat[colIdx][g];
                        tmp = min(abs(l1), tmp);
                        if (tmp == min_list[rowIdx])
                            break;
                    }
                    min_list[rowIdx] = tmp;
                }
                tmp *= signal_list[rowIdx];
                if (value_to_check_mat[i][j] < 0)
                    tmp *= -1;
                tmp = _norm_quan_index_local[tmp + max_abs_val_in_decoder];
                check_to_value_mat[i][j] = tmp;
            }
            // update llr
            updated_llr[i] = llr_list[i];
            for (int j = 0; j < _column_mat[i].size(); j++)
                updated_llr[i] = updated_llr[i] + check_to_value_mat[i][j];
            if (updated_llr[i] > max_abs_val_in_decoder)
                updated_llr[i] = max_abs_val_in_decoder;
            else if (updated_llr[i] < -max_abs_val_in_decoder)
                updated_llr[i] = -max_abs_val_in_decoder;

            // update value_to_check_mat & signal list & min list
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                    min_list[rowIdx] = abs(value_to_check_mat[i][j]);
            }
        }

        // hard-decision decoding
        for (int i = 0; i < column; i++)
        {
            if (updated_llr[i] > 0)
                corrected[i] = 0;
            else
                corrected[i] = 1;
        }
        // check whether decode correctly
        if (check(corrected, _row_mat))
        {
            return iter;
        }
    } // Iteration loop end
    return -1;
}

// belief propagation algorithm (quantized layered min sum)
int QLMSA::decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_local, double beta_local)
{
    int row = _row_mat.size();
    int column = _column_mat.size();

    vector<vector<int>> check_to_value_mat(column);
    vector<vector<int>> value_to_check_mat(column);
    vector<int> updated_llr(column);
    vector<int> llr_list(column);
    vector<int> signal_list;
    vector<int> min_list;

    // Init llr list
    for (int i = 0; i < column; i++)
    {
        if (receive[i] > beta_local)
            llr_list[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta_local)
            llr_list[i] = 1;
        else if (receive[i] > -beta_local && receive[i] <= 0)
            llr_list[i] = -1;
        else
            llr_list[i] = -4;
    }
    memcpy(&updated_llr[0], &llr_list[0], column * sizeof(int));

    // Init check to value mat & value to check mat
    for (int i = 0; i < column; i++)
    {
        check_to_value_mat[i].resize(_column_mat[i].size(), 0);
        value_to_check_mat[i].resize(_column_mat[i].size(), 0);
        for (int j = 0; j < _column_mat[i].size(); j++)
            value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
    }

    // Init siglnal list & min list
    signal_list.resize(row, 1);
    min_list.resize(row, max_abs_val_in_decoder);
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            if (value_to_check_mat[i][j] < 0)
                signal_list[rowIdx] *= -1;
            if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                min_list[rowIdx] = abs(value_to_check_mat[i][j]);
        }
    }

    int iter = 0;
    for (; iter < _round_local;)
    {
        iter++;
        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                int tmp = max_abs_val_in_decoder;
                if (abs(value_to_check_mat[i][j]) != min_list[rowIdx])
                    tmp = min_list[rowIdx];
                else
                {
                    for (int k = 0; k < _row_mat[rowIdx].size(); k++)
                    {
                        int colIdx = _row_mat[rowIdx][k];
                        if (i == colIdx)
                            continue;
                        int g = column_to_column_index_local_t[i][j][k];
                        int l1 = value_to_check_mat[colIdx][g];
                        tmp = min(abs(l1), tmp);
                        if (tmp == min_list[rowIdx])
                            break;
                    }
                    min_list[rowIdx] = tmp;
                }
                tmp *= signal_list[rowIdx];
                if (value_to_check_mat[i][j] < 0)
                    tmp *= -1;
                tmp = _norm_quan_index_local[tmp + max_abs_val_in_decoder];
                check_to_value_mat[i][j] = tmp;
            }
            // update llr
            updated_llr[i] = llr_list[i];
            for (int j = 0; j < _column_mat[i].size(); j++)
                updated_llr[i] = updated_llr[i] + check_to_value_mat[i][j];
            if (updated_llr[i] > max_abs_val_in_decoder)
                updated_llr[i] = max_abs_val_in_decoder;
            else if (updated_llr[i] < -max_abs_val_in_decoder)
                updated_llr[i] = -max_abs_val_in_decoder;

            // update value_to_check_mat & signal list & min list
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                    min_list[rowIdx] = abs(value_to_check_mat[i][j]);
            }
        }

        // hard-decision decoding
        for (int i = 0; i < column; i++)
        {
            if (updated_llr[i] > 0)
                corrected[i] = 0;
            else
                corrected[i] = 1;
        }
        // check whether decode correctly
        if (check(corrected, _row_mat))
        {
            return iter;
        }
    } // Iteration loop end
    return -1;
}

// belief propagation algorithm (quantized layered min sum)
int QLMSA::decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha_global, double beta_global)
{
    int row = _row_mat.size();
    int column = _column_mat.size();

    vector<vector<int>> check_to_value_mat(column);
    vector<vector<int>> value_to_check_mat(column);
    vector<int> updated_llr(column);
    vector<int> llr_list(column);
    vector<int> signal_list;
    vector<int> min_list;

    // Init llr list
    for (int i = 0; i < column; i++)
    {
        if (receive[i] > beta_global)
            llr_list[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta_global)
            llr_list[i] = 1;
        else if (receive[i] > -beta_global && receive[i] <= 0)
            llr_list[i] = -1;
        else
            llr_list[i] = -4;
    }
    memcpy(&updated_llr[0], &llr_list[0], column * sizeof(int));

    // Init check to value mat & value to check mat
    for (int i = 0; i < column; i++)
    {
        check_to_value_mat[i].resize(_column_mat[i].size(), 0);
        value_to_check_mat[i].resize(_column_mat[i].size(), 0);
        for (int j = 0; j < _column_mat[i].size(); j++)
            value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
    }

    // Init siglnal list & min list
    signal_list.resize(row, 1);
    min_list.resize(row, max_abs_val_in_decoder);
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            if (value_to_check_mat[i][j] < 0)
                signal_list[rowIdx] *= -1;
            if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                min_list[rowIdx] = abs(value_to_check_mat[i][j]);
        }
    }

    int iter = 0;
    for (; iter < _round_global;)
    {
        iter++;
        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                int tmp = max_abs_val_in_decoder;
                if (abs(value_to_check_mat[i][j]) != min_list[rowIdx])
                    tmp = min_list[rowIdx];
                else
                {
                    for (int k = 0; k < _row_mat[rowIdx].size(); k++)
                    {
                        int colIdx = _row_mat[rowIdx][k];
                        if (i == colIdx)
                            continue;
                        int g = column_to_column_index_global[i][j][k];
                        int l1 = value_to_check_mat[colIdx][g];
                        tmp = min(abs(l1), tmp);
                        if (tmp == min_list[rowIdx])
                            break;
                    }
                    min_list[rowIdx] = tmp;
                }
                tmp *= signal_list[rowIdx];
                if (value_to_check_mat[i][j] < 0)
                    tmp *= -1;
                tmp = _norm_quan_index_global[tmp + max_abs_val_in_decoder];
                check_to_value_mat[i][j] = tmp;
            }
            // update llr
            updated_llr[i] = llr_list[i];
            for (int j = 0; j < _column_mat[i].size(); j++)
                updated_llr[i] = updated_llr[i] + check_to_value_mat[i][j];
            if (updated_llr[i] > max_abs_val_in_decoder)
                updated_llr[i] = max_abs_val_in_decoder;
            else if (updated_llr[i] < -max_abs_val_in_decoder)
                updated_llr[i] = -max_abs_val_in_decoder;

            // update value_to_check_mat & signal list & min list
            for (int j = 0; j < _column_mat[i].size(); j++)
            {
                int rowIdx = _column_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                value_to_check_mat[i][j] = updated_llr[i] - check_to_value_mat[i][j];
                if (value_to_check_mat[i][j] < 0)
                    signal_list[rowIdx] *= -1;
                if (abs(value_to_check_mat[i][j]) < min_list[rowIdx])
                    min_list[rowIdx] = abs(value_to_check_mat[i][j]);
            }
        }

        // hard-decision decoding
        for (int i = 0; i < column; i++)
        {
            if (updated_llr[i] > 0)
                corrected[i] = 0;
            else
                corrected[i] = 1;
        }
        // check whether decode correctly
        if (check(corrected, _row_mat))
        {
            return iter;
        }
    } // Iteration loop end
    return -1;
}