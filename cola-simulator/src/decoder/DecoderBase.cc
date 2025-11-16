#include "DecoderBase.hh"

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
