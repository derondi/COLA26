#include "GC-IRS.hh"

GC_IRS::GC_IRS(string param)
{
    // parse param for GC
    vector<string> v;
    splitString(param, v, ";");
    _t = stoi(v[0]);
    _m_local = stoi(v[1]);
    _m_global = stoi(v[2]);
    _m = stoi(v[3]);
    _n = stoi(v[4]);
    _N = stoi(v[5]);
    _n_local = _n / _t;

    // calculate bit length
    getBlockLengthLocal() = _n_local * _N;
    getInfoLengthLocal() = (_n_local - _m_local) * _N;
    getParityLengthLocal() = _m_local * _N;
    getInfoLengthLocalt() = (_n_local - _m_local - _m_global) * _N;
    getParityLengthLocalt() = (_m_local + _m_global) * _N;
    getInfoLengthGlobal() = (_t * _n_local - _m_global) * _N;
    getParityLengthGlobal() = _m_global * _N;
    getBlockLengthTotal() = _n * _N;
    getParityLengthTotal() = _m * _N;
    getLocalMatrixNumber() = _t;

    string fileMatrixP = "./conf/gc/GC_" + param + "_P";
    string fileMatrixG1 = "./conf/gc/GC_" + param + "_G1";
    string fileMatrixC1 = "./conf/gc/GC_" + param + "_C1";
    string fileMatrixR1 = "./conf/gc/GC_" + param + "_R1";
    string fileMatrixG2 = "./conf/gc/GC_" + param + "_G2";
    string fileMatrixC2 = "./conf/gc/GC_" + param + "_C2";
    string fileMatrixR2 = "./conf/gc/GC_" + param + "_R2";
    string fileMatrixV = "./conf/gc/GC_" + param + "_V";
    string fileMatrixW = "./conf/gc/GC_" + param + "_W";
    string fileMatrixX = "./conf/gc/GC_" + param + "_X";
    string fileMatrixY = "./conf/gc/GC_" + param + "_Y";
    string fileMatrixZ = "./conf/gc/GC_" + param + "_Z";

    // when get a special P matrix, we can initialize qc-ldpc by EncoderBase function, judge that we have 2 or 3 local matrix
    if (_t == 2){
        initializeGCLDPC_2local(_N, fileMatrixP, fileMatrixG1, fileMatrixC1, fileMatrixR1, fileMatrixV, fileMatrixW, fileMatrixX, fileMatrixY, fileMatrixZ, param);
    }
    if (_t == 3){
        initializeGCLDPC_3local(_N, fileMatrixP, fileMatrixG1, fileMatrixC1, fileMatrixR1, fileMatrixG2, fileMatrixC2, fileMatrixR2, fileMatrixV, fileMatrixW, fileMatrixX, fileMatrixY, fileMatrixZ, param);
    }
}

void GC_IRS::encode1(unsigned char *info, unsigned char *parity)
{
    vector<vector<int>> &_G_mat = getGMat1();
    // Encode by guassianJordan _G_mat_1
    for (int i = 0; i < _G_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _G_mat[i].size(); j++)
        {
            _xor = _xor + info[_G_mat[i][j]];
        }
        parity[i] = (unsigned char)(_xor % 2);
    }
}

void GC_IRS::encode2(unsigned char *info, unsigned char *parity)
{
    vector<vector<int>> &_G_mat = getGMat2();
    // Encode by guassianJordan _G_mat_2
    for (int i = 0; i < _G_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _G_mat[i].size(); j++)
        {
            _xor = _xor + info[_G_mat[i][j]];
        }
        parity[i] = (unsigned char)(_xor % 2);
    }
}

void GC_IRS::encode_2local(unsigned char *c1, unsigned char *s2, unsigned char* p2, unsigned char* p0)
{
    vector<vector<int>> &_V_mat = getVMat();
    vector<vector<int>> &_W_mat = getWMat();
    vector<vector<int>> &_X_mat = getXMat();
    vector<vector<int>> &_Y_mat = getYMat();
    std::vector<unsigned char> p01(_m_global * _N, 0);
    std::vector<unsigned char> p02(_m_global * _N, 0);
    std::vector<unsigned char> p21(_m_local * _N, 0);
    std::vector<unsigned char> p22(_m_local * _N, 0);
    // encode by semi-matrix _V_mat, _W_mat, _X_mat and _Y_mat
    // get p01
    for (int i = 0; i < _V_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _V_mat[i].size(); j++)
        {
            _xor = _xor + s2[_V_mat[i][j]];
        }
        p01[i] = (unsigned char)(_xor % 2);
    }
    // get p02
    for (int i = 0; i < _W_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _W_mat[i].size(); j++)
        {
            _xor = _xor + c1[_W_mat[i][j]];
        }
        p02[i] = (unsigned char)(_xor % 2);
    }
    // get p0
    for (int i = 0; i < _V_mat.size(); i++)
    {
        p0[i] = p01[i] ^ p02[i];
    }
    // get p21
    for (int i = 0; i < _X_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _X_mat[i].size(); j++)
        {
            _xor = _xor + s2[_X_mat[i][j]];
        }
        p21[i] = (unsigned char)(_xor % 2);
    }
    // get p22
    for (int i = 0; i < _Y_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _Y_mat[i].size(); j++)
        {
            _xor = _xor + p0[_Y_mat[i][j]];
        }
        p22[i] = (unsigned char)(_xor % 2);
    }
    // get p2
    for (int i = 0; i < _X_mat.size(); i++)
    {
        p2[i] = p21[i] ^ p22[i]; 
    }
}

void GC_IRS::encode_3local(unsigned char *c1, unsigned char *c2, unsigned char* s3, unsigned char* p3, unsigned char* p0)
{
    vector<vector<int>> &_V_mat = getVMat();
    vector<vector<int>> &_W_mat = getWMat();
    vector<vector<int>> &_X_mat = getXMat();
    vector<vector<int>> &_Y_mat = getYMat();
    vector<vector<int>> &_Z_mat = getZMat();
    std::vector<unsigned char> p01(_m_global * _N, 0);
    std::vector<unsigned char> p02(_m_global * _N, 0);
    std::vector<unsigned char> p03(_m_global * _N, 0);
    std::vector<unsigned char> p31(_m_local * _N, 0);
    std::vector<unsigned char> p32(_m_local * _N, 0);
    // encode by semi-matrix _V_mat, _W_mat, _X_mat and _Y_mat
    // get p01
    for (int i = 0; i < _V_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _V_mat[i].size(); j++)
        {
            _xor = _xor + s3[_V_mat[i][j]];
        }
        p01[i] = (unsigned char)(_xor % 2);
    }
    // get p02
    for (int i = 0; i < _W_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _W_mat[i].size(); j++)
        {
            _xor = _xor + c1[_W_mat[i][j]];
        }
        p02[i] = (unsigned char)(_xor % 2);
    }
    // get p03
    for (int i = 0; i < _Z_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _Z_mat[i].size(); j++)
        {
            _xor = _xor + c2[_Z_mat[i][j]];
        }
        p03[i] = (unsigned char)(_xor % 2);
    }
    // get p0
    for (int i = 0; i < _V_mat.size(); i++)
    {
        bool a = p01[i] ^ p02[i];
        p0[i] = a ^ p03[i];
        
    }
    // get p21
    for (int i = 0; i < _X_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _X_mat[i].size(); j++)
        {
            _xor = _xor + s3[_X_mat[i][j]];
        }
        p31[i] = (unsigned char)(_xor % 2);
    }
    // get p22
    for (int i = 0; i < _Y_mat.size(); i++)
    {
        int _xor = 0;
        for (int j = 0; j < _Y_mat[i].size(); j++)
        {
            _xor = _xor + p0[_Y_mat[i][j]];
        }
        p32[i] = (unsigned char)(_xor % 2);
    }
    // get p2
    for (int i = 0; i < _X_mat.size(); i++)
    {
        p3[i] = p31[i] ^ p32[i]; 
    }
}