#include <iostream>
#include <vector>
#include <map>
#include <tuple>

#include "Matrix.hpp"

int main(){

    Matrix<int, 0> matrix;

    for(int i = 0; i < 10; ++i){
        matrix[i][i] = i;
    }

    for(int i = 0; i < 10; ++i){
        matrix[i][9 - i] = i;
    }

    for(int i = 1; i < 9; ++i){
        for(int j = 1; j < 9 ; ++j){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    

    std::cout << "Matrix size = " << matrix.Size() << std::endl;

    for(auto c: matrix){
        size_t x;
        size_t y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << x << " " << y << " " << v << std::endl;
    }

    return 0;
}