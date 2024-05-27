#ifndef MATRIX
#define MATRIX

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>

#include <vector>
#include <thread>
#include <future>

template<typename T> class Matrix;  //TODO: почему без этих двух строчек пре-объявления не работает переопределение <<
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
   double Determinant_OldSchool() const{
      if(!(this->IsSquare())){
         throw invalid_argument("\n ERROR: non-square matrix has no determinant \n");
      }if(this->number_of_columns == 1)
         return this->matrix[0][0];

      double result = 0;

      for (unsigned int column = 0; column < this->number_of_columns; column++){
         result += ( (column % 2 == 0) ? 1 : -1) * this->matrix[0][column] * this->Minor(0, column).Determinant_OldSchool();
      }return result;
   }
#endif
   // вычислить детерминант (multihreaded)
   double Determinant() const{
      if (!(IsSquare())){
         throw invalid_argument("\n ERROR: non-square matrix has no determinant \n");
      }if (number_of_columns == 1)
         return matrix[0][0];

      double result = '\0';
      vector<thread> threads;

      for (unsigned int column = 0; column < number_of_columns; column++){
         threads.emplace_back( thread( [this, &result, column](){
            result += ( (column % 2 == 0) ? 1 : -1) * matrix[0][column] * Minor(0, column).Determinant(); 
         } ) );
      }

      for (thread& thr : threads)
         thr.join();

      return result;
   } 
   double asyncDeterminant(unsigned int block_size) const{
      if (!(IsSquare())){
         throw invalid_argument("\n ERROR: non-square matrix has no determinant \n");
      }if (number_of_columns == 1)
         return matrix[0][0];
      
      double result = '\0';
      vector<future<double>> futures;

      for (unsigned int column = 0; column < number_of_columns; column += block_size){
			futures.push_back( async( launch::async, [&, column, block_size](){
				double block_determinant = '\0';
				for (unsigned int column_ = column; column_ < min(column + block_size, number_of_columns); column_++)
					block_determinant += ( (column_ % 2 == 0) ? 1 : -1) * matrix[0][column_] * Minor(0, column_).asyncDeterminant(block_size);
				return block_determinant;
			} ) );
		}

		for (future<double>& f : futures)
			result += f.get();

		return result;
   }
   // вычислить минор 
   Matrix<T> Minor(unsigned int row_current, unsigned int column_current) const{
      Matrix result(number_of_rows - 1, number_of_columns - 1);
      for(unsigned int row = 0; row < number_of_rows; row++){
            if(row == row_current) continue;
            for(unsigned int column = 0; column < number_of_columns; column++){
                if(column == column_current) continue;
                result.matrix[(row > row_current) ? (row - 1) : row][(column > column_current) ? (column - 1) : column] = matrix[row][column];
            }
      }return result;
   }
  // транспонировать (multithreaded)
   Matrix<T> Transpose() const{
      Matrix result(number_of_columns, number_of_rows);
      vector<thread> threads;

      for(unsigned int row = 0; row < number_of_rows; row++){
         threads.emplace_back( thread( [this, &result, row](){
            for(unsigned int column = 0; column < number_of_columns; column++){
               result.matrix[column][row] = matrix[row][column];
            }
         } ) );
      }
         
      for(thread& thr : threads)
         thr.join();
      
      return result;
   }
   Matrix<T> asyncTranspose(unsigned int block_size) const{
      Matrix result(number_of_columns, number_of_rows);
      vector<future<void>> futures;

      for (unsigned int row = 0; row < number_of_rows; row += block_size){
			for (unsigned int column = 0; column < number_of_columns; column += block_size){
				futures.push_back( async( launch::async, [&, row, column, block_size](){
					for (unsigned int row_ = row; row_ < min(row + block_size, number_of_rows); row_++){
						for (unsigned int column_ = column; column_ < min(column + block_size, number_of_columns); column_++)
							result.matrix[column_][row_] = matrix[row_][column_];
					}
				} ) );
			}
		}

		for (future<void>& f : futures)
			f.wait();

		return result;
   }
   // посчитать линейное дополнение (multithreaded)
   Matrix<T> Linear() const{
      Matrix result(number_of_rows, number_of_columns);
      vector<thread> threads;

      for(unsigned int row = 0; row < number_of_rows; row++){
         for(unsigned int column = 0; column < number_of_columns; column++){
            threads.emplace_back( thread( [this, &result, column, row](){
               result.matrix[row][column] = ((row + column) % 2 == 0 ? 1 : -1) * this->Minor(row, column).Determinant();
            } ) );
         }
      }
      
      for (thread& thr : threads)
         thr.join();
      
      return result;
   }
   Matrix<T> asyncLinear(unsigned int block_size) const {
      Matrix result(number_of_rows, number_of_columns);
      //vector<future<Matrix<T>>> futures;
      vector<future<void>> futures;

      for (unsigned int row = 0; row < number_of_rows; row += block_size){
         for (unsigned int column = 0; column < number_of_columns; column += block_size){
            futures.push_back( async( launch::async, [&, row, column, block_size](){
               for (unsigned int row_ = row; row_ < min(row + block_size, number_of_rows); row_++){
						for (unsigned int column_ = column; column_ < min(column + block_size, number_of_columns); column_++)
							result.matrix[row_][column_] = ((row_ + column_) % 2 == 0 ? 1 : -1) * Minor(row_, column_).Determinant();
               }
            } ) );
         }
      }

      for (future<void>& f : futures)
         f.wait();

      return result;
   }
   // посчитать обратную матрицу (multithreaded)
   Matrix Inverse() const{
      if(!(IsSquare()) || !(Determinant()))
         throw invalid_argument("\n ERROR: non-square or singular matrix has no inverse matrix \n");
      else
         return ( Linear().Transpose() * (1 / Determinant()) );
   }
   // создать еденичную матрицу заданного размера
   static Matrix<T> identityMatrix(unsigned int num){
      Matrix<T> result(num, num);
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
   Matrix asyncMultiply(const Matrix&, unsigned int) const;

   Matrix operator * (double) const;
   Matrix asyncMultiply(double, unsigned int) const;

   Matrix operator + (const Matrix&) const;
   Matrix asyncAdd(const Matrix& other, unsigned int block_size) const;

   Matrix operator - (const Matrix&) const;
   Matrix asyncSubtract(const Matrix&, unsigned int) const;

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
// матрица * матрица (multithreaded)
template<typename T>
Matrix<T> Matrix<T>::operator * (const Matrix& other) const
{ 
   if(other.number_of_rows != this->number_of_columns){ // необходимое условие для перемножения матриц
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot multiply) \n");
   }else{
      Matrix result(number_of_rows, other.number_of_columns);
      vector<thread> threads;

      for (unsigned int row = 0; row < result.number_of_rows; row++){
			threads.emplace_back(thread([this, &result, &other, row](){
				for (unsigned int column = 0; column < result.number_of_columns; column++){
					for (unsigned int index = 0; index < number_of_columns; index++)
						   result.matrix[row][column] += matrix[row][index] * other.matrix[index][column];
				}
			}));
		}
	
		for (thread& thr : threads)
			thr.join();

		return result;
   }
}
template<typename T>
Matrix<T> Matrix<T>::asyncMultiply(const Matrix& other, unsigned int block_size) const{
   if (other.number_of_rows != number_of_columns)
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot multiply) \n");

   Matrix result(number_of_rows, other.number_of_columns);
	vector<future<void>> futures;

	for (unsigned int row = 0; row < number_of_rows; row += block_size){
		for (unsigned int column = 0; column < other.number_of_columns; column += block_size){
			futures.push_back( async( launch::async, [&, row, column, block_size](){
				for (unsigned int row_ = row; row_ < min(row + block_size, number_of_rows); row_++){
					for (unsigned int column_ = column; column_ < min(column + block_size, other.number_of_columns); column_++){
                  for (unsigned int index = 0; index < number_of_columns; index++)
						   result.matrix[row_][column_] += matrix[row_][index] * other.matrix[index][column_];
               }
				}
			} ) );
		}
	}

	for (future<void>& f : futures)
		f.wait();
	
   return result;
}
// матрица * скаляр (multithreaded)
template<typename T>
Matrix<T> Matrix<T>::operator * (double number) const
{
   Matrix result(number_of_rows, number_of_columns, T());
   vector<thread> threads;

   for(unsigned int row = 0; row < result.number_of_rows; row++){
		threads.emplace_back(thread([this, &result, number, row](){
			for(unsigned int column = 0; column < result.number_of_columns; column++){
				result.matrix[row][column] = matrix[row][column] * number;
			}
		}));
	}
	
	for (thread& thr : threads)
		thr.join();

	return result;
}
template<typename T>
Matrix<T> Matrix<T>::asyncMultiply(double number, unsigned int block_size) const{
	Matrix result(number_of_rows, number_of_columns);
	vector<future<void>> futures;

	for (unsigned int row = 0; row < number_of_rows; row += block_size){
		for (unsigned int column = 0; column < number_of_columns; column += block_size){
			futures.push_back( async( launch::async, [&, row, column, number, block_size](){
				for (unsigned int row_ = row; row_ < min(row + block_size, this->number_of_rows); row_++){
					for (unsigned int column_ = column; column_ < min(column + block_size, this->number_of_columns); column_++)
						result.matrix[row_][column_] = this->matrix[row_][column_] * number;
				}
			} ) );
		}
	}

	for (future<void>& f : futures)
		f.wait();
	
   return result;
}
// матрица + матрица (multithreaded)
template<typename T>
Matrix<T> Matrix<T>::operator + (const Matrix<T>& other) const
{
   if(number_of_rows != other.number_of_rows or number_of_columns != other.number_of_columns){
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot add) \n");
   }else{
      Matrix result(number_of_rows, number_of_columns, T());
      vector<thread> threads;

      for(unsigned int row = 0; row < result.number_of_rows; row++){
			threads.emplace_back(thread([this, &result, &other, row](){
				for(unsigned int column = 0; column < result.number_of_columns; column++){
					result.matrix[row][column] = matrix[row][column] + other.matrix[row][column];
				}
			}));
		}
	
		for (thread& thr : threads)
			thr.join();

		return result;
   }
}
template<typename T>
Matrix<T> Matrix<T>::asyncAdd(const Matrix& other, unsigned int block_size) const{
   if (number_of_rows != other.number_of_rows || number_of_columns != other.number_of_columns)
		throw invalid_argument("\n ERROR: Matrix are not compatible (cannot add) \n");

	Matrix result(number_of_rows, number_of_columns);
	vector<future<void>> futures;
	for (unsigned int row = 0; row < number_of_rows; row += block_size){
		for (unsigned int column = 0; column < number_of_columns; column += block_size){
			futures.push_back( async( launch::async, [&, row, column, block_size](){
				for (unsigned int row_ = row; row_ < min(row + block_size, this->number_of_rows); row_++){
					for (unsigned int column_ = column; column_ < min(column + block_size, this->number_of_columns); column_++)
						result.matrix[row_][column_] = this->matrix[row_][column_] + other.matrix[row_][column_];
				}
			} ) );
		}
	}

	for (future<void>& f : futures)
		f.wait();
	
   return result;
}
// матрица - матрица (multithreaded)
template<typename T>
Matrix<T> Matrix<T>::operator - (const Matrix& other) const
{
   if(this->number_of_rows != other.number_of_rows or this->number_of_columns != other.number_of_columns){
      throw invalid_argument("\n ERROR: Matrix are not compatible (cannot subtract) \n");
   }else{
      Matrix result(number_of_rows, number_of_columns, T());
      vector<thread> threads;

      for(unsigned int row = 0; row < result.number_of_rows; row++){
			threads.emplace_back(thread([this, &result, &other, row](){
				for(unsigned int column = 0; column < result.number_of_columns; column++){
					result.matrix[row][column] = matrix[row][column] - other.matrix[row][column];
				}
			}));
		}
	
		for (thread& thr : threads)
			thr.join();

		return result;
   }
}
template<typename T>
Matrix<T> Matrix<T>::asyncSubtract(const Matrix& other, unsigned int block_size) const{
   if (number_of_rows != other.number_of_rows || number_of_columns != other.number_of_columns)
		throw invalid_argument("\n ERROR: Matrix are not compatible (cannot subtract) \n");

	Matrix result(number_of_rows, number_of_columns);
	vector<future<void>> futures;
	for (unsigned int row = 0; row < number_of_rows; row += block_size){
		for (unsigned int column = 0; column < number_of_columns; column += block_size){
			futures.push_back( async( launch::async, [&, row, column, block_size](){
				for (unsigned int row_ = row; row_ < min(row + block_size, this->number_of_rows); row_++){
					for (unsigned int column_ = column; column_ < min(column + block_size, this->number_of_columns); column_++)
						result.matrix[row_][column_] = this->matrix[row_][column_] - other.matrix[row_][column_];
				}
			} ) );
		}
	}

	for (future<void>& f : futures)
		f.wait();
	
   return result;
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
// !матрица (multithreaded)
template<typename T>
Matrix<T> Matrix<T>::operator ! () const{
   if(!(IsSquare()) || !(Determinant()))
      throw invalid_argument("\n ERROR: non-square or singular matrix has no inverse matrix \n");
   else
      return ( Linear().Transpose() * (1 / Determinant()) );
}

#endif