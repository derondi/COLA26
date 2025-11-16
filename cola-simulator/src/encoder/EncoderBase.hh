#ifndef _ENCODERBASE_HH_
#define _ENCODERBASE_HH_

#include "../inc/include.hh"
#include "../util/utils.hh"

using namespace std;

class EncoderBase
{
private:
    int _block_length;
    int _info_length;
    int _parity_length;

    vector<int> _invalid_row_index;
    vector<int> _invalid_column_index;

    vector<vector<int>> _row_mat;
    vector<vector<int>> _column_mat;
    vector<vector<int>> _G_mat;

    vector<vector<unsigned char>> _H_mat; // uesd as the whole matrix usaully and used as local matrix when using GC-LDPC, used for decode

    vector<vector<int>> _A_mat;
    vector<vector<int>> _B_mat; // semi-matrix when using RU encode
    vector<vector<int>> _T_mat; // semi-matrix when using RU encode
    vector<vector<int>> _CD_mat; // semi-matrix when using RU encode

public:
    EncoderBase();
    EncoderBase(string param);
    virtual ~EncoderBase();

    virtual void encode(unsigned char *info, unsigned char *parity) = 0;
    void initializeQCLDPC(int N, const string &fileMatrixP, const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR);
    int guassianJordanH(const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR);
  
    // for RU encode
    void LowerTriangular(const int &g, const string &param);
    void RUinitializeQCLDPC(int N, const string &fileMAtrixP, const string &fileMatrixA, const string &fileMatrixB, const string &fileMatrixT, const string &fileMatrixCD, const string &param, const int &col, const int &row, const int &g, const int &gap);                                           

    int &getBlockLength() { return _block_length; };
    int &getInfoLength() { return _info_length; };
    int &getParityLength() { return _parity_length; };

    vector<vector<int>> &getRowMat() { return _row_mat; }
    vector<vector<int>> &getColMat() { return _column_mat; }

    vector<vector<int>> &getGMat() { return _G_mat; }
    vector<vector<int>> &getAMat() { return _A_mat; }
    vector<vector<int>> &getBMat() { return _B_mat; }
    vector<vector<int>> &getTMat() { return _T_mat; }
    vector<vector<int>> &getCDMat() { return _CD_mat; }

    vector<int> &getInvalidRowIndex() { return _invalid_row_index; };
    vector<vector<unsigned char>> &getHMat() { return _H_mat; }

    // break H_matrix into semi-matrix for RU
    void break_to_semimatrix(std::vector<std::vector<unsigned char>> &H_matrix, std::vector<std::vector<unsigned char>> &A_matrix, std::vector<std::vector<unsigned char>> &B_matrix, std::vector<std::vector<unsigned char>> &C_matrix, std::vector<std::vector<unsigned char>> &D_matrix, std::vector<std::vector<unsigned char>> &T_matrix, std::vector<std::vector<unsigned char>> &E_matrix, int g)
    {
        long row = H_matrix.size();
        long col = H_matrix[0].size();
        for (int i = 0; i < g; i++)
        {
            for (int j = 0; j < col - row; j++)
            {
                A_matrix[i][j] = H_matrix[i][j];
            }
            for (long j = col - row; j < col - g; j++)
            {
                B_matrix[i][j - col + row] = H_matrix[i][j];
            }
            for (long j = col - g; j < col; j++)
            {
                T_matrix[i][j - col + g] = H_matrix[i][j];
            }
        }
        for (int i = g; i < row; i++)
        {
            for (int j = 0; j < col - row; j++)
            {
                C_matrix[i - g][j] = H_matrix[i][j];
            }
            for (long j = col - row; j < col - g; j++)
            {
                D_matrix[i - g][j - col + row] = H_matrix[i][j];
            }
            for (long j = col - g; j < col; j++)
            {
                E_matrix[i - g][j - col + g] = H_matrix[i][j];
            }
        }
    }

    // get_nex_matrix(), for RU encode, do the inverse and multiplication of semi-matrixs
    std::vector<std::vector<unsigned char>> get_new_matrix(std::vector<std::vector<unsigned char>> &aim_matrix, std::vector<std::vector<unsigned char>> &help_matrix,
                                                           std::vector<std::vector<unsigned char>> &E_matrix, std::vector<std::vector<unsigned char>> &T_matrix)
    {
        std::vector<std::vector<unsigned char>> new_matrix = aim_matrix;
        std::vector<std::vector<unsigned char>> inverse_T_matrix = inverseMatrix(T_matrix);
        std::vector<std::vector<unsigned char>> E_T = matrixMultiplication(E_matrix, inverse_T_matrix);
        std::vector<std::vector<unsigned char>> E_T_help = matrixMultiplication(E_T, help_matrix);
        new_matrix = matrixAddition(E_T_help, aim_matrix);
        return new_matrix;
    }

    // matrixAddition(), do the addition of two matrix
    std::vector<std::vector<unsigned char>> matrixAddition(const std::vector<std::vector<unsigned char>> &mat1, const std::vector<std::vector<unsigned char>> &mat2)
    {
        long n = mat1.size();
        long m = mat1[0].size();
        std::vector<std::vector<unsigned char>> result(n, std::vector<unsigned char>(m, 0));
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < m; ++j)
            {
                result[i][j] = mat1[i][j] ^ mat2[i][j];
            }
        }
        return result;
    }

    // matrixMultiplication(), do the multiplication of two matrix
    std::vector<std::vector<unsigned char>> matrixMultiplication(const std::vector<std::vector<unsigned char>> &mat1, const std::vector<std::vector<unsigned char>> &mat2)
    {
        long n = mat1.size();
        long m = mat2[0].size();
        long p = mat2.size();
        std::vector<std::vector<unsigned char>> result(n, std::vector<unsigned char>(m, 0));
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < m; ++j)
            {
                for (int k = 0; k < p; ++k)
                {
                    result[i][j] ^= (mat1[i][k] & mat2[k][j]);
                }
            }
        }
        return result;
    }

    // inverseMatrix(), get the inverse-matrix of the input matrix
    std::vector<std::vector<unsigned char>> inverseMatrix(const std::vector<std::vector<unsigned char>> &matrix)
    {
        long n = matrix.size();
        std::vector<std::vector<unsigned char>> augMatrix(n, std::vector<unsigned char>(2 * n, 0));
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                augMatrix[i][j] = matrix[i][j];
            }
            augMatrix[i][i + n] = 1;
        }
        for (int i = 0; i < n; ++i)
        {
            int pivotRow = -1;
            for (int j = i; j < n; ++j)
            {
                if (augMatrix[j][i])
                {
                    pivotRow = j;
                }
            }
            if (pivotRow == -1)
            {
                std::cout << "can't find the pivot row! gaussian-jordan to get inverse matrix fails!" << std::endl;
            }
            std::swap(augMatrix[i], augMatrix[pivotRow]);
            for (int k = 0; k < n; ++k)
            {
                if (k != i && augMatrix[k][i] == 1)
                {
                    for (int j = 0; j < 2 * n; ++j)
                    {
                        augMatrix[k][j] = augMatrix[k][j] ^ augMatrix[i][j];
                    }
                }
            }
        }
        std::vector<std::vector<unsigned char>> invMatrix(n, std::vector<unsigned char>(n, 0));
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                invMatrix[i][j] = augMatrix[i][j + n];
            }
        }
        return invMatrix;
    }

    // transpose(), to get the transpose format of a matrix
    std::vector<std::vector<unsigned char>> transpose(const std::vector<std::vector<unsigned char>> &matrix)
    {
        long rows = matrix.size();
        long cols = matrix[0].size();
        std::vector<std::vector<unsigned char>> transposed(cols, std::vector<unsigned char>(rows));
        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                transposed[j][i] = matrix[i][j];
            }
        }
        return transposed;
    }

    // row_multiplication_matrix(), do the multiplication of a vector with a matrix
    std::vector<int> row_multiplication_matrix(const std::vector<int> &vec, const std::vector<std::vector<int>> &matrix)
    {
        long n = matrix.size();
        long m = matrix[0].size();
        std::vector<int> result(n, 0);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < matrix[i].size(); j++)
            {
                result[i] ^= vec[matrix[i][j]];
            }
        }
        return result;
    }

    // row_addition(), do the addition of vector
    std::vector<int> row_addition(std::vector<int> code_1, std::vector<int> code_2)
    {
        long m = code_1.size();
        long n = code_2.size();
        if (m != n)
        {
            throw std::invalid_argument("Vector size must be equal.");
        }
        std::vector<int> new_code(m, 0);
        for (long i = 0; i < m; i++)
        {
            new_code[i] = code_1[i] ^ code_2[i];
        }
        return new_code;
    }
};

#endif
