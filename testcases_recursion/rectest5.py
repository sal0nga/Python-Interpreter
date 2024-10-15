# Calculate the result of a number raised to a power
def power(base, exponent):
    if exponent == 0:
        return 1
    else:
        return base * power(base, exponent - 1)

base = 2
exponent = 3
result = power(base, exponent)
print("2 raised to the power of 3 is", result)
