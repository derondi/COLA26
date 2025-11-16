#include "Encoder.h"
#include "Config.h"
#include "ECC.h"
#include "Sim_Defs.h"
#include "Utils.h"

#include <fstream>
#include <numeric>
#include <sstream>

EncoderBase::EncoderBase()
{
}

EncoderBase::EncoderBase(string param)
{
}

EncoderBase::~EncoderBase()
{
}

// gaussianJordanElimination of HMatrix, get G_mat and save in fileMatrixG
int EncoderBase::guassianJordanH1(const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1)
{
    int row = _H_mat_1.size();
    int col = _H_mat_1[0].size();
    vector<vector<unsigned char>> GuassHMatrix(_H_mat_1);
    bool full_rank = true;
    vector<int> column_index(row);
    for (int i = 0; i < row; i++)
        column_index[i] = i;

    // gaussianJordanElimination process, HMatrix [A|B] would be [A'|I_B] format
    for (int i = 0; i < row; i++)
    {
        // Find the pivot row (first 1 in the current [i+col-row]_th column)
        int pivotRow = -1;
        for (int j = i; j < row; j++)
        {
            if (GuassHMatrix[j][i + col - row])
            {
                pivotRow = j;
                break;
            }
        }
        // will be reduced row echelon instead of identity, need column transformation
        if (pivotRow == -1)
        {
            full_rank = false;
            ofstream out1(fileMatrixC1.c_str(), ios::app);
            if (!out1.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixC1 << endl;
                exit(-1);
            }
            out1 << (i + col - row) << " ";
            for (int j = 0; j < col; j++)
                GuassHMatrix[i][j] = (unsigned char)0;
            int j = column_index[i];
            ofstream out2(fileMatrixR1.c_str(), ios::app);
            if (!out2.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixR1 << endl;
                exit(-1);
            }
            out2 << j << " ";
            for (int k = 0; k < row; k++)
                _H_mat_1[k][i + col - row] = (unsigned char)0;
            for (int k = 0; k < col; k++)
                _H_mat_1[j][k] = (unsigned char)0;
            out1.close();
            out2.close();
            continue;
        }

        swap(GuassHMatrix[i], GuassHMatrix[pivotRow]);
        swap(column_index[i], column_index[pivotRow]);

        for (int k = 0; k < row; k++)
        {
            if (k != i && GuassHMatrix[k][i + col - row])
            {
                for (int j = 0; j < col; j++)
                {
                    GuassHMatrix[k][j] ^= GuassHMatrix[i][j];
                }
            }
        }
    }
    int rank = row;
    if (!full_rank)
    {
        GuassHMatrix = _H_mat_1;
        for (int i = 0; i < row; i++)
        {
            // Find the pivot row (first 1 in the current [i+col-row]_th column)
            int pivotRow = -1;
            for (int j = i; j < row; j++)
            {
                if (GuassHMatrix[j][i + col - row])
                {
                    pivotRow = j;
                    break;
                }
            }
            // will be reduced row echelon instead of identity, need column transformation
            if (pivotRow == -1)
            {
                rank--;
                continue;
            }

            swap(GuassHMatrix[i], GuassHMatrix[pivotRow]);

            for (int k = 0; k < row; k++)
            {
                if (k != i && GuassHMatrix[k][i + col - row])
                {
                    for (int j = 0; j < col; j++)
                    {
                        GuassHMatrix[k][j] ^= GuassHMatrix[i][j];
                    }
                }
            }
        }
    }
    // save GuassHMatrix in fileMatrixG
    ofstream out(fileMatrixG1.c_str());
    if (!out.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixG1 << endl;
        exit(-1);
    }
    for (int i = 0; i != row; i++)
    {
        for (int j = 0; j != col - row; j++)
        {
            if (GuassHMatrix[i][j])
            {
                out << j << " ";
            }
        }
        out << endl;
    }
    out.close();
    return 0;
}

// gaussianJordanElimination of HMatrix, get G_mat and save in fileMatrixG
int EncoderBase::guassianJordanH2(const string &fileMatrixG2, const string &fileMatrixC2, const string &fileMatrixR2)
{
    int row = _H_mat_2.size();
    int col = _H_mat_2[0].size();
    vector<vector<unsigned char>> GuassHMatrix(_H_mat_2);
    bool full_rank = true;
    vector<int> column_index(row);
    for (int i = 0; i < row; i++)
        column_index[i] = i;

    // gaussianJordanElimination process, HMatrix [A|B] would be [A'|I_B] format
    for (int i = 0; i < row; i++)
    {
        // Find the pivot row (first 1 in the current [i+col-row]_th column)
        int pivotRow = -1;
        for (int j = i; j < row; j++)
        {
            if (GuassHMatrix[j][i + col - row])
            {
                pivotRow = j;
                break;
            }
        }
        // will be reduced row echelon instead of identity, need column transformation
        if (pivotRow == -1)
        {
            full_rank = false;
            ofstream out1(fileMatrixC2.c_str(), ios::app);
            if (!out1.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixC2 << endl;
                exit(-1);
            }
            out1 << (i + col - row) << " ";
            for (int j = 0; j < col; j++)
                GuassHMatrix[i][j] = (unsigned char)0;
            int j = column_index[i];
            ofstream out2(fileMatrixR2.c_str(), ios::app);
            if (!out2.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixR2 << endl;
                exit(-1);
            }
            out2 << j << " ";
            for (int k = 0; k < row; k++)
                _H_mat_2[k][i + col - row] = (unsigned char)0;
            for (int k = 0; k < col; k++)
                _H_mat_2[j][k] = (unsigned char)0;
            out1.close();
            out2.close();
            continue;
        }

        swap(GuassHMatrix[i], GuassHMatrix[pivotRow]);
        swap(column_index[i], column_index[pivotRow]);

        for (int k = 0; k < row; k++)
        {
            if (k != i && GuassHMatrix[k][i + col - row])
            {
                for (int j = 0; j < col; j++)
                {
                    GuassHMatrix[k][j] ^= GuassHMatrix[i][j];
                }
            }
        }
    }
    int rank = row;
    if (!full_rank)
    {
        GuassHMatrix = _H_mat_2;
        for (int i = 0; i < row; i++)
        {
            // Find the pivot row (first 1 in the current [i+col-row]_th column)
            int pivotRow = -1;
            for (int j = i; j < row; j++)
            {
                if (GuassHMatrix[j][i + col - row])
                {
                    pivotRow = j;
                    break;
                }
            }
            // will be reduced row echelon instead of identity, need column transformation
            if (pivotRow == -1)
            {
                rank--;
                continue;
            }

            swap(GuassHMatrix[i], GuassHMatrix[pivotRow]);

            for (int k = 0; k < row; k++)
            {
                if (k != i && GuassHMatrix[k][i + col - row])
                {
                    for (int j = 0; j < col; j++)
                    {
                        GuassHMatrix[k][j] ^= GuassHMatrix[i][j];
                    }
                }
            }
        }
    }
    // save GuassHMatrix in fileMatrixG
    ofstream out(fileMatrixG2.c_str());
    if (!out.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixG2 << endl;
        exit(-1);
    }
    for (int i = 0; i != row; i++)
    {
        for (int j = 0; j != col - row; j++)
        {
            if (GuassHMatrix[i][j])
            {
                out << j << " ";
            }
        }
        out << endl;
    }
    out.close();
    return 0;
}

void EncoderBase::GetSemiMatrix_2local(const string &param){
    // matrix for encode, need to be stored in files, initialize matrix to zero
    std::vector<std::vector<unsigned char>> V(_parity_length_global, std::vector<unsigned char>(_info_length_local_t, 0));
    std::vector<std::vector<unsigned char>> W(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));
    std::vector<std::vector<unsigned char>> X(_parity_length_local, std::vector<unsigned char>(_info_length_local_t, 0));
    std::vector<std::vector<unsigned char>> Y(_parity_length_local, std::vector<unsigned char>(_parity_length_global, 0));

    std::vector<std::vector<unsigned char>> Hd(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    std::vector<std::vector<unsigned char>> Hs(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));

    _Hg1.resize(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));
    _Hc.resize(_parity_length_local, std::vector<unsigned char>(_info_length_local_t, 0));
    _Hd.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    _He.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_global, 0));
    _Hx.resize(_parity_length_global, std::vector<unsigned char>(_info_length_local_t, 0));
    _Hy.resize(_parity_length_global, std::vector<unsigned char>(_parity_length_local, 0));
    _Hz.resize(_parity_length_global, std::vector<unsigned char>(_parity_length_global, 0));
    _Hs.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    // matrix for encode, using _H_GC_mat to get Hg, Hs, Hc, Hd, He, Hx, Hy, Hz, and output it into file
    // for Hg Matrix
    for( int i = _parity_length_total - _parity_length_global; i < _parity_length_total; i++){
        for( int j = 0; j < _block_length_local; j++){
            _Hg1[i - (_parity_length_total - _parity_length_global)][j] = _H_GC_mat[i][j];
        }
    }
    // for Hc,Hd,He Matrix
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for(int j = _block_length_total - _block_length_local; j < _block_length_total - _parity_length_local - _parity_length_global; j++){
            _Hc[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_local - _parity_length_global; j < _block_length_total - _parity_length_global; j++){
            Hd[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _parity_length_local - _parity_length_global)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_global; j < _block_length_total; j++){
            _He[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _parity_length_global)] = _H_GC_mat[i][j];
        }
    }
    // for Hx,Hy,Hz Matrix
    for( int i = _parity_length_total - _parity_length_global; i < _parity_length_total; i++){
        for(int j = _block_length_total - _block_length_local; j < _block_length_total - _parity_length_local - _parity_length_global; j++){
            _Hx[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_local - _parity_length_global; j < _block_length_total - _parity_length_global; j++){
            _Hy[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _parity_length_local - _parity_length_global)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_global; j < _block_length_total; j++){
            _Hz[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _parity_length_global)] = _H_GC_mat[i][j];
        }
    }

    // matrix computation
    cout << "begin to get inverse matrix of Hd" << endl;
    _Hd = inverseMatrix(Hd);
    cout << "get inverse of Hd finished" << endl;
    Hs = matrixAddition(_Hz, matrixMultiplication(_Hy, matrixMultiplication(_Hd, _He)));
    cout << "begin to gt inverse matrix of Hs" << endl;
    _Hs = inverseMatrix(Hs);
    cout << "get inverse of Hs finished" << endl;

    // get fileMatrixV and output it 
    V = matrixAddition(matrixMultiplication(_Hs, matrixMultiplication(_Hy, matrixMultiplication(_Hd, _Hc))),matrixMultiplication(_Hs, _Hx));
    string fileMatrixV = PREFIX_PATH + param + "_V";
    ofstream out1(fileMatrixV.c_str());
    if (!out1.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixV << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_global; j++)
    {
        for (int i = 0; i < _info_length_local_t; i++)
        {
            if (V[j][i])
            {
                out1 << i << " ";
            }
        }
        out1 << endl;
    }
    out1.close();

    // get fileMatrixW and output it
    W = matrixMultiplication(_Hs, _Hg1);
    string fileMatrixW = PREFIX_PATH + param + "_W";
    ofstream out2(fileMatrixW.c_str());
    if (!out2.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixW << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_global; j++)
    {
        for (int i = 0; i < _block_length_local; i++)
        {
            if (W[j][i])
            {
                out2 << i << " ";
            }
        }
        out2 << endl;
    }
    out2.close();
    
    // get fileMatrixX and output it
    X = matrixMultiplication(_Hd, _Hc);
    string fileMatrixX = PREFIX_PATH + param + "_X";
    ofstream out3(fileMatrixX.c_str());
    if (!out3.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixX << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_local; j++)
    {
        for (int i = 0; i < _info_length_local_t; i++)
        {
            if (X[j][i])
            {
                out3 << i << " ";
            }
        }
        out3 << endl;
    }

    // get fileMatrixY and output it
    Y = matrixMultiplication(_Hd, _He);
    string fileMatrixY = PREFIX_PATH + param + "_Y";
    ofstream out4(fileMatrixY.c_str());
    if (!out4.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixY << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_local; j++)
    {
        for (int i = 0; i < _parity_length_global; i++)
        {
            if (Y[j][i])
            {
                out4 << i << " ";
            }
        }
        out4 << endl;
    }
    out4.close();
}

// case that _t = 3
void EncoderBase::GetSemiMatrix_3local(const string &param){
    // matrix for encode, need to be stored in files, initialize matrix to zero
    std::vector<std::vector<unsigned char>> V(_parity_length_global, std::vector<unsigned char>(_info_length_local_t, 0));
    std::vector<std::vector<unsigned char>> W(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));
    std::vector<std::vector<unsigned char>> X(_parity_length_local, std::vector<unsigned char>(_info_length_local_t, 0));
    std::vector<std::vector<unsigned char>> Y(_parity_length_local, std::vector<unsigned char>(_parity_length_global, 0));
    std::vector<std::vector<unsigned char>> Z(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));

    _Hg1.resize(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));
    _Hg2.resize(_parity_length_global, std::vector<unsigned char>(_block_length_local, 0));
    _Hc.resize(_parity_length_local, std::vector<unsigned char>(_info_length_local_t, 0));
    _Hd.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    std::vector<std::vector<unsigned char>> Hd(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    _He.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_global, 0));
    _Hx.resize(_parity_length_global, std::vector<unsigned char>(_info_length_local_t, 0));
    _Hy.resize(_parity_length_global, std::vector<unsigned char>(_parity_length_local, 0));
    _Hz.resize(_parity_length_global, std::vector<unsigned char>(_parity_length_global, 0));
    std::vector<std::vector<unsigned char>> Hs(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    _Hs.resize(_parity_length_local, std::vector<unsigned char>(_parity_length_local, 0));
    // break H matrix to these semi-matrix
    // matrix for encode, using _H_GC_mat to get Hg, Hs, Hc, Hd, He, Hx, Hy, Hz, and output it into file
    // for Hg1 Matrix and Hg2 Matrix
    for( int i = _parity_length_total - _parity_length_global; i < _parity_length_total; i++){
        // for Hg1 Matrix
        for( int j = 0; j < _block_length_local; j++){
            _Hg1[i - (_parity_length_total - _parity_length_global)][j] = _H_GC_mat[i][j];
        }
        // for Hg2 Matrix
        for( int j = _block_length_local; j < _block_length_total - _block_length_local; j++){
            _Hg2[i - (_parity_length_total - _parity_length_global)][j - _block_length_local] = _H_GC_mat[i][j];
        }
    }
    // for Hc,Hd,He Matrix
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for(int j = _block_length_total - _block_length_local; j < _block_length_total - _parity_length_local - _parity_length_global; j++){
            _Hc[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_local - _parity_length_global; j < _block_length_total - _parity_length_global; j++){
            Hd[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _parity_length_local - _parity_length_global)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_global; j < _block_length_total; j++){
            _He[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _parity_length_global)] = _H_GC_mat[i][j];
        }
    }
    // for Hx,Hy,Hz Matrix
    for( int i = _parity_length_total - _parity_length_global; i < _parity_length_total; i++){
        for(int j = _block_length_total - _block_length_local; j < _block_length_total - _parity_length_local - _parity_length_global; j++){
            _Hx[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_local - _parity_length_global; j < _block_length_total - _parity_length_global; j++){
            _Hy[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _parity_length_local - _parity_length_global)] = _H_GC_mat[i][j];
        }
        for(int j = _block_length_total - _parity_length_global; j < _block_length_total; j++){
            _Hz[i - (_parity_length_total - _parity_length_global)][j - (_block_length_total - _parity_length_global)] = _H_GC_mat[i][j];
        }
    }

    // get fileMatrix V
    cout << "begin to get the inverse matrix of Hd" << endl;
    _Hd = inverseMatrix(Hd);
    cout << "get the inverse matrix of Hd" << endl;
    Hs = matrixAddition(_Hz, matrixMultiplication(_Hy, matrixMultiplication(_Hd, _He)));
    cout << "begin to  get the inverse matrix of Hs" << endl;
    _Hs = inverseMatrix(Hs);
    cout << "get the inverse matrix of Hs" << endl;

    // get fileMtrix V and output it
    V = matrixAddition(matrixMultiplication(_Hs, matrixMultiplication(_Hy, matrixMultiplication(_Hd, _Hc))),matrixMultiplication(_Hs, _Hx));
    string fileMatrixV = PREFIX_PATH + param + "_V";
    ofstream out1(fileMatrixV.c_str());
    if (!out1.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixV << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_global; j++)
    {
        for (int i = 0; i < _info_length_local_t; i++)
        {
            if (V[j][i])
            {
                out1 << i << " ";
            }
        }
        out1 << endl;
    }
    out1.close();

    // get fileMatrix W
    W = matrixMultiplication(_Hs, _Hg1);
    string fileMatrixW = PREFIX_PATH + param + "_W";
    ofstream out2(fileMatrixW.c_str());
    if (!out2.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixW << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_global; j++)
    {
        for (int i = 0; i < _block_length_local; i++)
        {
            if (W[j][i])
            {
                out2 << i << " ";
            }
        }
        out2 << endl;
    }
    out2.close();
    
    // get fileMatrix Z
    Z = matrixMultiplication(_Hs, _Hg2);
    string fileMatrixZ = PREFIX_PATH + param + "_Z";
    ofstream out3(fileMatrixZ.c_str());
    if (!out3.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixZ << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_global; j++)
    {
        for (int i = 0; i < _block_length_local; i++)
        {
            if (Z[j][i])
            {
                out3 << i << " ";
            }
        }
        out3 << endl;
    }
    out3.close();

    // get fileMatrix X
    X = matrixMultiplication(_Hd, _Hc);
    string fileMatrixX = PREFIX_PATH + param + "_X";
    ofstream out4(fileMatrixX.c_str());
    if (!out4.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixX << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_local; j++)
    {
        for (int i = 0; i < _info_length_local_t; i++)
        {
            if (X[j][i])
            {
                out4 << i << " ";
            }
        }
        out4 << endl;
    }
    out4.close();

    // get fileMatrix Y
    Y = matrixMultiplication(_Hd, _He);
    string fileMatrixY = PREFIX_PATH + param + "_Y";
    ofstream out5(fileMatrixY.c_str());
    if (!out5.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixY << endl;
        exit(-1);
    }
    for (int j = 0; j < _parity_length_local; j++)
    {
        for (int i = 0; i < _parity_length_global; i++)
        {
            if (Y[j][i])
            {
                out5 << i << " ";
            }
        }
        out5 << endl;
    }
    out5.close();
}

// read existed qc-ldpc file and initialize HMatrix and GMatrix
void EncoderBase::initializeGCLDPC_3local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixG2, const string &fileMatrixC2, 
                                    const string &fileMatrixR2, const string &fileMatrixV, const string &fileMatrixW, const string &fileMatrixX, const string &fileMatrixY, const string &fileMatrixZ, const string &param)
{
    // 1、initialize baseH using fileMatrixP, first line is metadata [m n N], remain is a m * n matrix
    ifstream f1(fileMatrixP.c_str());
    if (!f1.is_open())
    {
        cerr << "open MatrixP failed! fileName = " << fileMatrixP << endl;
        exit(-1);
    }
    vector<vector<int>> baseH(_parity_length_total / N);
    string s = "";
    getline(f1, s);
    int i = 0;
    while (getline(f1, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            baseH[i].push_back(stoi(num));
        i++;
    }
    f1.close();

    // 2、intialize _H_GC_mat by lifting baseH by factor N, composed of rotate right shift I
    _H_GC_mat.resize(_parity_length_total, vector<unsigned char>(_block_length_total, 0));
    for (int i = 0; i < baseH.size(); i++)
    {
        for (int j = 0; j < baseH[0].size(); j++)
        {
            if (baseH[i][j] >= 0)
            {
                for (int lift = 0; lift < N; lift++)
                {
                    _H_GC_mat[N * i + lift][N * j + (lift + baseH[i][j]) % N] = (unsigned char)1;
                }
            }
        }
    }

    // 3、intialize _H_mat_1, _H_mat_2
    _H_mat_1.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    _H_mat_2.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    for(int i = 0; i < _parity_length_local; i++){
        for(int j = 0; j < _block_length_local; j++){
            _H_mat_1[i][j] = _H_GC_mat[i][j];
        }
    }
    for(int i = _parity_length_local; i < 2 * _parity_length_local; i++){
        for(int j = _block_length_local; j < 2 * _block_length_local; j++){
            _H_mat_2[i - _parity_length_local][j - _block_length_local] = _H_GC_mat[i][j];
        }
    }

    // 4 initialize _G_mat_1 using fileMatrixG1, regenerate if not exist, each parity bit calculated by mutable_i info bits
    struct stat buffer_2;
    if (stat(fileMatrixG1.c_str(), &buffer_2) != 0)
    {
        if (guassianJordanH1(fileMatrixG1, fileMatrixC1, fileMatrixR1) != 0)
        {
            std::cerr << "QC-LDPC HMatrix [A|B] GuassianJordan to [A'|I_B] Failed! PMatrix = " << fileMatrixP << endl;
            exit(-1);
        }
    }
    ifstream f2(fileMatrixG1.c_str());
    if (!f2.is_open())
    {
        cerr << "open MatrixG failed! fileName = " << fileMatrixG1 << endl;
        exit(-1);
    }
    i = 0;
    _G_mat_1.resize(_parity_length_local);
    while (getline(f2, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _G_mat_1[i].push_back(stoi(num));
        i++;
    }
    f2.close();

    // 5 initialize _G_mat_2 using fileMatrixG2, regenerate if not exist, each parity bit calculated by mutable_i info bits
    struct stat buffer_3;
    if (stat(fileMatrixG2.c_str(), &buffer_3) != 0)
    {
        if (guassianJordanH2(fileMatrixG2, fileMatrixC2, fileMatrixR2) != 0)
        {
            std::cerr << "QC-LDPC HMatrix [A|B] GuassianJordan to [A'|I_B] Failed! PMatrix = " << fileMatrixP << endl;
            exit(-1);
        }
    }
    ifstream f10(fileMatrixG2.c_str());
    if (!f10.is_open())
    {
        cerr << "open MatrixG failed! fileName = " << fileMatrixG2 << endl;
        exit(-1);
    }
    i = 0;
    _G_mat_2.resize(_parity_length_local);
    while (getline(f10, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _G_mat_2[i].push_back(stoi(num));
        i++;
    }
    f10.close();

    // 6、Update HMatrix_1 and _H_GC_mat by invalid check equation
    ifstream f3(fileMatrixC1.c_str());
    if (f3.is_open())
    {
        while (getline(f3, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_column_index.push_back(stoi(num));
        }
    }
    for (int i : _invalid_column_index){
        for (int j = 0; j < _parity_length_local; ++j)
            _H_mat_1[j][i] = (unsigned char)0;
        for (int j = 0; j < _parity_length_total; ++j)
            _H_GC_mat[j][i] = (unsigned char)0;
    }

    ifstream f4(fileMatrixR1.c_str());
    if (f4.is_open())
    {
        while (getline(f4, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_row_index.push_back(stoi(num));
        }
    } 
    for (int i : _invalid_row_index){
        for (int j = 0; j < _block_length_local; ++j)
            _H_mat_1[i][j] = (unsigned char)0;
        for (int j = 0; j < _block_length_total; ++j)
            _H_GC_mat[i][j] = (unsigned char)0;
    }
    f3.close();
    f4.close();

    // 7、Update HMatrix_2 and _H_GC_mat by invalid check equation
    ifstream f11(fileMatrixC2.c_str());
    if (f11.is_open())
    {
        while (getline(f11, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_column_index.push_back(stoi(num));
        }
    }
    for (int i : _invalid_column_index){
        for (int j = 0; j < _parity_length_local; j++)
            _H_mat_2[j][i] = (unsigned char)0;
        for (int j = 0; j < _parity_length_total; ++j)
            _H_GC_mat[j][i + _block_length_local] = (unsigned char)0;
    }

    ifstream f12(fileMatrixR2.c_str());
    if (f12.is_open())
    {
        while (getline(f12, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_row_index.push_back(stoi(num));
        }
    } 
    for (int i : _invalid_row_index){
        for (int j = 0; j < _block_length_local; j++)
            _H_mat_2[i][j] = (unsigned char)0;
        for (int j = 0; j < _block_length_total; ++j)
            _H_GC_mat[i + _parity_length_local][j] = (unsigned char)0;
    }
    f11.close();
    f12.close();
    
    // 8、_Ht_mat
    _Ht_mat.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for( int j = _block_length_total - _block_length_local; j < _block_length_total; j++){
            _Ht_mat[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
    }
    
    // 9、check whether already exists V to Z matrix and intilaize them using _H_GC_matrix
    vector<string> v;
    splitString(param, v, ";");
    int _t = stoi(v[0]);
    struct stat buffer_1;
    if (stat(fileMatrixV.c_str(), &buffer_1) != 0)
    {
        if(_t == 3)
            GetSemiMatrix_3local(param);
        else{
            std::cerr << "wrong local matrix number when geting semi matrix! Number = " << _t << endl;
            exit(-1);
        }
    }

    // 10、initialize V,W,X,Y,Z matrix
    // V matrix
    ifstream f5(fileMatrixV.c_str());
    if (!f5.is_open())
    {
        cerr << "open MatrixV failed! fileName = " << fileMatrixV << endl;
        exit(-1);
    }
    i = 0;
    _V_mat.resize(_parity_length_global);
    while (getline(f5, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _V_mat[i].push_back(stoi(num));
        i++;
    }
    f5.close();
    // W matrix
    ifstream f6(fileMatrixW.c_str());
    if (!f6.is_open())
    {
        cerr << "open MatrixW failed! fileName = " << fileMatrixW << endl;
        exit(-1);
    }
    i = 0;
    _W_mat.resize(_parity_length_global);
    while (getline(f6, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _W_mat[i].push_back(stoi(num));
        i++;
    }
    f6.close();
    // X matrix
    ifstream f7(fileMatrixX.c_str());
    if (!f7.is_open())
    {
        cerr << "open MatrixX failed! fileName = " << fileMatrixX << endl;
        exit(-1);
    }
    i = 0;
    _X_mat.resize(_parity_length_local);
    while (getline(f7, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _X_mat[i].push_back(stoi(num));
        i++;
    }
    f7.close();
    // Y matrix
    ifstream f8(fileMatrixY.c_str());
    if (!f8.is_open())
    {
        cerr << "open MatrixY failed! fileName = " << fileMatrixY << endl;
        exit(-1);
    }
    i = 0;
    _Y_mat.resize(_parity_length_local);
    while (getline(f8, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _Y_mat[i].push_back(stoi(num));
        i++;
    }
    f8.close();
    // Z matrix
    if(_t == 3){
        ifstream f9(fileMatrixZ.c_str());
        if (!f9.is_open())
        {
            cerr << "open MatrixZ failed! fileName = " << fileMatrixZ << endl;
            exit(-1);
        }
        i = 0;
        _Z_mat.resize(_parity_length_global);
        while (getline(f9, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _Z_mat[i].push_back(stoi(num));
            i++;
        }
        f9.close();
    }
    
    // 11、intialize _row_mat_1 and _column_mat_1, decoder will use them to reconstruct HMatrix
    _column_mat_1.resize(_block_length_local);
    _row_mat_1.resize(_parity_length_local);
    for (int i = 0; i < _parity_length_local; i++)
    {
        for (int j = 0; j < _block_length_local; j++)
        {
            if (_H_mat_1[i][j] == 1)
            {
                _row_mat_1[i].push_back(j);
                _column_mat_1[j].push_back(i);
            }
        }
    }

     // 12、intialize _row_mat_2 and _column_mat_2, decoder will use them to reconstruct HMatrix
    _column_mat_2.resize(_block_length_local);
    _row_mat_2.resize(_parity_length_local);
    for (int i = 0; i < _parity_length_local; i++)
    {
        for (int j = 0; j < _block_length_local; j++)
        {
            if (_H_mat_2[i][j] == 1)
            {
                _row_mat_2[i].push_back(j);
                _column_mat_2[j].push_back(i);
            }
        }
    }
    
    // 13、intialize _t_row_mat and _t_column_mat, decoder will use them to reconstruct HtMatrix
    _t_column_mat.resize(_block_length_local);
    _t_row_mat.resize(_parity_length_local);
    for (int i = 0; i < _parity_length_local; i++)
    {
        for (int j = 0; j < _block_length_local; j++)
        {
            if (_Ht_mat[i][j] == 1)
            {
                _t_row_mat[i].push_back(j);
                _t_column_mat[j].push_back(i);
            }
        }
    }
    
   // 14、intialize _global_row_mat and _global_column_mat, decoder will use them to reconstruct HglobalMatrix
    _global_column_mat.resize(_block_length_total);
    _global_row_mat.resize(_parity_length_total);
    for (int i = 0; i < _parity_length_total; i++)
    {
        for (int j = 0; j < _block_length_total; j++)
        {
            if (_H_GC_mat[i][j] == 1)
            {
                _global_row_mat[i].push_back(j);
                _global_column_mat[j].push_back(i);
            }
        }
    }
}

// read existed qc-ldpc file and initialize HMatrix and GMatrix
void EncoderBase::initializeGCLDPC_2local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixV, 
                                            const string &fileMatrixW, const string &fileMatrixX, const string &fileMatrixY, const string &fileMatrixZ, const string &param)
{
    // 1、initialize baseH using fileMatrixP, first line is metadata [m n N], remain is a m * n matrix
    ifstream f1(fileMatrixP.c_str());
    if (!f1.is_open())
    {
        cerr << "open MatrixP failed! fileName = " << fileMatrixP << endl;
        exit(-1);
    }
    vector<vector<int>> baseH(_parity_length_total / N);
    string s = "";
    getline(f1, s);
    int i = 0;
    while (getline(f1, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            baseH[i].push_back(stoi(num));
        i++;
    }
    f1.close();

    // 2、intialize _H_GC_mat by lifting baseH by factor N, composed of rotate right shift I
    _H_GC_mat.resize(_parity_length_total, vector<unsigned char>(_block_length_total, 0));
    for (int i = 0; i < baseH.size(); i++)
    {
        for (int j = 0; j < baseH[0].size(); j++)
        {
            if (baseH[i][j] >= 0)
            {
                for (int lift = 0; lift < N; lift++)
                {
                    _H_GC_mat[N * i + lift][N * j + (lift + baseH[i][j]) % N] = (unsigned char)1;
                }
            }
        }
    }

    // 3、intialize _H_mat_1
    _H_mat_1.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    for(int i = 0; i < _parity_length_local; i++){
        for(int j = 0; j < _block_length_local; j++){
            _H_mat_1[i][j] = _H_GC_mat[i][j];
        }
    }

    // 4、initialize _G_mat_1 using fileMatrixG1, regenerate if not exist, each parity bit calculated by info bits
    struct stat buffer_2;
    if (stat(fileMatrixG1.c_str(), &buffer_2) != 0)
    {
        if (guassianJordanH1(fileMatrixG1, fileMatrixC1, fileMatrixR1) != 0)
        {
            std::cerr << "QC-LDPC HMatrix [A|B] GuassianJordan to [A'|I_B] Failed! PMatrix = " << fileMatrixP << endl;
            exit(-1);
        }
    }
    ifstream f2(fileMatrixG1.c_str());
    if (!f2.is_open())
    {
        cerr << "open MatrixG failed! fileName = " << fileMatrixG1 << endl;
        exit(-1);
    }
    i = 0;
    _G_mat_1.resize(_parity_length_local);
    while (getline(f2, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _G_mat_1[i].push_back(stoi(num));
        i++;
    }
    f2.close();

    // 5 、Update _H_mat_1 and _H_GC_mat by invalid check equation
    ifstream f3(fileMatrixC1.c_str());
    if (f3.is_open())
    {
        while (getline(f3, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_column_index.push_back(stoi(num));
        }
    }
    for (int i : _invalid_column_index){
        for (int j = 0; j < _parity_length_local; j++)
            _H_mat_1[j][i] = (unsigned char)0;
        for (int j = 0; j < _parity_length_total; j++)
            _H_GC_mat[j][i] = (unsigned char)0;
    }
    f3.close();

    ifstream f4(fileMatrixR1.c_str());
    if (f4.is_open())
    {
        while (getline(f4, s))
        {
            istringstream istrm(s);
            string num;
            while (istrm >> num)
                _invalid_row_index.push_back(stoi(num));
        }
    } 
    for (int i : _invalid_row_index){
        for (int j = 0; j < _block_length_local; j++)
            _H_mat_1[i][j] = (unsigned char)0;
        for (int j = 0; j < _block_length_total; j++)
            _H_GC_mat[i][j] = (unsigned char)0;
    }
    f4.close();
    
    // 6、intialize _Ht_mat
    _Ht_mat.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for( int j = _block_length_total - _block_length_local; j < _block_length_total; j++){
            _Ht_mat[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
    }
    
    // 7、check whether already exists V to Z matrix and intilaize them using _H_GC_matrix
    vector<string> v;
    splitString(param, v, ";");
    int _t = stoi(v[0]);
    struct stat buffer_1;
    if (stat(fileMatrixV.c_str(), &buffer_1) != 0)
    {
        if(_t == 2)
            GetSemiMatrix_2local(param);
        else{
            std::cerr << "wrong local matrix number when geting semi matrix! Number = " << _t << endl;
            exit(-1);
        }
    }

    // 8、initialize V,W,X,Y,Z matrix by using fileMatrix
    // initialize V matrix
    ifstream f5(fileMatrixV.c_str());
    if (!f5.is_open())
    {
        cerr << "open MatrixV failed! fileName = " << fileMatrixV << endl;
        exit(-1);
    }
    i = 0;
    _V_mat.resize(_parity_length_global);
    while (getline(f5, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _V_mat[i].push_back(stoi(num));
        i++;
    }
    f5.close();
    // initialize W matrix
    ifstream f6(fileMatrixW.c_str());
    if (!f6.is_open())
    {
        cerr << "open MatrixW failed! fileName = " << fileMatrixW << endl;
        exit(-1);
    }
    i = 0;
    _W_mat.resize(_parity_length_global);
    while (getline(f6, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _W_mat[i].push_back(stoi(num));
        i++;
    }
    f6.close();
    // initialize X matrix
    ifstream f7(fileMatrixX.c_str());
    if (!f7.is_open())
    {
        cerr << "open MatrixX failed! fileName = " << fileMatrixX << endl;
        exit(-1);
    }
    i = 0;
    _X_mat.resize(_parity_length_local);
    while (getline(f7, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _X_mat[i].push_back(stoi(num));
        i++;
    }
    f7.close();
    // initialize Y matrix
    ifstream f8(fileMatrixY.c_str());
    if (!f8.is_open())
    {
        cerr << "open MatrixY failed! fileName = " << fileMatrixY << endl;
        exit(-1);
    }
    i = 0;
    _Y_mat.resize(_parity_length_local);
    while (getline(f8, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _Y_mat[i].push_back(stoi(num));
        i++;
    }
    f8.close();
    
    // 9、intialize _row_mat and _column_mat, decoder will use them to reconstruct HMatrix
    _column_mat_1.resize(_block_length_local);
    _row_mat_1.resize(_parity_length_local);
    for (int i = 0; i < _parity_length_local; i++)
    {
        for (int j = 0; j < _block_length_local; j++)
        {
            if (_H_mat_1[i][j] == 1)
            {
                _row_mat_1[i].push_back(j);
                _column_mat_1[j].push_back(i);
            }
        }
    }
    
    // 10、intialize _t_row_mat and _t_column_mat, decoder will use them to reconstruct HtMatrix
    _t_column_mat.resize(_block_length_local);
    _t_row_mat.resize(_parity_length_local);
    for (int i = 0; i < _parity_length_local; i++)
    {
        for (int j = 0; j < _block_length_local; j++)
        {
            if (_Ht_mat[i][j] == 1)
            {
                _t_row_mat[i].push_back(j);
                _t_column_mat[j].push_back(i);
            }
        }
    }
    
   // 11、intialize _global_row_mat and _global_column_mat, decoder will use them to reconstruct HglobalMatrix
    _global_column_mat.resize(_block_length_total);
    _global_row_mat.resize(_parity_length_total);
    for (int i = 0; i < _parity_length_total; i++)
    {
        for (int j = 0; j < _block_length_total; j++)
        {
            if (_H_GC_mat[i][j] == 1)
            {
                _global_row_mat[i].push_back(j);
                _global_column_mat[j].push_back(i);
            }
        }
    }
}

IRS::IRS(string param)
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

    string fileMatrixP = PREFIX_PATH + param + "_P";
    string fileMatrixG1 = PREFIX_PATH + param + "_G1";
    string fileMatrixC1 = PREFIX_PATH + param + "_C1";
    string fileMatrixR1 = PREFIX_PATH + param + "_R1";
    string fileMatrixG2 = PREFIX_PATH + param + "_G2";
    string fileMatrixC2 = PREFIX_PATH + param + "_C2";
    string fileMatrixR2 = PREFIX_PATH + param + "_R2";
    string fileMatrixV = PREFIX_PATH + param + "_V";
    string fileMatrixW = PREFIX_PATH + param + "_W";
    string fileMatrixX = PREFIX_PATH + param + "_X";
    string fileMatrixY = PREFIX_PATH + param + "_Y";
    string fileMatrixZ = PREFIX_PATH + param + "_Z";

    // when get a special P matrix, we can initialize qc-ldpc by EncoderBase function, judge that we have 2 or 3 local matrix
    if (_t == 2){
        initializeGCLDPC_2local(_N, fileMatrixP, fileMatrixG1, fileMatrixC1, fileMatrixR1, fileMatrixV, fileMatrixW, fileMatrixX, fileMatrixY, fileMatrixZ, param);
    }
    if (_t == 3){
        initializeGCLDPC_3local(_N, fileMatrixP, fileMatrixG1, fileMatrixC1, fileMatrixR1, fileMatrixG2, fileMatrixC2, fileMatrixR2, fileMatrixV, fileMatrixW, fileMatrixX, fileMatrixY, fileMatrixZ, param);
    }
}

void IRS::encode1(unsigned char *info, unsigned char *parity)
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

void IRS::encode2(unsigned char *info, unsigned char *parity)
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

void IRS::encode_2local(unsigned char *c1, unsigned char *s2, unsigned char* p2, unsigned char* p0)
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

void IRS::encode_3local(unsigned char *c1, unsigned char *c2, unsigned char* s3, unsigned char* p3, unsigned char* p0)
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