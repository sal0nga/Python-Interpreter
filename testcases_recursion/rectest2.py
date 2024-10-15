# Calculate the sum of natural numbers up to n (inclusive)
def sum_natural(n):
    if n <= 1:
        return n
    else:
        return n + sum_natural(n - 1)

num = 10
result = sum_natural(num)
print("sum_natural(10) =", result)
