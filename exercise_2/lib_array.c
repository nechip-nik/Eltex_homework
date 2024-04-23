#include "lib_array.h"
#include <stdio.h>

#define N 3
#define M 10
#define X 5

void array() {
  int count = 1;
  printf("Вывод квадратной матрицы размера на N=3\n");
  int array[N][N];
  //   заполнение матрицы
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      array[i][j] = count;
      count++;
    }
  }
  //   вывод матрицы
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", array[i][j]);
    }
    printf("\n");
  }
}


void reverse() {
  printf("Вывод массива в обратном порядке размера на M=10\n");
  int array[M];
  for (int i = 0; i < M; i++) {
    array[i] = i + 1;
  }
  for (int i = 0; i < M; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
  int buff;
  for (int i = 0; i < M / 2; i++) {
    buff = array[i];
    array[i] = M - i;
    array[M - 1 - i] = buff;
  }
  for (int i = 0; i < M; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}


void triangle() {
  int array[M][M];
  int count;
  printf("Заполняет верхний треугольник матрицы 1, а нижний 0. Размер матрицы "
         "M=10\n");
  for (int i = 0; i < M; i++) {
    count = 0;
    for (int j = 0; j < M; j++) {
      if (count < M - i - 1) {
        array[i][j] = 0;
        count++;
      } else {
        array[i][j] = 1;
      }
    }
  }
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      printf("%d ", array[i][j]);
    }
    printf("\n");
  }
}


void printSpiral() {
  printf("Заполнение матрицы числами улиткой.\n");
  int array[X][X];
  int value = 1;
  int minCol = 0;
  int maxCol = X - 1;
  int minRow = 0;
  int maxRow = X - 1;
  while (value <= X * X) {
    for (int i = minCol; i <= maxCol; ++i) {
      array[minRow][i] = value;
      value++;
    }
    minRow++;

    for (int i = minRow; i <= maxRow; ++i) {
      array[i][maxCol] = value;
      value++;
    }
    maxCol--;

    for (int i = maxCol; i >= minCol; --i) {
      array[maxRow][i] = value;
      value++;
    }
    maxRow--;

    for (int i = maxRow; i >= minRow; --i) {
      array[i][minCol] = value;
      value++;
    }
    minCol++;
  }
  for (int i = 0; i < X; i++) {
    for (int j = 0; j < X; j++) {
      printf("%d\t", array[i][j]);
    }
    printf("\n");
  }
}