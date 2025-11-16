#include "GC-EncoderBase.hh"

GC_EncoderBase::GC_EncoderBase()
{
}

GC_EncoderBase::GC_EncoderBase(string param)
{
}

GC_EncoderBase::~GC_EncoderBase()
{
}

// gaussianJordanElimination of HMatrix, get G_mat and save in fileMatrixG
int GC_EncoderBase::guassianJordanH1(const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1)
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
int GC_EncoderBase::guassianJordanH2(const string &fileMatrixG2, const string &fileMatrixC2, const string &fileMatrixR2)
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

void GC_EncoderBase::GetSemiMatrix_2local(const string &param){
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
    string fileMatrixV = "./conf/gc/GC_" + param + "_V";
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
    string fileMatrixW = "./conf/gc/GC_" + param + "_W";
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
    string fileMatrixX = "./conf/gc/GC_" + param + "_X";
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
    string fileMatrixY = "./conf/gc/GC_" + param + "_Y";
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
void GC_EncoderBase::GetSemiMatrix_3local(const string &param){
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
    string fileMatrixV = "./conf/gc/GC_" + param + "_V";
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
    string fileMatrixW = "./conf/gc/GC_" + param + "_W";
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
    string fileMatrixZ = "./conf/gc/GC_" + param + "_Z";
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
    string fileMatrixX = "./conf/gc/GC_" + param + "_X";
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
    string fileMatrixY = "./conf/gc/GC_" + param + "_Y";
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
void GC_EncoderBase::initializeGCLDPC_3local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixG2, const string &fileMatrixC2, 
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
    
    // 3、intialize _H_mat_1, _H_mat_2, _Ht_mat and _H_global_mat
    _H_mat_1.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    _H_mat_2.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    _Ht_mat.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
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
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for( int j = _block_length_total - _block_length_local; j < _block_length_total; j++){
            _Ht_mat[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
    }
    
    // 4、check whether already exists V to Z matrix and intilaize them using _H_GC_matrix
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

    // 5、initialize _G_mat_1 using fileMatrixG1, regenerate if not exist, each parity bit calculated by mutable_i info bits
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

    // 6、initialize _G_mat_2 using fileMatrixG2, regenerate if not exist, each parity bit calculated by mutable_i info bits
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
    f1.close();
    f2.close();
    f10.close();

    // 7、Update HMatrix_1 by invalid check equation
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
    for (int i : _invalid_column_index)
        for (int j = 0; j < _parity_length_local; j++)
            _H_mat_1[j][i] = (unsigned char)0;

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
    for (int i : _invalid_row_index)
        for (int j = 0; j < _block_length_local; j++)
            _H_mat_1[i][j] = (unsigned char)0;
    f3.close();
    f4.close();

    // 8、Update HMatrix_2 by invalid check equation
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
    for (int i : _invalid_column_index)
        for (int j = 0; j < _parity_length_local; j++)
            _H_mat_2[j][i] = (unsigned char)0;

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
    for (int i : _invalid_row_index)
        for (int j = 0; j < _block_length_local; j++)
            _H_mat_2[i][j] = (unsigned char)0;
    f11.close();
    f12.close();

    // 9、update _H_GC_mat using _H_mat_1 and _H_mat_2
    for (int i = 0; i < _parity_length_local; i++){
        for (int j = 0; j < _block_length_local; j++){
            _H_GC_mat[i][j] = _H_mat_1[i][j];
        }
    }
    for (int i = _parity_length_local; i < 2 * _parity_length_local; i++){
        for (int j = _block_length_local; j < 2 * _block_length_local; j++){
            _H_GC_mat[i][j] = _H_mat_2[i - _parity_length_local][j - _block_length_local];
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
void GC_EncoderBase::initializeGCLDPC_2local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixV, 
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
    
    // 3、intialize _H_mat_1, _H_mat_2, _Ht_mat and _H_global_mat
    _H_mat_1.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    _Ht_mat.resize(_parity_length_local, vector<unsigned char>(_block_length_local, 0));
    for(int i = 0; i < _parity_length_local; i++){
        for(int j = 0; j < _block_length_local; j++){
            _H_mat_1[i][j] = _H_GC_mat[i][j];
        }
    }
    for( int i = _parity_length_total - _parity_length_local - _parity_length_global; i < _parity_length_total - _parity_length_global; i++){
        for( int j = _block_length_total - _block_length_local; j < _block_length_total; j++){
            _Ht_mat[i - (_parity_length_total - _parity_length_local - _parity_length_global)][j - (_block_length_total - _block_length_local)] = _H_GC_mat[i][j];
        }
    }
    
    // 4、check whether already exists V to Z matrix and intilaize them using _H_GC_matrix
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

    // 5、initialize _G_mat_1 using fileMatrixG1, regenerate if not exist, each parity bit calculated by info bits
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

    // 6、Update HMatrix_1 by invalid check equation
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
    for (int i : _invalid_column_index)
        for (int j = 0; j < _parity_length_local; j++)
            _H_mat_1[j][i] = (unsigned char)0;

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
    for (int i : _invalid_row_index)
        for (int j = 0; j < _block_length_local; j++)
            _H_mat_1[i][j] = (unsigned char)0;
    f3.close();
    f4.close();

    // 7、update H_GC_mat using updated H_mat_1
    for (int i = 0; i < _parity_length_local; i++){
        for (int j = 0; j < _block_length_local; j++){
            _H_GC_mat[i][j] = _H_mat_1[i][j];
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