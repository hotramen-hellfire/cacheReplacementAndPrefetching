#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <chrono>

class LinearRegression {
private:
    std::vector<double> weights;
    std::vector<std::vector<double> > X;
    std::vector<double> Y;

    void matrix_vector_multiply(double** A, int n, int d, std::vector<double> &vec, std::vector<double> &weights)
    {
        assert(n > 0);
        assert(d > 0);
        assert(vec.size() == d);
        for(size_t i = 0; i < n; ++i)
        {
            double sum = 0;
            for(size_t j = 0; j < d; ++j)
            {
                sum = sum + A[i][j]*vec[j];
            }
            weights[i] = sum;
        }
        return;
    }

    void matrix_transpose(std::vector<std::vector<double> > &matr, double** ret_val)
    {
        size_t n = matr.size();
        assert(n > 0);
        size_t d = matr[0].size();
        assert(d > 0);
        for(size_t j = 0; j < d; ++j)
        {
            for(size_t i = 0; i < n; ++i)
            {
                ret_val[j][i]=matr[i][j];
            }
        }
        return;
    }

    void matrix_multiply(double** A, int n, int k, std::vector<std::vector<double> > &B, double** ret_val)
    {
        assert(B.size() == k);
        size_t d = B[0].size();
        assert(d > 0);
        for (int i=0; i<n; ++i)
        {
            for (int j=0; j<d; ++j)
            {
                double accumulate = 0;
                for (int l=0; l<k; ++l)
                {
                    accumulate=accumulate+A[i][l]*B[l][j];
                }
                ret_val[i][j]=accumulate;
            }
        }
        return;
    }

    void matrix_multiply(double** A, int n, int k, double** B, int n1, int d, double** ret_val)
    {
        assert(n1 == k);
        assert(d > 0);
        for (int i=0; i<n; ++i)
        {
            for (int j=0; j<d; ++j)
            {
                double accumulate = 0;
                for (int l=0; l<k; ++l)
                {
                    accumulate=accumulate+A[i][l]*B[l][j];
                }
                ret_val[i][j]=accumulate;
            }
        }
        return;
    }

    void matrix_inverse(double** matrix, int n)
    {
       // Augment the matrix [A | I]
        for (int i = 0; i < n; ++i) {
            for (int j = n; j < 2*n; ++j) {
                if (j-n!=i)
                {
                    matrix[i][j]=0;
                }
                else
                {
                    matrix[i][j]=1;
                }
            }
        }

        for (int i = 0; i < n; ++i) {
            assert(std::abs(matrix[i][i]) > 1e-10);

            // Make the diagonal contain all ones
            double diag = matrix[i][i];
            for (int j = 0; j < 2*n; ++j) {
                matrix[i][j] = matrix[i][j]/diag;
            }

            // Make the other rows contain zeros
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    double ratio = matrix[j][i];
                    for (int k = 0; k < 2*n; ++k) {
                        matrix[j][k] = matrix[j][k] - ratio * matrix[i][k];
                    }
                }
            }
        }

        for (int i=0; i<n; ++i)
        {
            for (int j=n; j<2*n; ++j)
            {
                matrix[i][j-n]=matrix[i][j];
            }
        }
        return;
    }

public:
    LinearRegression(std::vector<std::vector<double>> x, std::vector<double> y)
    {
        assert(x.size() == y.size());
        size_t n = x.size();
        assert(n > 0);
        size_t d = x[0].size();
        assert(d > 0);
        for(size_t i = 1; i < n; ++i) assert(x[i].size() == d);
        X.resize(n, {1});
        weights.resize(d + 1, 0);
        for(size_t i = 0; i < n; ++i)
        {
            for(size_t j = 0; j < d; ++j) X[i].push_back(x[i][j]);
        }
        Y = y;
    }

    void fit() 
    {
        int n = X.size();
        int d = X[0].size();
        for (auto it : X) assert(it.size()==d);
        
        double** XT = new double*[d];
        for (int i=0; i<d; ++i)
        XT[i] = new double[n];
        
        matrix_transpose(X, XT);
        
        double** W = new double*[d];
        for (int i=0; i<d; ++i)
        W[i] = new double[2*d];
        
        matrix_multiply(XT, d, n, X, W);
        matrix_inverse(W, d);
        
        double** Z = new double*[d];
        for (int i=0; i<d; ++i)
        Z[i] = new double[n];
        
        matrix_multiply(W, d, d, XT, d, n, Z);
        matrix_vector_multiply(Z, d, n, Y, weights);
        
        for (int i=0; i<d; ++i)
        {
            delete [] XT[i];
            delete [] W[i];
            delete [] Z[i];
        }
        
        delete [] XT;
        delete [] W;
        delete [] Z;
        
        return;
    }

    double loss() const
    {
        double sum = 0;
        for(size_t i = 0; i < Y.size(); ++i)
        {
            double err = (Y[i] - predict(X[i]));
            sum = sum+err*err;
        }
        return sum/Y.size();
    }

    double predict(std::vector<double> x) const 
    {
        double sum = 0;
        for (size_t i = 0; i < x.size(); ++i) 
        {
            sum = sum + weights[i] * x[i];
        }
        return sum;
    }
};

int main(int argc, char **argv) {
    // Open data.txt for reading
    std::ifstream infile("data.txt");
    if (!infile.is_open()) {
        std::cerr << "Error opening data.txt" << std::endl;
        return 1;
    }

    // Read in the number of training examples
    int n_samples;
    infile >> n_samples;

    // Read in the number of input features
    int n_features;
    infile >> n_features;

    // Read in the training examples
    std::vector<std::vector<double>> X(n_samples, std::vector<double>(n_features));
    std::vector<double> y(n_samples);

    for (int i = 0; i < n_samples; ++i) {
        for (int j = 0; j < n_features; ++j) {
            infile >> X[i][j];
        }
        infile >> y[i];
    }

    infile.close(); // Close the file

    auto start = std::chrono::high_resolution_clock::now();

    LinearRegression model(X, y);
    model.fit();
    double loss = model.loss();

    auto stop = std::chrono::high_resolution_clock::now();

    std::cout <<loss << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()/1e6<< std::endl;

    return 0;
}
