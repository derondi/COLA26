#ifndef ENCODER_H
#define ENCODER_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <sys/stat.h>
#include <utility>
#include <vector>

using namespace std;

class EncoderBase
{
private:
    //int _block_length;
    //int _info_length;
    //int _parity_length;

    int _block_length_local;
    int _info_length_local;
    int _parity_length_local;
    int _info_length_local_t;
    int _parity_length_local_t;
    int _parity_length_global;
    int _info_length_global;
    int _block_length_total;
    int _parity_length_total;
    int _local_matrix_number;

    vector<int> _invalid_row_index;
    vector<int> _invalid_column_index;

    vector<vector<int>> _row_mat_1;
    vector<vector<int>> _column_mat_1;
    vector<vector<int>> _row_mat_2;
    vector<vector<int>> _column_mat_2;
    vector<vector<int>> _t_row_mat;
    vector<vector<int>> _t_column_mat;
    vector<vector<int>> _global_row_mat;
    vector<vector<int>> _global_column_mat;
    vector<vector<int>> _G_mat_1;
    vector<vector<int>> _G_mat_2;

    vector<vector<unsigned char>> _H_mat_1; // uesd as the whole matrix usaully and used as local matrix when using GC-LDPC, used for decode
    vector<vector<unsigned char>> _H_mat_2; // uesd as the whole matrix usaully and used as local matrix when using GC-LDPC, used for decode

    vector<vector<int>> _V_mat; // semi-matrix when using GC encode
    vector<vector<int>> _W_mat; // semi-matrix when using GC encode
    vector<vector<int>> _X_mat; // semi-matrix when using GC encode
    vector<vector<int>> _Y_mat; // semi-matrix when using GC encode
    vector<vector<int>> _Z_mat; // semi-matrix when using GC encode

    vector<vector<unsigned char>> _H_GC_mat; // the whole matrix when using GC-LDPC
    vector<vector<unsigned char>> _Ht_mat; // the last local matrix when using GC-LDPC, used for decode
    
    vector<vector<unsigned char>> _Hg1; // the first Hglobal matrix
    vector<vector<unsigned char>> _Hg2; // the second Hglobal matrix
    vector<vector<unsigned char>> _Hs; // the phi matrix, used for encode 
    vector<vector<unsigned char>> _Hc; // the Hc matrix
    vector<vector<unsigned char>> _Hd; // the inverse matrix of Hd, used for encode
    vector<vector<unsigned char>> _He; // the He matrix, used for encode
    vector<vector<unsigned char>> _Hx; // the Hx matrix, used for encode
    vector<vector<unsigned char>> _Hy; // the Hy matrix, used for encode
    vector<vector<unsigned char>> _Hz; // the Hz matrix, used for encode
    

public:
    EncoderBase();
    EncoderBase(string param);
    virtual ~EncoderBase();

    virtual void encode1(unsigned char *info, unsigned char *parity) = 0;
    virtual void encode2(unsigned char *info, unsigned char *parity) = 0;
    int guassianJordanH1(const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR);
    int guassianJordanH2(const string &fileMatrixG, const string &fileMatrixC, const string &fileMatrixR);
    
    // for GC-LDPC
    void initializeGCLDPC_2local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixV, const string &fileMatrixW, const string &fileMatrixX, const string &fileMatrixY, const string &fileMatrixZ, const string &param);
    void initializeGCLDPC_3local(int N, const string &fileMatrixP, const string &fileMatrixG1, const string &fileMatrixC1, const string &fileMatrixR1, const string &fileMatrixG2, const string &fileMatrixC2, const string &fileMatrixR2, const string &fileMatrixV, const string &fileMatrixW, const string &fileMatrixX, const string &fileMatrixY, const string &fileMatrixZ, const string &param);
    void GetSemiMatrix_2local(const string &param);
    void GetSemiMatrix_3local(const string &param);
    virtual void encode_2local(unsigned char *c1, unsigned char *s2, unsigned char* p2, unsigned char* p0) = 0;
    virtual void encode_3local(unsigned char *c1, unsigned char *c2, unsigned char* s3, unsigned char* p3, unsigned char* p0) = 0;                                                

    //int &getBlockLength() { return _block_length; };
    //int &getInfoLength() { return _info_length; };
    //int &getParityLength() { return _parity_length; };
    int &getBlockLengthLocal() { return _block_length_local; };
    int &getInfoLengthLocal() { return _info_length_local; };
    int &getParityLengthLocal() { return _parity_length_local; };
    int &getInfoLengthLocalt() { return _info_length_local_t; };
    int &getParityLengthLocalt() { return _parity_length_local_t; };
    int &getParityLengthGlobal() { return _parity_length_global; };
    int &getInfoLengthGlobal() { return _info_length_global; };
    int &getBlockLengthTotal() { return _block_length_total; };
    int &getParityLengthTotal() { return _parity_length_total; };
    int &getLocalMatrixNumber() { return _local_matrix_number; };

    vector<vector<int>> &getRowMat1() { return _row_mat_1; }
    vector<vector<int>> &getColMat1() { return _column_mat_1; }
    vector<vector<int>> &getRowMat2() { return _row_mat_2; }
    vector<vector<int>> &getColMat2() { return _column_mat_2; }
    vector<vector<int>> &getTRowMat() { return _t_row_mat; }
    vector<vector<int>> &getTColMat() { return _t_column_mat; }
    vector<vector<int>> &getGlobalRowMat() { return _global_row_mat; }
    vector<vector<int>> &getGlobalColMat() { return _global_column_mat; }

    vector<vector<int>> &getGMat1() { return _G_mat_1; }
    vector<vector<int>> &getGMat2() { return _G_mat_2; }
    vector<vector<int>> &getVMat() { return _V_mat; }
    vector<vector<int>> &getWMat() { return _W_mat; }
    vector<vector<int>> &getXMat() { return _X_mat; }
    vector<vector<int>> &getYMat() { return _Y_mat; }
    vector<vector<int>> &getZMat() { return _Z_mat; }

    vector<int> &getInvalidRowIndex() { return _invalid_row_index; };

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

// Ref: Integer Ring Sieve for Constructing Compact QC-LDPC Codes With Girths 8, 10, and 12
class IRS : public EncoderBase {
    private:
        int _m;     // row of exponent p matrix
        int _n;     // column of exponent p matrix
        int _N;     // lifting factor of exponent p matrix
        int _t; //the number of Hlocal matrix
        int _m_local; // row of local exponent p matrix
        int _m_global; // row of global exponent p matrix
        int _n_local; // column of local exponent p matrix

        int _a;
        set<int> _Tn;
        
    public:
        IRS(string param);
        void encode1(unsigned char* info, unsigned char* parity);
        void encode2(unsigned char* info, unsigned char* parity);
        void encode_2local(unsigned char *c1, unsigned char *s2, unsigned char* p2, unsigned char* p0);
        void encode_3local(unsigned char *c1, unsigned char *c2, unsigned char* s3, unsigned char* p3, unsigned char* p0);
};

#endif