#include "LMSA.hh"

LMSA::LMSA(string param)
{
    // parse param for MSA algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round = stoi(v[0]);
}

// belief propagation algorithm (min sum)
int LMSA::decode(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta)
{
    int row = _row_mat.size();
    int column = _column_mat.size();
    // decode using beta&alpha
    // double _beta = beta;
    // double _alpha = alpha;
    vector<vector<double>> edge_mat(row);
    vector<vector<double>> last_edge_mat(row);
    vector<double> updated_llr(column);
    vector<double> llrlist(column);

    // Init LLR
    for (int i = 0; i < column; i++)
    {
        // llrlist[i] = receive[i];
        if (receive[i] > beta)
            llrlist[i] = 4;
        else if (receive[i] > 0 && receive[i] <= beta)
            llrlist[i] = 1;
        else if (receive[i] > -beta && receive[i] <= 0)
            llrlist[i] = -1;
        else
            llrlist[i] = -4;
        if (llrlist[i] > 0)
            corrected[i] = 0;
        else
            corrected[i] = 1;
    }

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
                double tmp = 100;
                for (int k = 0; k < _row_mat[i].size(); k++)
                {
                    if (j == k)
                        continue;
                    int colIdx = _row_mat[i][k];
                    double l1 = updated_llr[colIdx] - last_edge_mat[i][k];
                    l1 = min(l1, 20.0);
                    l1 = max(l1, -20.0);
                    double sign_tmp = 1.0;
                    if (tmp < 0.0)
                        sign_tmp = -1.0;
                    double sign_l1 = 1.0;
                    if (l1 < 0.0)
                        sign_l1 = -1.0;
                    tmp = sign_tmp * sign_l1 * min(abs(l1), abs(tmp));
                }
                if (tmp > 0)
                    tmp = std::ceil(alpha * tmp);
                else if (tmp < 0)
                    tmp = std::ceil(alpha * tmp) - 1;
                else
                    tmp = 0;
                edge_mat[i][j] = tmp;
                int colIdx_2 = _row_mat[i][j];
                updated_llr[colIdx_2] = updated_llr[colIdx_2] - last_edge_mat[i][j] + tmp;
                updated_llr[colIdx_2] = min(updated_llr[colIdx_2], 7.0);
                updated_llr[colIdx_2] = max(updated_llr[colIdx_2], -7.0);
                last_edge_mat[i][j] = edge_mat[i][j];
            }
        }
        // hard decision
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