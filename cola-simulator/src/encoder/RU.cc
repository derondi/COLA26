#include "RU.hh"

RU::RU(string param) {
    // parse param for RU
    vector<string> v;
    splitString(param, v, ";");
    _m = stoi(v[0]);
    _n = stoi(v[1]);
    _N = stoi(v[2]);
    _k = stoi(v[3]);
    _p = stoi(v[4]);
    getBlockLength() = _n * _N;
    getInfoLength() = (_n - _m) * _N;
    getParityLength() = _m * _N;
    _col = _n * _N;
    _row = _m * _N;
    _g = _k * _N - _p;
    _gap = _row - _g;

    string fileMatrixP = "./conf/normal/RU_" + param + "_P";
    string fileMatrixA = "./conf/normal/RU_" + param + "_A";
    string fileMatrixB = "./conf/normal/RU_" + param + "_B";
    string fileMatrixT = "./conf/normal/RU_" + param + "_T";
    string fileMatrixCD = "./conf/normal/RU_" + param + "_CD";
    // when get a special P matrix, we can initialize qc-ldpc by EncoderBase function
    RUinitializeQCLDPC(_N, fileMatrixP, fileMatrixA, fileMatrixB, fileMatrixT, fileMatrixCD, param, _col, _row , _g, _gap);
}

void RU::encode(unsigned char* info, unsigned char* parity) {
    vector<vector<int>>& _A_mat = getAMat();
    vector<vector<int>>& _B_mat = getBMat();
    vector<vector<int>>& _T_mat = getTMat();
    vector<vector<int>>& _CD_mat = getCDMat();
    std::vector<int> P_1(_gap,0);
    std::vector<int> P_2(_g,0);
    std::vector<int> uA(_g,0);
    std::vector<int> pB(_g,0);
    std::vector<int> uA_pB(_g,0);
    //P_1
    
      for(int i = 0; i < _gap; i++){
        for(int j = 0; j < _CD_mat[i].size(); j++){
            P_1[i] ^= info[_CD_mat[i][j]];
        }
    }
    //uA and pB
    for(int i = 0; i < _g; i++){
        for(int j = 0; j < _A_mat[i].size(); j++){
            uA[i] ^= info[_A_mat[i][j]];
        }
        for(int k = 0; k < _B_mat[i].size(); k++){
            pB[i] ^= P_1[_B_mat[i][k]];
        }
        uA_pB[i] = uA[i]^pB[i];
    }
    //P_2
      for(int i = 0; i < _g; i++){
        for(int j = 0; j < _T_mat[i].size(); j++){
            P_2[i] ^= uA_pB[_T_mat[i][j]];
        }
    }
    //parity
    for (int i = 0; i < _row; i++) {
        if(0 <= i < _gap)
        parity[i] = P_1[i];
        if(i >= _gap)
        parity[i] = P_2[i - _gap];
    }
}