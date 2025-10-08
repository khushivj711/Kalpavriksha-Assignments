#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_STACK 1000

void pushNumber(int value);
int popNumber();
void pushOperator(char operator);
char popOperator();
int getPrecedence(char operator);
int performCalculation(int leftOperand, int rightOperand, char operator);
void evaluateExpression();
bool isOperator(char ch);

int numberStack[MAX_STACK];
int numberTop = -1;
char operatorStack[MAX_STACK];
int operatorTop = -1;

int main()
{
  printf("Enter expression: ");
  evaluateExpression();
  return 0;
}

void pushNumber(int value)
{
  numberStack[++numberTop] = value;
}

int popNumber()
{
  return numberStack[numberTop--];
}

void pushOperator(char operator)
{
  operatorStack[++operatorTop] = operator;
}

char popOperator()
{
  return operatorStack[operatorTop--];
}

int getPrecedence(char operator)
{
  if (operator == '*' || operator == '/')
  {
    return 2;
  }
  if (operator == '+' || operator == '-')
  {
    return 1;
  }
  return 0;
}

int performCalculation(int leftOperand, int rightOperand, char operator)
{
  if (operator == '+')
  {
    return leftOperand + rightOperand;
  }
  if (operator == '-')
  {
    return leftOperand - rightOperand;
  }
  if (operator == '*')
  {
    return leftOperand * rightOperand;
  }
  if (operator == '/')
  {
    if (rightOperand == 0)
    {
      printf("Error: Division by zero.\n");
      return 0;
    }
    return leftOperand / rightOperand;
  }
  printf("Error: Invalid expression.\n");
  return 0;
}

bool isOperator(char ch)
{
  return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

void evaluateExpression()
{
  char expression[1000];
  if (!fgets(expression, sizeof(expression), stdin))
  {
    return;
  }
  int index = 0;
  while (expression[index])
  {
    if (isspace(expression[index]))
    {
      index++;
      continue;
    }
    if (isdigit(expression[index]))
    {
      int number = 0;
      while (isdigit(expression[index]))
      {
        number = number * 10 + (expression[index] - '0');
        index++;
      }
      pushNumber(number);
      continue;
    }
    if (isOperator(expression[index]))
    {
      while (operatorTop >= 0 && getPrecedence(operatorStack[operatorTop]) >= getPrecedence(expression[index]))
      {
        if (numberTop < 1)
        {
          printf("Error: Invalid expression.\n");
          return;
        }
        int rightOperand = popNumber();
        int leftOperand = popNumber();
        char operator = popOperator();
        int result = performCalculation(leftOperand, rightOperand, operator);
        pushNumber(result);
      }
      pushOperator(expression[index]);
      index++;
      continue;
    }
    printf("Error: Invalid expression.\n");
    return;
  }
  while (operatorTop >= 0)
  {
    if (numberTop < 1)
    {
      printf("Error: Invalid expression.\n");
      return;
    }
    int rightOperand = popNumber();
    int leftOperand = popNumber();
    char operator = popOperator();
    int result = performCalculation(leftOperand, rightOperand, operator);
    pushNumber(result);
  }
  if (numberTop == 0)
  {
    printf("%d\n", numberStack[numberTop]);
  }
  else
  {
    printf("Error: Invalid expression.\n");
  }
}
