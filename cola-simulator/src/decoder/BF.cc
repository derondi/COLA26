#include "BF.hh"

BF::BF(string param) {
    // parse param for BF algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round = stoi(v[0]);
}

// basic bit flipping
int BF::decode(double* receive, unsigned char* corrected, vector<vector<int>>& _row_mat, vector<vector<int>>& _column_mat,double beta, double alph) {
    int row = _row_mat.size();
    int column = _column_mat.size();
    // result of syn check
    vector<int> syn_check(row);
    // energy list of bit
    vector<int> synlist(column);
    // bitlist
    vector<int> bitlist(column);
    // initialize bitlist
    for (int i = 0; i < column; i++){
        bitlist[i] = (receive[i] > 0) ? 0 : 1;
    }
    int iter = 0;
    for (; iter < _round;){
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
            synlist[j] = 0;
            for( int i = 0; i < _column_mat[j].size(); i++){
                int rowIdx = _column_mat[j][i];
                synlist[j] = synlist[j] + syn_check[rowIdx];
            }
            // update flip threshold
            if( FlipThd < synlist[j]){
                FlipThd = synlist[j];
                }
        }
        // do the bit flipping
        for (int i = 0; i < column; i++){
            if(FlipThd == synlist[i])
                bitlist[i] = 1^bitlist[i];
            corrected[i] = bitlist[i];
        }
        // check
        if (check(corrected, _row_mat))
            return iter;
    }
    return -1;

}