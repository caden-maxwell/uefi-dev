int add(int a, int b)
{
    return a + b;
}

int factorial(int n)
{
    if (n > 15)
        return 0;
    if (n == 1)
        return 1;

    return n * factorial(n - 1);
}
