import sys

with open(sys.argv[2], "w+") as f_out, open(sys.argv[1]) as f_in:
    lines = f_in.readlines()
    
    i = 0
    
    for line in lines:
        if i >= 10:    
            f_out.write(line)
        i += 1
