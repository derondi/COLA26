#include "SPA.hh"

SPA::SPA(string param)
{
    // parse param for SPA algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round = stoi(v[0]);
}

// belief propagation algorithm (sum product)
int SPA::decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta)
{
    int row = _row_mat.size();
    int column = _column_mat.size();
    vector<vector<double>> edge_mat(row);
    vector<vector<double>> last_edge_mat(row);
    vector<double> updated_llr(column);
    vector<double> llrlist(column);

    // Init LLR
    for (int i = 0; i < column; i++)
        llrlist[i] = receive[i] * 16;

    memcpy(&updated_llr[0], &llrlist[0], column * sizeof(double));

    for (int i = 0; i < row; i++)
    {
        edge_mat[i].resize(_row_mat[i].size(), 0);
        last_edge_mat[i].resize(_row_mat[i].size(), 0);
    }
    int iter = 0;
    for (; iter < _round;)
    {
        iter++;
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < _row_mat[i].size(); j++)
            {
                double tmp = 1;
                for (int k = 0; k < _row_mat[i].size(); k++)
                {
                    if (j == k)
                        continue;
                    int colIdx = _row_mat[i][k];
                    double l1 = updated_llr[colIdx] - last_edge_mat[i][k];
                    l1 = min(l1, 20.0);
                    l1 = max(l1, -20.0);
                    tmp = tmp * tanh(l1 / 2);
                }
                edge_mat[i][j] = 2 * atanh(tmp);
            }
        }

        last_edge_mat = edge_mat;
        memcpy(&updated_llr[0], &llrlist[0], column * sizeof(double));

        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < _row_mat[i].size(); j++)
            {
                int colIdx = _row_mat[i][j];
                updated_llr[colIdx] = updated_llr[colIdx] + last_edge_mat[i][j];
            }
        }
        for (int i = 0; i < column; i++)
        {
            if (updated_llr[i] > 0)
                corrected[i] = 0;
            else
                corrected[i] = 1;
        }
        // check whether decode correctly
        if (check(corrected, _row_mat))
            return iter;
    } // Iteration loop end
    return -1;
}
