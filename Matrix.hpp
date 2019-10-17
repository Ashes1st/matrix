#ifndef MATRIX
#define MATRIX 1

#include <iostream>
#include <vector>
#include <map>
#include <tuple>

template<typename T, const T baseValue>
class Matrix{
private:
    class Point{
    public:
        size_t lineNum;
        size_t columnNum;
        T value;
        bool isTemp = false;
        class Matrix::Column * clmn;
    
        Point(){
            lineNum = 0;
            columnNum = 0;
            value = baseValue;
            clmn = nullptr;
        }

        Point(class Matrix::Column * _clmn, bool _isT){
            lineNum = 0;
            columnNum = 0;
            value = baseValue;
            clmn = _clmn;
            isTemp = _isT;
        }

        Point(const Point & copyObj, const T & _value){
            lineNum = copyObj.lineNum;
            columnNum = copyObj.columnNum;
            value = _value;
            clmn = copyObj.clmn;
        }

        void setLineNum(size_t _lineNum){
            lineNum = _lineNum;
        }

        void setColumnNum(size_t _columnNum){
            columnNum = _columnNum;
        }

        operator std::tuple<size_t&, size_t&, T&>(){
            return std::tuple<size_t&, size_t&, T&>(lineNum, columnNum, value);
        }

        operator T(){
            if(isTemp)
                return baseValue;
            return value;
        }
    };

    /**
     * @brief Обертка над классом Point для возможности корректного удаления объектов матрицы
     * 
     */
    class PointWrapper{
    public:
        Point * currentPoint;
        class Matrix::Column * currentColumn;

        PointWrapper(){
            currentPoint = nullptr;
            currentColumn = nullptr;
        }

        PointWrapper(Point & curPoint, class Matrix::Column & curColumn){
            currentPoint = &curPoint;
            currentColumn = &curColumn;
        }

        Point & operator=(const T & right){
            if(right == baseValue){
                if(currentPoint->isTemp){
                    return currentColumn->tempPoint;
                }
                --currentColumn->mtr->matrixSize;
                currentColumn->column.erase(currentColumn->column.find(currentPoint->columnNum));
                return currentColumn->tempPoint;
            }
            
            else{
                if(currentPoint->isTemp){     
                    currentPoint->clmn = currentPoint->clmn->createIfNot();
                    currentPoint->clmn->column.insert({currentPoint->columnNum, Point(*currentPoint, right)});
                    ++currentColumn->mtr->matrixSize;
                    return currentPoint->clmn->column[currentPoint->columnNum];
                }
                currentPoint->value = right;
                return *currentPoint;
            }
        }

        operator Point(){
            return *currentPoint;
        }

        operator T(){
            return currentPoint->value;
        }

        operator std::tuple<int&, int&, T&>(){
            return std::tuple<int&, int&, T&>(currentPoint->lineNum, currentPoint->columnNum, currentPoint->value);
        }

        PointWrapper & operator=(const PointWrapper & right){
            currentPoint = right.currentPoint;
            currentColumn = right.currentColumn;
            return *this;
        }
    };

    class Column{
    public:
        size_t lineNum;
        size_t columnNum;
        std::map<size_t, Point> column;

        Point tempPoint = Point(this, true);
        PointWrapper pWrapper;
        bool isTemp = false;
        Matrix * mtr;

        Column(){
            lineNum = 0;
            columnNum = 0;
            mtr = nullptr;
        }

        Column(Matrix * _mtr, bool _isT){
            lineNum = 0;
            columnNum = 0;
            mtr = _mtr;
            isTemp = _isT;
        }

        Column(const Column & copyObj){
            lineNum = copyObj.lineNum;
            columnNum = copyObj.columnNum;
            mtr = copyObj.mtr;
            isTemp = false;
        }

        Column * createIfNot(){
            if(isTemp){
                mtr->line.insert({lineNum, Column(*this)});
                isTemp = true;
                return &mtr->line[lineNum];
            }
            return this;
        }

        void setLineNum(size_t _lineNum){
            lineNum = _lineNum;
            updateTmpPoint();
        }

        void updateTmpPoint(){
            tempPoint = Point(this, true);
        }

        PointWrapper & operator[](size_t _columnNum){
            auto point = column.find(_columnNum);

            if(point != column.end()){ //element was found
                pWrapper = PointWrapper(point->second, *this);
                return pWrapper;
            }
            else{ // element not found
                tempPoint.setLineNum(lineNum);
                tempPoint.setColumnNum(_columnNum);
                pWrapper = PointWrapper(tempPoint, *this);
                return pWrapper;
            }
        }
    };

    /**
     * @brief Класс итератор для матрицы
     * 
     */
    class MatrixIterator{
    private:
        Matrix * matrix;
        typename std::map<size_t, Column>::iterator _line;
        typename std::map<size_t, Point>::iterator _column;

    public:

        /**
         * @brief Construct a new Matrix Iterator object
         * Получаем итератор на первый существующий элемент матрицы
         * 
         * @param _matrix 
         */
        MatrixIterator(Matrix * _matrix){
            matrix = _matrix;
            for(_line = matrix->line.begin(); _line != matrix->line.end(); _line++){
                if(_line->second.column.empty()){
                    continue;
                }
                _column = _line->second.column.begin();
                break;
            }
        } 

        MatrixIterator(Matrix * _matrix, bool endRequest){
            if(endRequest){
                matrix = _matrix;
                if(matrix->Size() != 0){
                    for(_line = matrix->line.end(); _line != matrix->line.begin();){
                        _line--;
                        if(_line->second.column.empty()){
                            continue;
                        }
                        _column = _line->second.column.end();
                        break;
                    }
                }
                else{
                    _line = matrix->line.end();
                    _column = _line->second.column.end();
                }
            }
        }

        bool operator!=(const MatrixIterator & other){
            if(_column != other._column)
                return true;
            if(_line != other._line)
                return true;

            return false;
        }

        MatrixIterator & operator++(){
            if(_line != MatrixIterator(matrix, true)._line){ // Не последняя занятая строка
                ++_column;
                if(_column == _line->second.column.end())
                {
                    ++_line;
                    while (_line->second.column.empty())
                    {
                        ++_line;
                    }
                    _column = _line->second.column.begin();
                }
            }
            else{ // Последняя занятая строка
                if(_column != _line->second.column.end())
                {
                    ++_column;
                }
            }
            return *this;
        }
        
        Point operator*(){
            return _column->second;
        }
    };

    Column tempColumn = Column(this, true);
    std::map<size_t, Column> line;
    size_t matrixSize = 0;


public:
    Column & operator[](const size_t & lineNum){
        auto col = line.find(lineNum);
    
        if(col != line.end()){ //element was found
            return line[lineNum];
        }
        else{ // element not found
            tempColumn.setLineNum(lineNum);
            return tempColumn;
        }
    }

    size_t Size(){ return matrixSize; }

    MatrixIterator begin(){
        if(matrixSize == 0){
            return MatrixIterator(this, true);
        }
        return MatrixIterator(this);
    }

    MatrixIterator end(){
        return MatrixIterator(this, true);
    }
};

#endif