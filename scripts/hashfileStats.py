import zlib, base64, sys

MAX_DEPTH = 50

if __name__ == "__main__":
    try:
        hashfile = open("hashfile", "r")
    except:
        print("ERROR: While opening hash file!")
        sys.exit(-1)

    line_number = 0
    depths = [0 for _ in range(MAX_DEPTH)]

    for line in hashfile.readlines():
        line_number += 1
        l = line.strip().split()

        if len(l) < 7:
            print(
                "Bad entry on line " + str(line_number) + " (ignored): " + line.strip()
            )
            continue

        hash = l[0]
        depth = int(l[1])
        score = int(l[2])
        fen = " ".join(l[3:])

        depths[depth] += 1

    hashfile.close()
    print("-- Depths --")
    for i in range(MAX_DEPTH):
        if not depths[i]:
            continue
        print("{:2d}: {:8d}".format(i, depths[i]))
    print("------------")
