#include "EncoderBase.hh"

EncoderBase::EncoderBase()
{
}

EncoderBase::EncoderBase(string param)
{
}

EncoderBase::~EncoderBase()
{
}

// the QCLDPC initialize for lower triangular
void EncoderBase::RUinitializeQCLDPC(int N, const string &fileMatrixP, const string &fileMatrixA, const string &fileMatrixB, const string &fileMatrixT, const string &fileMatrixCD, const string &param, const int &col, const int &row, const int &g, const int &gap)
{
    // 1、initialize baseH using fileMatrixP, first line is metadata [m n N], remain is a m * n matrix
    ifstream f1(fileMatrixP.c_str());
    if (!f1.is_open())
    {
        cerr << "open MatrixP failed! fileName = " << fileMatrixP << endl;
        exit(-1);
    }
    vector<vector<int>> baseH(_parity_length / N);
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

    // 2、intialize _H_mat by lifting baseH by factor N, composed of rotate right shift I
    _H_mat.resize(_parity_length, vector<unsigned char>(_block_length, 0));
    for (int i = 0; i < baseH.size(); i++)
    {
        for (int j = 0; j < baseH[0].size(); j++)
        {
            if (baseH[i][j] >= 0)
            {
                for (int lift = 0; lift < N; lift++)
                {
                    _H_mat[N * i + lift][N * j + (lift + baseH[i][j]) % N] = (unsigned char)1;
                }
            }
        }
    }
    // 3、intialize _row_mat and _column_mat, decoder will use them to reconstruct HMatrix
    _column_mat.resize(_block_length);
    _row_mat.resize(_parity_length);
    for (int i = 0; i < _parity_length; i++)
    {
        for (int j = 0; j < _block_length; j++)
        {
            if (_H_mat[i][j] == 1)
            {
                _row_mat[i].push_back(j);
                _column_mat[j].push_back(i);
            }
        }
    }

    struct stat buffer;
    if (stat(fileMatrixA.c_str(), &buffer) != 0)
    {
        LowerTriangular(g, param);
    }
    // 4.1、initialize _A_mat using fileMatrixA
    ifstream f2(fileMatrixA.c_str());
    if (!f2.is_open())
    {
        cerr << "open MatrixA failed! fileName = " << fileMatrixA << endl;
        exit(-1);
    }
    i = 0;
    _A_mat.resize(g);
    while (getline(f2, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _A_mat[i].push_back(stoi(num));
        i++;
    }
    // 4.2、initialize _B_mat using fileMatrixB
    ifstream f3(fileMatrixB.c_str());
    if (!f3.is_open())
    {
        cerr << "open MatrixB failed! fileName = " << fileMatrixB << endl;
        exit(-1);
    }
    i = 0;
    _B_mat.resize(g);
    while (getline(f3, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _B_mat[i].push_back(stoi(num));
        i++;
    }
    // 4.3、initialize _T_mat using fileMatrixT
    ifstream f4(fileMatrixT.c_str());
    if (!f4.is_open())
    {
        cerr << "open MatrixT failed! fileName = " << fileMatrixT << endl;
        exit(-1);
    }
    i = 0;
    _T_mat.resize(g);
    while (getline(f4, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _T_mat[i].push_back(stoi(num));
        i++;
    }
    // 4.4、initialize _CD_mat using fileMatrixCD
    ifstream f5(fileMatrixCD.c_str());
    if (!f5.is_open())
    {
        cerr << "open MatrixCD failed! fileName = " << fileMatrixCD << endl;
        exit(-1);
    }
    i = 0;
    _CD_mat.resize(col - row);
    while (getline(f5, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _CD_mat[i].push_back(stoi(num));
        i++;
    }
    f1.close();
    f2.close();
    f3.close();
    f4.close();
    f5.close();
}

// read existed qc-ldpc file and initialize HMatrix and GMatrix
void EncoderBase::initializeQCLDPC(int N, const string &fileMatrixP, const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR)
{
    // 1、initialize baseH using fileMatrixP, first line is metadata [m n N], remain is a m * n matrix
    ifstream f1(fileMatrixP.c_str());
    if (!f1.is_open())
    {
        cerr << "open MatrixP failed! fileName = " << fileMatrixP << endl;
        exit(-1);
    }
    vector<vector<int>> baseH(_parity_length / N);
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

    // 2、intialize _H_mat by lifting baseH by factor N, composed of rotate right shift I
    _H_mat.resize(_parity_length, vector<unsigned char>(_block_length, 0));
    for (int i = 0; i < baseH.size(); i++)
    {
        for (int j = 0; j < baseH[0].size(); j++)
        {
            if (baseH[i][j] >= 0)
            {
                for (int lift = 0; lift < N; lift++)
                {
                    _H_mat[N * i + lift][N * j + (lift + baseH[i][j]) % N] = (unsigned char)1;
                }
            }
        }
    }

    // 3、initialize _G_mat using fileMatrixG, regenerate if not exist, each parity bit calculated by mutable_i info bits
    struct stat buffer;
    if (stat(fileMatrixG.c_str(), &buffer) != 0)
    {
        if (guassianJordanH(fileMatrixG, fileMatrixC, fileMatrixR) != 0)
        {
            std::cerr << "QC-LDPC HMatrix [A|B] GuassianJordan to [A'|I_B] Failed! PMatrix = " << fileMatrixP << endl;
            exit(-1);
        }
    }
    ifstream f2(fileMatrixG.c_str());
    if (!f2.is_open())
    {
        cerr << "open MatrixG failed! fileName = " << fileMatrixG << endl;
        exit(-1);
    }
    i = 0;
    _G_mat.resize(_parity_length);
    while (getline(f2, s))
    {
        istringstream istrm(s);
        string num;
        while (istrm >> num)
            _G_mat[i].push_back(stoi(num));
        i++;
    }
    f1.close();
    f2.close();

    // 4、Update HMatrix by invalid check equation
    ifstream f3(fileMatrixC.c_str());
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
        for (int j = 0; j < _parity_length; j++)
            _H_mat[j][i] = (unsigned char)0;

    ifstream f4(fileMatrixR.c_str());
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
        for (int j = 0; j < _block_length; j++)
            _H_mat[i][j] = (unsigned char)0;
    f3.close();
    f4.close();

    // 5、intialize _row_mat and _column_mat, decoder will use them to reconstruct HMatrix
    _column_mat.resize(_block_length);
    _row_mat.resize(_parity_length);
    for (int i = 0; i < _parity_length; i++)
    {
        for (int j = 0; j < _block_length; j++)
        {
            if (_H_mat[i][j] == 1)
            {
                _row_mat[i].push_back(j);
                _column_mat[j].push_back(i);
            }
        }
    }
}

// gaussianJordanElimination of HMatrix, get G_mat and save in fileMatrixG
int EncoderBase::guassianJordanH(const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR)
{
    int row = _H_mat.size();
    int col = _H_mat[0].size();
    vector<vector<unsigned char>> GuassHMatrix(_H_mat);
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
            ofstream out1(fileMatrixC.c_str(), ios::app);
            if (!out1.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixC << endl;
                exit(-1);
            }
            out1 << (i + col - row) << " ";
            for (int j = 0; j < col; j++)
                GuassHMatrix[i][j] = (unsigned char)0;
            int j = column_index[i];
            ofstream out2(fileMatrixR.c_str(), ios::app);
            if (!out2.is_open())
            {
                cerr << "Failed to create file! target fileName = " << fileMatrixR << endl;
                exit(-1);
            }
            out2 << j << " ";
            for (int k = 0; k < row; k++)
                _H_mat[k][i + col - row] = (unsigned char)0;
            for (int k = 0; k < col; k++)
                _H_mat[j][k] = (unsigned char)0;
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
        GuassHMatrix = _H_mat;
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
    ofstream out(fileMatrixG.c_str());
    if (!out.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixG << endl;
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

void EncoderBase::LowerTriangular(const int &g, const string &param)
{
    int row = _H_mat.size();
    int col = _H_mat[0].size();
    int gap = row - g;
    // initialize matrix to zero
    std::vector<std::vector<unsigned char>> A_matrix(g, std::vector<unsigned char>(col - row, 0));
    std::vector<std::vector<unsigned char>> B_matrix(g, std::vector<unsigned char>(gap, 0));
    std::vector<std::vector<unsigned char>> C_matrix(gap, std::vector<unsigned char>(col - row, 0));
    std::vector<std::vector<unsigned char>> D_matrix(gap, std::vector<unsigned char>(gap, 0));
    std::vector<std::vector<unsigned char>> T_matrix(g, std::vector<unsigned char>(g, 0));
    std::vector<std::vector<unsigned char>> E_matrix(gap, std::vector<unsigned char>(g, 0));
    vector<vector<unsigned char>> HtMatrix(_H_mat);
    // lower triangular process
    for (int i = 0; i < g; ++i)
    {
        // Find the first row with a non-zero element in column i
        int pivot_row = -1;
        for (int j = i; j < row; ++j)
        {
            if (HtMatrix[j][i + col - g] == 1)
            {
                pivot_row = j;
                break;
            }
        }

        // If no pivot row is found, the matrix is not full rank
        if (pivot_row == -1)
        {
            std::cerr << "Matrix is not full rank.\n";
            return;
        }

        // Swap the current row with the pivot row
        std::swap(HtMatrix[i], HtMatrix[pivot_row]);
    }
    // initialize semimatrix using HtMmatrix
    break_to_semimatrix(HtMatrix, A_matrix, B_matrix, C_matrix, D_matrix, T_matrix, E_matrix, g);
    // update the D matrix and C matrix
    C_matrix = get_new_matrix(C_matrix, A_matrix, E_matrix, T_matrix);
    D_matrix = get_new_matrix(D_matrix, B_matrix, E_matrix, T_matrix);
    // calculation of matrix
    std::vector<std::vector<unsigned char>> transpose_A_matrix = transpose(A_matrix);
    std::vector<std::vector<unsigned char>> transpose_B_matrix = transpose(B_matrix);
    std::vector<std::vector<unsigned char>> transpose_C_matrix = transpose(C_matrix);
    std::vector<std::vector<unsigned char>> transpose_D_matrix = transpose(D_matrix);
    std::vector<std::vector<unsigned char>> transpose_T_matrix = transpose(T_matrix);
    std::vector<std::vector<unsigned char>> inverse_transpose_D_matrix = inverseMatrix(transpose_D_matrix);
    std::vector<std::vector<unsigned char>> inverse_transpose_T_matrix = inverseMatrix(transpose_T_matrix);
    std::vector<std::vector<unsigned char>> C_D = matrixMultiplication(transpose_C_matrix, inverse_transpose_D_matrix);
    // save matrix in local
    // CD matrix
    string fileMatrixCD = "./conf/RU_" + param + "_CD";
    ofstream out1(fileMatrixCD.c_str());
    if (!out1.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixCD << endl;
        exit(-1);
    }
    for (int j = 0; j < gap; j++)
    {
        for (int i = 0; i < col - row; i++)
        {
            if (C_D[i][j])
            {
                out1 << i << " ";
            }
        }
        out1 << endl;
    }
    out1.close();
    // A matrix
    string fileMatrixA = "./conf/RU_" + param + "_A";
    ofstream out2(fileMatrixA.c_str());
    if (!out2.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixA << endl;
        exit(-1);
    }
    for (int j = 0; j < g; j++)
    {
        for (int i = 0; i < col - row; i++)
        {
            if (transpose_A_matrix[i][j])
            {
                out2 << i << " ";
            }
        }
        out2 << endl;
    }
    out2.close();
    // B matrix
    string fileMatrixB = "./conf/RU_" + param + "_B";
    ofstream out3(fileMatrixB.c_str());
    if (!out3.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixB << endl;
        exit(-1);
    }
    for (int j = 0; j < g; j++)
    {
        for (int i = 0; i < gap; i++)
        {
            if (transpose_B_matrix[i][j])
            {
                out3 << i << " ";
            }
        }
        out3 << endl;
    }
    out3.close();
    // T matrix
    string fileMatrixT = "./conf/RU_" + param + "_T";
    ofstream out4(fileMatrixT.c_str());
    if (!out4.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixT << endl;
        exit(-1);
    }
    for (int j = 0; j < g; j++)
    {
        for (int i = 0; i < g; i++)
        {
            if (inverse_transpose_T_matrix[i][j])
            {
                out4 << i << " ";
            }
        }
        out4 << endl;
    }
    out4.close();
}
