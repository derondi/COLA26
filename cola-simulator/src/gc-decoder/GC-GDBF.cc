#include "GC-GDBF.hh"

GC_GDBF::GC_GDBF(string param) {
    // parse param for GDBF algorithm
    vector<string> v;
    splitString(param, v, ";");
    _round_local = stoi(v[0]);
    _round_global = stoi(v[1]);
}

// Gradient Descent Bit Flipping
int GC_GDBF::decode_local_1(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
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
    }
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

int GC_GDBF::decode_local_2(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
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
    }
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

int GC_GDBF::decode_local_t(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
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
    }
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

int GC_GDBF::decode_global(double *receive, unsigned char *corrected, vector<vector<int>> &_row_mat, vector<vector<int>> &_column_mat, double alpha, double beta){
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
    }
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