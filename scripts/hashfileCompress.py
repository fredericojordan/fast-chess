import zlib, base64, sys

if __name__ == "__main__":
    try:
        hashfile = open("hashfile", "r+")
    except:
        print("ERROR: While opening hash file!")
        sys.exit(-1)

    content = hashfile.read()
    content = base64.b64encode(zlib.compress(str.encode(content), 9)).decode("utf-8")

    hashfile.seek(0)
    hashfile.truncate()
    hashfile.write(content)
    hashfile.close()

    print("Done!")
