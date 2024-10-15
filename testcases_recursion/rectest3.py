# Calculate the nth Fibonacci number
def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n - 1) + fibonacci(n - 2)

position = 7
fib_number = fibonacci(position)
print("fibonacci(7) =", fib_number)
