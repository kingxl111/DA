import random
import string

alf = list(string.ascii_lowercase)
for _ in range(1000000):
    print('a', end='')
for _ in range(100000):
    print(random.choice(alf), end='')
print()
