fh = open('registers', 'r')
raw = fh.read()
fh.close()

registers = []

lines = raw.strip().split('\n')
for line in lines :
    pieces = line.split('\t')
    for piece in pieces :
        if len(piece) < 2 :
            continue
        if piece[-1] == ',' :
            registers.append(piece.strip()[:-1])

for register in registers :
    print '"' + register + '",'
