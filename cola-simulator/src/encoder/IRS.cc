#include "IRS.hh"

IRS::IRS(string param)
{
    // parse param for IRS
    vector<string> v;
    splitString(param, v, ";");
    _m = stoi(v[0]);
    _n = stoi(v[1]);
    _N = stoi(v[2]);
    _girth = stoi(v[3]);
    getBlockLength() = _n * _N;
    getInfoLength() = (_n - _m) * _N;
    getParityLength() = _m * _N;

    struct stat buffer;
    string fileMatrixP = "./conf/normal/IRS_" + param + "_P";
    string fileMatrixG = "./conf/normal/IRS_" + param + "_G";
    string fileMatrixC = "./conf/normal/IRS_" + param + "_C";
    string fileMatrixR = "./conf/normal/IRS_" + param + "_R";
    // need generate new qc-ldpc matrix first time (exponent P matrix)
    if (stat(fileMatrixP.c_str(), &buffer) != 0)
    {
        vector<int> G(_n, _N);
        G[2] = 2;
        if (greedySearch(_m, _n, _N, _girth, G, fileMatrixP) != 0)
        {
            cout << "IRS Construction Failed! Param = " << param << endl;
            exit(-1);
        }
    }
    // when get a special P matrix, we can initialize qc-ldpc by EncoderBase function
    initializeQCLDPC(_N, fileMatrixP, fileMatrixG, fileMatrixC, fileMatrixR);
}

void IRS::encode(unsigned char *info, unsigned char *parity)
{
    vector<vector<int>> &_G_mat = getGMat();
    // Encode by guassianJordan _G_mat
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

// Deprecated: Too complex and time-consuming to get girth by extend matrix and loop calculation
int IRS::girthEvaluation(vector<vector<int>> &P, int N)
{
    int m = P.size();
    int n = P[0].size();
    vector<vector<int>> col_mat(n * N, vector<int>{});
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (P[i][j] >= 0)
            {
                for (int k = 0; k < N; k++)
                {
                    col_mat[j * N + (k + P[i][j]) % N].push_back(i * N + k);
                }
            }
        }
    }
    int min_girth = 1000;
    for (int i = 0; i < n; i++)
    {
        int colIdx = i * N;
        vector<int> &cn = col_mat[colIdx];
        for (int j = 0; j < cn.size(); j++)
        {
            vector<int> X(n * N, 0);
            vector<int> S(m * N, 0);
            S[cn[j]] = 1;
            int girth = 0;
            int cpt = 1;
            while (X[colIdx] == 0 && cpt < 20)
            {
                cpt++;
                for (int k = 0; k < n * N; k++)
                {
                    for (int l = 0; l < col_mat[k].size(); l++)
                    {
                        if (k == colIdx && l == j)
                            continue;
                        if (S[col_mat[k][l]])
                        {
                            X[k] = 1;
                            break;
                        }
                    }
                }
                S.assign(S.size(), 0);
                for (int k = 0; k < n * N; k++)
                {
                    for (int l = 0; l < col_mat[k].size(); l++)
                    {
                        if (k == colIdx && l == j)
                            continue;
                        if (X[k])
                        {
                            S[col_mat[k][l]] += 1;
                        }
                    }
                }
                girth = girth + 2;
            }
            if (girth < min_girth)
                min_girth = girth;
        }
    }
    return min_girth;
}

// In Use: avoid short cycles to get girth quickly (only support max girth 10 now)
int IRS::girthEvaluationV2(vector<vector<int>> &P, int N)
{
    int m = P.size();
    int n = P[0].size();

    // avoid 4-cycle
    for (int i = 1; i < m; i++)
    {
        for (int j1 = 1; j1 < n; j1++)
        {
            // inequation (9)
            if (P[i][j1] % N == 0)
                return 4;
            // inequation (11)
            if ((P[2][j1] - P[1][j1]) % N == 0)
                return 4;
            for (int j2 = 1; j2 < n; j2++)
            {
                if (j1 == j2)
                    continue;
                // inequation (10)
                if ((P[i][j2] - P[i][j1]) % N == 0)
                    return 4;
                // inequation (12)
                if ((P[1][j1] - P[1][j2] + P[2][j2] - P[2][j1]) % N == 0)
                    return 4;
            }
        }
    }

    // avoid 6-cycle
    for (int i1 = 1; i1 < m; i1++)
    {
        for (int i2 = 1; i2 < m; i2++)
        {
            if (i1 == i2)
                continue;
            for (int j1 = 1; j1 < n; j1++)
            {
                for (int j2 = 1; j2 < n; j2++)
                {
                    if (j1 == j2)
                        continue;
                    // inequation (14)
                    if ((P[i1][j2] - P[i2][j1]) % N == 0)
                        return 6;
                    // inequation (15)
                    if ((P[i1][j1] - P[i1][j2] + P[i2][j2]) % N == 0)
                        return 6;
                    for (int j3 = 1; j3 < n; j3++)
                    {
                        if (j1 == j3 || j2 == j3)
                            continue;
                        // inequation (16)
                        if ((P[i1][j2] - P[i1][j3] + P[i2][j3] - P[i2][j1]) % N == 0)
                            return 6;
                    }
                }
            }
        }
    }

    // avoid 8-cycle
    //  A QC-LDPC code has no 8-cycles if:
    //  (i) the matrix obtained by multiplying the difference matrix, DD, by 2 (or simply 2DD) has no zero element,
    //  (ii) the matrix DD has no repeated elements
    //  (iii) for each 4 × 4 submatrix of the exponent matrix B, the left side of Equation (2) is non-zero.
    vector<vector<int>> D(m * (m - 1) / 2, vector<int>(n));
    vector<vector<pair<int, int>>> DD(m * (m - 1) / 2, vector<pair<int, int>>(n * (n - 1) / 2));
    int row = 0;
    for (int i1 = 0; i1 < m - 1; i1++)
    {
        for (int i2 = i1 + 1; i2 < m; i2++)
        {
            for (int j = 0; j < n; j++)
            {
                D[row][j] = P[i1][j] - P[i2][j];
            }
            row++;
        }
    }
    int col = 0;
    set<int> st;
    for (int j1 = 0; j1 < n - 1; j1++)
    {
        for (int j2 = j1 + 1; j2 < n; j2++)
        {
            for (int i = 0; i < D.size(); i++)
            {
                int first = (D[i][j1] - D[i][j2]) % N;
                first = first >= 0 ? first : first + N;
                int second = N - first;
                DD[i][col] = {first, second};
                // condition (i)
                if (2 * first % N == 0 || 2 * second % N == 0)
                    return 8;
                // condition (ii)
                if (st.find(first) != st.end() || st.find(second) != st.end())
                {
                    return 8;
                }
                st.insert(first);
                st.insert(second);
            }
            col++;
        }
    }
    // condition (iii) is not required now when m = 3

    return 10;
}

// only for m = 3, not for m > 3
int IRS::greedySearch(int m, int n, int N, int g, vector<int> &G, const string &fileMatrixP)
{
    set<int> A;
    for (int i = 2; i <= N - 1; i++)
        A.insert(i);
    while (!A.empty() && _Tn.size() == 0)
    {
        _a = *A.begin();
        A.erase(_a);
        if (_a * (1 - _a) % N + N != 1)
            continue;
        cout << "IRS get posible a: " << _a << endl;
        for (int i = 2; i <= 5; i++)
            A.erase(pow(_a, i));
        vector<int> P1{0, 1, _a};
        set<int> T{0, 1};
        set<int> S;
        for (int i = 2; i <= N - 1; i++)
            S.insert(i);
        getRemainCoeff(T, S, P1, N, g);
        recursiveSearch(T, S, N, n, P1, G);
    }
    if (_Tn.size() == 0)
        return -1;
    cout << "IRS find solution: m = " << m << ", n = " << n << ", N = " << N << ", girth = " << g << endl
         << endl;
    // generate P matrix by _a and coefficient _Tn
    vector<int> P1{0, 1, _a};
    vector<vector<int>> P(m, vector<int>(n, 0));
    auto it = _Tn.begin();
    for (int col = 1; col < n; col++)
    {
        it++;
        for (int row = 0; row < m; row++)
        {
            if (col == 1)
                P[row][col] = P1[row];
            else
                P[row][col] = (*it) * P1[row] % N;
        }
    }
    // save P Matrix in fileMatrixP
    ofstream out(fileMatrixP.c_str());
    if (!out.is_open())
    {
        cerr << "Failed to create file! target fileName = " << fileMatrixP << endl;
        exit(-1);
    }
    out << n << " " << m << " " << N << endl;
    for (int row = 0; row < m; row++)
    {
        for (int col = 0; col < n; col++)
        {
            out << P[row][col] << " ";
        }
        out << endl;
    }
    out.close();
    return 0;
}

void IRS::getRemainCoeff(set<int> &coeff, set<int> &S, vector<int> &P1, int N, int g)
{
    int num = coeff.size();
    vector<vector<int>> P(_m, vector<int>(num + 1, 0));
    auto it = coeff.begin();
    for (int col = 1; col < num; col++)
    {
        it++;
        for (int row = 0; row < _m; row++)
        {
            if (col == 1)
                P[row][col] = P1[row];
            else
                P[row][col] = (*it) * P1[row] % N;
        }
    }
    // for girth-8 code，eliminate 12-cycles-wc and some trapping sets
    set<int> st1;
    for (int i = 0; i < num; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            int x = abs(P[1][j] - P[1][i]), y = N - x;
            st1.insert(x);
            st1.insert(y);
        }
    }
    // find remain coeff from S
    for (auto it = S.begin(); it != S.end();)
    {
        for (int row = 0; row < _m; row++)
        {
            P[row][num] = (*it) * P1[row] % N;
        }
        if (girthEvaluationV2(P, N) < g)
        {
            S.erase(it++);
            continue;
        }
        set<int> st2;
        bool repeat = false;
        for (int i = 0; i < num; i++)
        {
            int x = abs(P[1][num] - P[1][i]), y = N - x;
            if (st1.find(x) != st1.end() || st1.find(y) != st1.end() || st2.find(x) != st2.end() || st2.find(y) != st2.end())
                repeat = true;
            st2.insert(x);
            st2.insert(y);
        }
        if (repeat)
            S.erase(it++);
        else
            it++;
    }
}

void IRS::recursiveSearch(set<int> &coeff, set<int> &S, int N, int n, vector<int> &P1, vector<int> &G)
{
    int sz1 = coeff.size();
    int sz2 = S.size();
    _Tn = coeff;
    if (_Tn.size() == n)
        return;
    vector<int> vecS(S.begin(), S.end());
    // record: calculate newS size one bye one and then sort
    vector<int> s(S.size());
    int i = 0;
    for (auto it = S.begin(); it != S.end(); it++)
    {
        coeff.insert(*it);
        set<int> newS = S;
        newS.erase(*it);
        getRemainCoeff(coeff, newS, P1, N, _girth);
        s[i++] = newS.size();
        coeff.erase(*it);
    }
    vector<int> I(S.size());
    iota(I.begin(), I.end(), 0);
    sort(I.begin(), I.end(), [&s](int i, int j)
         { return s[i] > s[j]; });
    for (int i = 0; i < min(sz2, G[sz1]); i++)
    {
        if (_Tn.size() == n)
            return;
        int target = vecS[I[i]];
        bool exist = false;
        if (coeff.find(target) != coeff.end())
            exist = true;
        coeff.insert(target);
        S.erase(target);
        set<int> newS = S;
        getRemainCoeff(coeff, newS, P1, N, _girth);
        if (coeff.size() + newS.size() >= n)
            recursiveSearch(coeff, newS, N, n, P1, G);
        else
        {
            cout << "no solution for this a: " << P1[2] << endl;
            _Tn.clear();
            return;
        }
        // coeff needs erase target before backtracking
        if (exist == false)
            coeff.erase(target);
    }
}
