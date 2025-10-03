#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int solveExpr(const char *expr, int *err);
int solveTerm(const char **expr, int *err);
int solveFactor(const char **expr, int *err);

void skipSpaces(const char **expr)
{
    while (**expr == ' ')
    {
        (*expr)++;
    }
}

int solveFactor(const char **expr, int *err)
{
    skipSpaces(expr);

    if (isdigit(**expr))
    {
        int num = 0;
        while (isdigit(**expr))
        {
            num = num * 10 + (**expr - '0');
            (*expr)++;
        }
        return num;
    }
    else if (**expr == '(')
    {
        (*expr)++;
        int val = solveExpr(*expr, err);
        skipSpaces(expr);
        if (**expr == ')')
        {
            (*expr)++;
        }
        else
        {
            *err = 1;
        }
        return val;
    }
    else
    {
        *err = 1;
        return 0;
    }
}

int solveTerm(const char **expr, int *err)
{
    int result = solveFactor(expr, err);
    skipSpaces(expr);

    while (**expr == '*' || **expr == '/')
    {
        char op = **expr;
        (*expr)++;
        int right = solveFactor(expr, err);

        if (op == '*')
        {
            result *= right;
        }
        else
        {
            if (right == 0)
            {
                *err = 2;
                return 0;
            }
            result /= right;
        }
        skipSpaces(expr);
    }
    return result;
}

int solveExpr(const char *expr, int *err)
{
    int result = solveTerm(&expr, err);
    skipSpaces(&expr);

    while (*expr == '+' || *expr == '-')
    {
        char op = *expr;
        expr++;
        int right = solveTerm(&expr, err);

        if (op == '+')
            result += right;
        else
            result -= right;

        skipSpaces(&expr);
    }
    return result;
}

int main()
{
    char input[100];

    printf("Enter expression: ");
    if (!fgets(input, sizeof(input), stdin))
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    input[strcspn(input, "\n")] = 0;

    int err = 0;
    int ans = solveExpr(input, &err);

    if (err == 1)
    {
        printf("Error: Invalid expression.\n");
    }
    else if (err == 2)
    {
        printf("Error: Division by zero.\n");
    }
    else
    {
        printf("%d\n", ans);
    }
    return 0;
}
