import zlib, base64, sys

if __name__ == '__main__':
    try:
        hashfile = open('hashfile', 'r+')
    except:
        print('ERROR: While opening hash file!')
        sys.exit(-1)
    content = {}
    
    line_number = 0
    for line in hashfile.readlines():
        line_number += 1
        l = line.strip().split()
        
        if len(l) < 7:
            print('Bad entry on line ' + str(line_number) + ' (ignored): ' + line.strip())
            continue
        
        if l[0] in content:
            old = content[l[0]].split()
            if l[3:] == old[3:]:
                if l[1] > old[1]:
                    content[l[0]] = line
            else:
                print('Colision!')
                print(content[l[0]].strip())
                print(line.strip())
                sys.exit(-1)
        else:
            content[l[0]] = line
    
    hashfile.seek(0)
    hashfile.truncate()
      
    for entry in sorted(content.items(), key=lambda x: x[0]):
        e = entry[1].split()
        sanitized_str = '{} {} {} {} {} {} {}\n'.format(e[0], e[1], e[2], e[3], e[4], e[5], e[6]) 
        hashfile.write(sanitized_str)
     
    hashfile.close()
    print('Done!')