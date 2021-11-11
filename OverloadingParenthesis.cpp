// Overloading of parenthesis () operator
// Date:        November 11, 2021
// Author:      Caglayan DOKME, caglayandokme@gmail.com

/** Libraries **/
#include <iostream>

// Class for storing matrixes
template<class DataType, std::size_t ROW, std::size_t COLUMN>
class Matrix {
    static_assert(ROW       != 0, "Row size cannot be 0!");
    static_assert(COLUMN    != 0, "Column size cannot be 0!");

public:
    Matrix() = default;

    // Parenthesis overloaded
    DataType& operator()(const std::size_t row, const std::size_t column)
    {
        if((row >= ROW) || (column >= COLUMN))
            throw ("Index out-of-range error!");

        return data[row][column];
    }

    // Parenthesis overloaded
    const DataType& operator()(const std::size_t row, const std::size_t column) const
    {
        if((row >= ROW) || (column >= COLUMN))
            throw ("Index out-of-range error!");

        return data[row][column];
    }

private:
    DataType data[ROW][COLUMN];
};

int main() 
{
    // Program constants
    static const std::size_t ROW    = 3;
    static const std::size_t COLUMN = 4;

    // Creation of matrix
    Matrix<int, ROW, COLUMN> matrix;

    // Fill the matrix with incremental data
    for(std::size_t row = 0; row < ROW; ++row)
        for(std::size_t col = 0; col < COLUMN; ++col)
            matrix(row, col) = row + col;   // Parenthesis used
    
    // Print out the matrix content
    for(std::size_t row = 0; row < ROW; ++row)
    {
        for(std::size_t col = 0; col < COLUMN; ++col)
            std::cout << matrix(row, col) << " ";   // Parenthesis used

        std::cout << std::endl;
    }

	std::cout << "Program finished." << std::endl << std::endl;

	return 0;
}
