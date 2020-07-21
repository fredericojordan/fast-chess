import zlib, base64, sys


def hash(input):
    hash = 5381

    for c in input:
        hash = ((hash << 5) + hash) + ord(c)
        hash &= 0xFFFFFFFF

    return format(hash, "08x")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Please type input and press [ENTER].")
        print("Hash: " + hash(input()))
    else:
        print("Hash: " + hash(" ".join(sys.argv[1:])))
