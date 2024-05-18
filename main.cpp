#include "Matrix.hpp"

int main(){
   
  /*
  ВВОД МАТРИЦЫ ИЗ КОНСОЛИ/ФАЙЛА
  1.0. для создания матрицы, размер которой указывается в файле/консоли,
  использовать конструктор матрицы, в который передается поток ввода, пример:
  ifstream file("input.txt") ;      или    Matrix<double> matrix(cin);  
  Matrix<double> matrix(file); 
  
  1.1. для создания матрицы определенного размера и заполенения ее значениями из файла/консоли,
  использовать конструктор, принимающий размеры матрицы и поток ввода, пример:
  ifstream file("input.txt")       ;      или    Matrix<double> matrix(3, 3, cin);
  Matrix<double> matrix(3, 3, file);

  2.0. для изменения значения значений уже созданной матрицы размера N x M
  использовать оператор >>, принимающий поток ввода и саму матрицу, пример:
  ifstream file("input.txt");      или    cin >> matrix;
  file >> matrix            ;
  */
  
  /*ЕДЕНИЧНАЯ И НУЛЕВАЯ МАТРИЦЫ*/
  cout << "Identity matrix: \n" << Matrix<int>::identityMatrix(4)
       << "Zero matrix: \n" << Matrix<int>::zeroMatrix(2, 5);

  /*ЗАДАНИЕ МАТРИЦ*/
  cout << "\n first of all, insert hight and width (matrix has to be square), then values: \n";
  Matrix<double> cmatrix1(cin);       // размер и значения матрицы задаются через консоль (создаем новую матрицу)

  cout << "\n insert only values for matrix 3x3 starting with [0.0], [0, 1] ... : \n";
  Matrix<double> cmatrix2(3, 3, cin); // только значения матрицы задаются через консоль   (создаем новую матрицу)
  
  ifstream file1("size_and_values.txt");
  Matrix<double> fmatrix1(file1);       // размер и значения матрицы задаются из файла     (создаем новую матрицу)
  file1.close();

  ifstream file2("only_values.txt"); 
  Matrix<double> fmatrix2(3, 3, 0);
  file2 >> fmatrix2;                    // матрица принимает значения из файла             (изменяем имеющуюся матрицу)
  Matrix<double> fmatrix3(5, 5, file2); // только значения матрицы задаются через консоль  (создаем новую матрицу)
  file2.close();

/*ОСНОВНЫЕ ВЫЧИСЛЕНИЯ*/ 
  Matrix<double> computing1 = fmatrix1 + fmatrix2 -cmatrix2;
  cout << "\n fmatrix1 + fmatrix2 - cmatrix2: \n" << computing1;

  Matrix<double> computing2 = fmatrix1 * fmatrix2 * cmatrix1.Determinant();
  cout << "\n fmatrix2 * fmatrix1 * cmatrix1.Determinant(): \n" << computing2;

  // обработка случая когда fmatrix1 вырожденная => не имеет обратной (fmatrix1 задается из файла size_and_values.txt)
  Matrix<double> computing3;
  try {
    computing3 = fmatrix1 * !fmatrix1 * fmatrix1.Determinant();
    cout << "\n fmatrix1 * !fmatrix1 * fmatrix1.Determinant(): \n" << computing3;
  } catch (invalid_argument error) {
    do {
    cout << error.what() << "\n fmatrix1: \n" << fmatrix1
         << "re-initialize fmatrix1 so it will have an enverse matrix (fmatrix1.Determinant() should NOT equal 0) \n"
         << "enter values for matrix 3x3: \n";
    cin >> fmatrix1;
    } while (!fmatrix1.Determinant());
    computing3 = fmatrix1 * !fmatrix1 * fmatrix1.Determinant();
    cout << "\n fmatrix1 * !fmatrix1 * fmatrix1.Determinant(): \n" << computing3;
  }

  cout << "\n determinant of fmatrix3 (size: 5x5): \n" << setw(5) << fmatrix3.Determinant() << '\n';

  cout << "\n USED MATRIXES: \n" << "cmatrix1 \n" << cmatrix1 << "cmatrix2 \n" << cmatrix2
     << "fmatrix1 \n" << fmatrix1 << "fmatrix2 \n" << fmatrix2 << "fmatrix3 \n" << fmatrix3;

/*ЗАПИСЬ В ФАЙЛ*/
  ofstream fout("output.txt");
  fout << "\n fmatrix1 + fmatrix2 - cmatrix2: \n" << computing1
     << "\n fmatrix2 * fmatrix1 * cmatrix1.Determinant(): \n" << computing2
     << "\n fmatrix1 * !fmatrix1 * fmatrix1.Determinant(): \n" << computing3
     << "\n determinant of fmatrix3 (size: 5x5): \n" << setw(5) << fmatrix3.Determinant() << '\n'
     << "\n USED MATRIXES: \n\n" << "cmatrix1 \n" << cmatrix1 << "cmatrix2 \n" << cmatrix2
     << "fmatrix1 \n" << fmatrix1 << "fmatrix2 \n" << fmatrix2 << "fmatrix3 \n" << fmatrix3;
  fout.close();

  return 0;
} 
