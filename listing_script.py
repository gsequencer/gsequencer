import sys

f_out = open(sys.argv[2], "w+")
f_in = open(sys.argv[1])

lines = f_in.readlines()
f_in.close();

i = 0

for line in lines:
    if i >= 10:    
        f_out.write(line)
    i += 1

f_out.close()
