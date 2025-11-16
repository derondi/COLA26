#include "QLMSA.hh"

QLMSA::QLMSA(EncoderBase &encoderInstance, string param, double alpha)
    : encoder(encoderInstance)
{
    // parse param for QLMSA algorithm
    quan_bit_in_initllr = 2;
    quan_bit_in_decoder = 4;
    max_abs_val_in_decoder = (1 << (quan_bit_in_decoder - 1)) - 1; // 2^(4-1) - 1
    range_size_in_decoder = (1 << quan_bit_in_decoder) - 1;        // 2^4 - 1
    vector<string> v;
    splitString(param, v, ";");
    _round = stoi(v[0]);
    init_norm_quan_index(alpha);
    init_column_to_column_index(encoder);
}

void QLMSA::init_column_to_column_index(EncoderBase &encoder)
{
    int column = encoder.getBlockLength();
    vector<vector<int>> _row_mat = encoder.getRowMat();
    vector<vector<int>> _column_mat = encoder.getColMat();
    column_to_column_index.resize(column);
    for (int i = 0; i < column; i++)
    {
        column_to_column_index[i].resize(_column_mat[i].size());
        for (int j = 0; j < _column_mat[i].size(); j++)
        {
            int rowIdx = _column_mat[i][j];
            column_to_column_index[i][j].resize(_row_mat[rowIdx].size());
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
                column_to_column_index[i][j][k] = g;
            }
        }
    }
}

void QLMSA::init_norm_quan_index(double alpha)
{
    _norm_quan_index.resize(range_size_in_decoder);
    for (int i = -max_abs_val_in_decoder; i <= max_abs_val_in_decoder; i++)
    {
        int tmp = i;
        if (tmp > 0)
            tmp = std::ceil(alpha * tmp);
        else if (tmp < 0)
            tmp = std::ceil(alpha * tmp) - 1;
        else
            tmp = 0;
        _norm_quan_index[i + max_abs_val_in_decoder] = tmp;
    }
}

// belief propagation algorithm (quantized layered min sum)
int QLMSA::decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta)
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
        if (receive[i] > beta)
            llr_list[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta)
            llr_list[i] = 1;
        else if (receive[i] > -beta && receive[i] <= 0)
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
    for (; iter < _round;)
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
                        int g = column_to_column_index[i][j][k];
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
                tmp = _norm_quan_index[tmp + max_abs_val_in_decoder];
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
