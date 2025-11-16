#include "PPBF.hh"
#include "../util/utils.hh"

PPBF::PPBF(string param) {
    // parse param for PPBF algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round = stoi(v[0]);
}

// basic bit flipping
int PPBF::decode(double* receive, unsigned char* corrected, vector<vector<int>>& _row_mat, vector<vector<int>>& _column_mat,double beta, double alph) {
    //PPBF
    int row = _row_mat.size(); 
    int column = _column_mat.size();
    vector<int> syn_check(row);
    vector<int> energylist(column);
    vector<int> bitlist(column);
    vector<int> y(column);
    vector<bool> R(column);
    for (int i = 0; i < column; i++){
        y[i] = (receive[i] > 0) ? 0 : 1;
        bitlist[i] = y[i];
    }
    // flipping probability for every energy
    // slightly better than GDBF
    // double p[7]={0, 0.001, 0.001, 0.01, 0.7, 0.9, 1};
    double p[7]={0, 0.0001, 0.001, 0.01, 0.95, 0.97, 1};
    //start the iteration
    int iter = 0;
    for (; iter < _round;){
        iter++;
        //compute the syn result
        for (int i = 0; i < row; i++){
            syn_check[i] = 0;
            for(int j = 0;j < _row_mat[i].size();j++){
                int colIdx = _row_mat[i][j];
                syn_check[i] = syn_check[i]^bitlist[colIdx];
            }
        }
        //compute the syn result energy
        for (int j = 0; j < column; j++){
            energylist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                energylist[j] = energylist[j] + syn_check[rowIdx];
            }
            int b = bitlist[j]^y[j];
            energylist[j] = b + energylist[j];
            R[j] = generateR(p[energylist[j]]);
            bitlist[j] = R[j]^bitlist[j];
            corrected[j] = bitlist[j];
        }
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;

}

bool PPBF::generateR(double p){
    bernoulli_distribution d(p);
    return d(e);
}