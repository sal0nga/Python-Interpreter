# Calculate the greatest common divisor of a and b
def gcd(a, b):
    if b == 0:
        return a
    else:
        return gcd(b, a % b)

a = 48
b = 18
result = gcd(a, b)
print("gcd(48, 18) =", result)
