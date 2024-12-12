import random
import string

with open("1.in", 'w') as f1:
    alf = list(string.ascii_lowercase)
    for _ in range(1000100):
        f1.write('a')
    for _ in range(100000):
        f1.write(random.choice(alf))
    print("test 1.in added")

with open("2.in", 'w') as f2:
    cyrillic = [chr(i) for i in range(ord('а'), ord('я') + 1)]
    for _ in range(100000):
        f2.write(random.choice(cyrillic))
    print("test 2.in added")

with open("3.in", 'w') as f3:
    cyrillic = [chr(i) for i in range(ord('а'), ord('я') + 1)]
    for _ in range(1000000):
        f3.write(random.choice(cyrillic))
    print("test 3.in added")

with open("4.in", 'w') as f4:
    waves = "/la-0123🌊🌊🌊🌊Еще одна волна🌊🌊Волна🌊🌊0-a.zxf13GMРусская последовательностьAAke3Masd,🌊🌊🌊dk🌊"
    f4.write(waves)
    print("test 4.in added")
