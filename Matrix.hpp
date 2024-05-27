#ifndef MATRIX
#define MATRIX

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>

#include <vector>
#include <thread>
#include <future>

using namespace std;

template<typename T> class Matrix;  // почему без этих двух строчек пре-объявления не работает переопределение <<
template<typename T> std::ostream& operator << (std::ostream&, const Matrix<T>&);
template<typename T> std::istream& operator >> (std::istream&, Matrix<T>&);

using namespace std;

template<typename T>
class Matrix
{
private:
   unsigned int number_of_rows, number_of_columns;
   T **matrix;

   void Delete(){
      for(unsigned int row = 0; row < number_of_rows; row++)
         delete[] matrix[row];
      delete[] matrix;
   // cout << "Matrix was deleted" << '\n'; //очистили выдленную память
   }
   //выделить память под матрицу number_of_rows х number_of_columns
   void Create(){ 
      if (!number_of_rows || !number_of_columns)
         throw invalid_argument("\n ERROR: Matrix must not have a number of rows/columns equal to zero \n");
      matrix = new T* [number_of_rows];
      for(unsigned int row = 0; row < number_of_rows; row++)
         matrix[row] = new T [number_of_columns];
      //cout << "Matrix was declared" << '\n';
   }
   //присвоить значение из потока
   void Assign(istream& from){ 
      for(unsigned int row = 0; row < number_of_rows; row++){
         for(unsigned int column = 0; column < number_of_columns; column++)
            from >> matrix[row][column];   
      }
      //cout << "Matrix was assigned with values" << '\n';
   }
   //присвоить дефолтное значение
   void Assign(T value){  
      for(unsigned int row = 0; row < number_of_rows; row++){
         for(unsigned int column = 0; column < number_of_columns; column++)
            matrix[row][column] = value;
      } 
      //cout << "Matrix was assigned with values" << '\n';  
   }
      
public:
   //конструктор по умолчанию
   Matrix() : matrix(nullptr), number_of_rows(0), number_of_columns(0) {}
   // конструктор для задания матрицы заданного размера со значениями из консоли/файла
   Matrix(unsigned int _number_of_rows_, unsigned int _number_of_columns_, istream& in){
      number_of_rows = _number_of_rows_;
      number_of_columns = _number_of_columns_;
      Create();
      Assign(in);
   }
   // конструктор для задания матрицы со значением по умолчанию
   Matrix(unsigned int _number_of_rows_, unsigned int _number_of_columns_, T default_value = T()){
      number_of_rows = _number_of_rows_;
      number_of_columns = _number_of_columns_;
      Create();
      Assign(default_value);
   }
   // конструктор для задания матрицы из консоли/файла
   Matrix(istream& in){
      in >> number_of_rows >> number_of_columns;
      Create();
      Assign(in);
   }
   // деструктор
   ~Matrix(){
      Delete();
   }
   bool IsSquare() const{
      return this->number_of_rows == this->number_of_columns;
   }
   // вывод матрицы в консоль
   void PrintMatrix() const{
      for(unsigned int row = 0; row < number_of_rows; row++){
         for(unsigned int column = 0; column < number_of_columns; column++)
            cout << setw(7) << setprecision(5) << matrix[row][column] << ' ';
         cout << '\n';
      }cout << '\n';
   }
   // округлить до n чисел после запятой
   void Round(unsigned int num = 7){ 
      for(unsigned int row = 0; row < number_of_rows; row++){
         for(unsigned int column = 0; column < number_of_columns; column++)
            matrix[row][column] = round(matrix[row][column]*(10^num))/(10^num);
      }
   }
   // элементарное преобразование 1
   void elemental_1(unsigned int row1, unsigned int row2){
      if(row1 > this->number_of_rows or row2 >this->number_of_rows)
         throw invalid_argument("\n ERROR: cannot apply elemental_1 (number of the row is out of range) \n");
      else{
         row1--; row2--;
         for(unsigned int column = 0; column < this->number_of_columns; column++)
            std::swap(this->matrix[row1][column], this->matrix[row2][column]);
      }
   }
   // элементарное преобразование 2
   void elemental_2(unsigned int row, double num){
      if(row > this->number_of_rows)
         throw invalid_argument("\n ERROR: cannot apply elemental_2 (number of the row is out of range) \n");
      else{
         row--;
         for(unsigned int column = 0; column < number_of_columns; column++)
            this->matrix[row][column] *= num;
      }
   }
   // элементарное преобразование 3
   void elemental_3(unsigned int row1, unsigned int row2, double num){
      if(row1 > this->number_of_rows or row2 >this->number_of_rows)
         throw invalid_argument("\n ERROR: cannot apply elemental_3 (number of the row is out of range) \n");
      else{
         row1--;row2--;
         for(unsigned int column = 0; column < this->number_of_columns; column++)
            this->matrix[row2][column] += this->matrix[row1][column] * num;
      }
   }
#if 0
      double Determinant(){
      double sum = 0;
      if (this->IsSquare()){
         if (number_of_rows == 1){
            return matrix[0][0];
         }else if (number_of_rows == 2){
            return matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
         }else {
            for (unsigned int column = 0; column < number_of_columns; column++){
               sum += (column % 2 == 0 ? 1 : -1) * matrix[0][column] * Minor(0, column).Determinant();
            }
         }
      } else {
         cerr << "ERROR: non-square matrix has no determinant" << '\n';
      }return sum;
   }
   void Twodify(const double *a){
      unsigned int count = 0;
      for (unsigned int i = 0; i < number_of_rows; ++i){
         for (unsigned int j = 0; j < number_of_columns; ++j){
            matrix[i][j] = a[count];
            count += 1;
         }
      }
   }
   Matrix<T> Minor(unsigned int row_current, unsigned int column_current){
      unsigned int count = 0;
      Matrix<T> result(number_of_rows - 1, number_of_columns - 1, 0);
      double data[result.number_of_rows * result.number_of_columns];
      for (unsigned int i = 0; i <= result.number_of_rows; ++i){
         for (unsigned int j = 0; j <= result.number_of_columns; ++j){
            if (i != row_current && j != column_current){
               data[count] = matrix[i][j];
               count += 1;
            }
         }
      }result.Twodify(data);
      return result;
   }
#endif
   // вычислить детерминант
   double Determinant() const{
      if(!(this->IsSquare())){
         throw invalid_argument("\n ERROR: non-square matrix has no determinant \n");
      }if(this->number_of_columns == 1)
         return this->matrix[0][0];
      T result = T();
      for(unsigned int column = 0; column < this->number_of_columns; column++){
         result += ( (column % 2 == 0) ? 1 : -1) * this->matrix[0][column] * this->Minor(0, column).Determinant();
      }return result;
   }
   // вычислить минор 
   Matrix<T> Minor(unsigned int row_current, unsigned int column_current) const{
      Matrix result(this->number_of_rows - 1, this->number_of_columns - 1, 0);
      for(unsigned int row = 0; row < this->number_of_rows; row++){
            if(row == row_current) continue;
            for(unsigned int column = 0; column < this->number_of_columns; column++){
                if(column == column_current) continue;
                result.matrix[(row > row_current) ? (row - 1) : row][(column > column_current) ? (column - 1) : column] = this->matrix[row][column];
            }
      }return result;
   }
  // транспонировать
   Matrix<T> Transponed() const{
      Matrix result(this->number_of_rows, this->number_of_columns, 0);
      for(unsigned int row = 0; row < this->number_of_rows; row++){
         for(unsigned int column = 0; column < this->number_of_columns; column++)
            result.matrix[row][column] = this->matrix[column][row];
      }return result;
   }
   Matrix<T> Linear() const{
      Matrix result(this->number_of_rows, this->number_of_columns, 0);
      for(unsigned int row = 0; row < this->number_of_rows; row++){
         for(unsigned int column = 0; column < this->number_of_columns; column++)
            result.matrix[row][column] = ((row + column) % 2 == 0 ? 1 : -1) * this->Minor(row, column).Determinant();
      }return result;
   }
   // посчитать обратную матрицу
   Matrix Inverse() const{
      if(!(this->IsSquare()) || !(this->Determinant()))
         throw invalid_argument("\n ERROR: non-square or singular matrix has no inverse matrix \n");
      else
         return ( this->Linear().Transponed() * (1 / this->Determinant()) );
   }
   // создать еденичную матрицу заданного размера
   static Matrix<T> identityMatrix(unsigned int num){
      Matrix<T> result(num, num, 0);
      for(unsigned int row = 0; row < num; row++){
            for(unsigned int column = 0; column < num; column++)
               result.matrix[row][column] = (row == column ? 1 : 0);
      }return result;
   }
   // создать нулевую матрицу заданного размера
   static Matrix<T> zeroMatrix(unsigned int _number_of_rows_, unsigned int _number_of_columns_){
      Matrix<T> result(_number_of_rows_, _number_of_columns_, 0);
      return result;
   }

   Matrix operator * (const Matrix&) const;
   Matrix operator * (double) const;

   Matrix operator + (const Matrix&) const;
   Matrix operator - (const Matrix&) const;

   bool operator == (const Matrix&) const;
   bool operator != (const Matrix&) const;
   bool operator == (double) const;
   bool operator != (double) const;
   Matrix<T>& operator = (const Matrix<T> &);

   // friend ostream& operator << (ostream &, const Matrix<T>&);
   friend ostream& operator << <> (ostream &, const Matrix<T>&); // что означает <> и почему просто верхняя строчка не работает
   friend istream& operator >> <> (istream &, Matrix<T>&);

   Matrix operator ! () const;

};
// матрица * матрица
template<typename T>
Matrix<T> Matrix<T>::operator * (const Matrix& other) const
{
   if(this->number_of_columns != other.number_of_rows){ // необходимое условие для перемножения матриц
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot multiply) \n");
   }else{
      Matrix result(this->number_of_rows, other.number_of_columns, T());
      for(unsigned int row = 0; row < result.number_of_rows; row++){
         for(unsigned int column = 0; column < result.number_of_columns; column++){
            T sum = T();
            for(unsigned int count = 0; count < this->number_of_columns; count++)
               sum += this->matrix[row][count] * other.matrix[count][column];
            result.matrix[row][column] = sum;
         }
      }return result;
   }
}
// матрица * скаляр
template<typename T>
Matrix<T> Matrix<T>::operator * (double number) const
{
   Matrix result(this->number_of_rows, this->number_of_columns, T());
   for(unsigned int row = 0; row < result.number_of_rows; row++){
      for(unsigned int column = 0; column < result.number_of_columns; column++)
         result.matrix[row][column] = this->matrix[row][column] * number;
   }return result;
}
// матрица + матрица
template<typename T>
Matrix<T> Matrix<T>::operator + (const Matrix<T>& other) const
{
   if(number_of_rows != other.number_of_rows or number_of_columns != other.number_of_columns){
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot add) \n");
   }else{
      Matrix result(number_of_rows, number_of_columns, T());
      vector<thread> threads;

      for(unsigned int row = 0; row < result.number_of_rows; row++){
			threads.push_back(thread([this, &result, &other, row](){
				for(unsigned int column = 0; column < result.number_of_columns; column++){
					result.matrix[row][column] = matrix[row][column] + other.matrix[row][column];
				}
			}));
		}
	
		for (thread& thread : threads)
			thread.join();

		return result;
   }
}
// матрица - матрица
template<typename T>
Matrix<T> Matrix<T>::operator - (const Matrix& other) const
{
   if(this->number_of_rows != other.number_of_rows or this->number_of_columns != other.number_of_columns){
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot subtract) \n");
   }else{
      Matrix result(this->number_of_rows, this->number_of_columns, T());
      for(unsigned int row = 0; row < result.number_of_rows; row++){
         for(unsigned int column = 0; column < result.number_of_columns; column++)
            result.matrix[row][column] = this->matrix[row][column] - other.matrix[row][column];
      }return result;
   }
}
// матрица == матрица
template<typename T>
bool Matrix<T>::operator == (const Matrix& other) const
{
   if(this->number_of_rows == other.number_of_rows and this->number_of_columns == other.number_of_columns){
      for(unsigned int row = 0; row < this->number_of_rows; row++){
         for(unsigned int column = 0; column < this->number_of_columns; column++){
            if(this->matrix[row][column] != other.matrix[row][column])
               return false;
         }
      }return true;
   }return false;
}
// матрица != матрица
template<typename T>
bool Matrix<T>::operator != (const Matrix& other) const
{
   if(*this == other){
      return false;
   }return true;
}
// матрица == скаляр
template<typename T>
bool Matrix<T>::operator == (double number) const
{
   if(!(this->IsSquare()))
      return false;
   else{
      for(unsigned int row = 0; row < this->number_of_rows; row++){
         for(unsigned int column = 0; column < this->number_of_columns; column++){
            if(row == column and this->matrix[row][column] != number)
               return false;
            else if(row != column and this->matrix[row][column] != 0)
               return false;
         }
      }return true;
   }
}
// матрица != скаляр
template<typename T>
bool Matrix<T>::operator != (double number) const
{
   if(!(this->IsSquare()))
      return true;
   else{
      for(unsigned int row = 0; row < this->number_of_rows; row++){
         for(unsigned int column = 0; column < this->number_of_columns; column++){
            if(row == column and this->matrix[row][column] != number)
               return true;
            else if(row != column and this->matrix[row][column] != 0)
               return true;
         }
      }return false;
   }
}
// матрица = матрица
template<typename T>
Matrix<T>& Matrix<T>::operator = (const Matrix<T> &other)
{
   if (this == &other)
      return *this;
   if (this->matrix != nullptr)
      this->Delete();

   this->number_of_rows = other.number_of_rows; 
   this->number_of_columns = other.number_of_columns;
   this->Create();

   for(unsigned int row = 0; row < this->number_of_rows; row++){
      for(unsigned int column = 0; column < this->number_of_columns; column++)
         this->matrix[row][column] = other.matrix[row][column];
   }return *this;
}
// поток вывода << матрица
template<typename T>
ostream& operator << (ostream &out, const Matrix<T>& result){
  for(unsigned int row = 0; row < result.number_of_rows; row++){
      for(unsigned int column = 0; column < result.number_of_columns; column++)
         out << setw(5) << setprecision(3) << result.matrix[row][column] << ' ';
      out << '\n';
   }out << '\n';
   return out;
}
// поток ввода >> матрица
template<typename T>
istream& operator >> (istream &in, Matrix<T>& result){
   result.Assign(in);
   return in;
}
// !матрица
template<typename T>
Matrix<T> Matrix<T>::operator ! () const{
   return this->Inverse();
}

#endif