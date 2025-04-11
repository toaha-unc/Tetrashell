#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// function to convert binary to decimal
int binaryToDecimal(int n) {
  int decimal_number = 0;
  int reminder;
  int base_value = 1;
  int cross_firstMultiplier = 0;
  while (n != 0) {
    reminder = n % 10;
    n = n / 10;
    if (cross_firstMultiplier == 1) {
      base_value = base_value * 2;
    } else {
      cross_firstMultiplier = 1;
    }
    decimal_number = decimal_number + reminder * base_value;
  }
  return decimal_number;
}

// function to print from decimal to binary
int decimalToBinary(int n) {
  int p = 0;
  int m = 0;
  int y = 1;
  int x = 0;
  while (n != 0) {
    x = n % 2;
    n = n / 2;
    if (p == 1) {
      y = y * 10;
      m = m + (y * x);
    } else {
      m = m + x;
      p = 1;
    }
  }
  return m;
}

int rando(int lower, int upper, int count) {
  int i, num;
  for (i = 0; i < count; i++) {
    num = (rand() % (upper - lower + 1)) + lower;
  }
  return num;
}

int main(int argc, char* argv[]) {
  srand(time(0));
  int n = rando(0, 15, 15);
  int type = rando(3, 10, 8);
  char bin[] = "binary";
  char dec[] = "decimal";
  int input = 0;
  int actual = 0;
  int m = 0;
  if (type < 6) {
    printf("Convert %x into %s: ", n, bin);
    scanf("%i", &input);
    fgetc(stdin);
    actual = decimalToBinary(n);
    if (input == actual) {
      n = rando(16, 30, 15);
      printf("Convert the hexadecimal %X into %s: ", n, bin);
      scanf("%i", &input);
      fgetc(stdin);
      actual = decimalToBinary(n);
      if (input == actual) {
        printf("Correct!\nTry running the program again!\n");
      } else {
        printf(
            "Incorrect.\nThe correct answer is %i. Converting hex into binary "
            "requires to first convert the number into decimal by the sum of "
            "multiplying each position of hex by its base 16 multiplied by its "
            "digit position and then dividing the decimal by 2 until quotient "
            "is 0. The remainders from bottom to top is the binary number. Try running "
            "the program again!\n",
            actual);
        //fgetc(stdin);
      }
    } else {
      printf(
          "Incorrect.\nThe correct answer is %i. Converting hex into binary "
          "requires to first convert the number into decimal by the sum of "
          "multiplying each position by its base 16 multiplied by its digit "
          "position and then dividing the decimal by 2 until quotient is 0. "
          "The remainders from Bottom to top is the binary number.\n",
          actual);
      n = rando(0, 5, 6);
      printf("Convert the hexadecimal %X into %s: ", n, bin);
      scanf("%i", &input);
      fgetc(stdin);
      actual = decimalToBinary(n);
      if (input == actual) {
        printf("Correct!\nTry running the program again!\n");
      } else {
        printf(
            "Incorrect.\nThe correct answer is %i. Converting hex into binary "
            "requires to first convert the number into decimal by the sum of "
            "multiplying each position of hex by its base 16 multiplied by its "
            "digit position and then dividing the decimal by 2 until quotient "
            "is 0. The remainders from bottom to top is the binary number. Try running "
            "the program again!\n",
            actual);
        //fgetc(stdin);
      }
    }
  } else {
    printf("Convert the hexadecimal %X into %s: ", n, dec);
    scanf("%i", &input);
    fgetc(stdin);
    if (input == n) {
      n = rando(16, 30, 15);
      printf("Correct!\nConvert the hexadecimal %X into %s: ", n, dec);
      scanf("%i", &input);
      fgetc(stdin);
      if (input == n) {
        printf("Correct!\nTry running the program again!\n");
      } else {
        printf(
            "Incorrect.\nThe correct answer is %i. Converting hex into decimal "
            "requires to calculate the sum of multiplying each position of the "
            "hex by its base 16 multiplied by its digit position. Try running the "
            "program again!\n",
            n);
      }
    } else {
      printf(
          "Incorrect.\nThe correct answer is %i. Converting hex into decimal "
          "requires to calculate the sum of multiplying each position of the "
          "hex by its base 16 multiplied by its digit position.\n",
          n);
      n = rando(0, 5, 6);
      printf("Convert the hexadecimal %X into %s: ", n, dec);
      scanf("%i", &input);
      fgetc(stdin);
      if (input == n) {
        printf("Correct!\nTry running the program again!\n");
      } else {
        printf(
            "Incorrect.\nThe correct answer is %i. Converting hex into decimal "
            "requires to calculate the sum of multiplying each position of the "
            "hex by its base 16 multiplied by its digit position. Try running the "
            "program again!\n",
            n);
      }
    }
  }
}
