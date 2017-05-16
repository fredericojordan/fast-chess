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
        entry = line.strip().split()
        
        if len(entry) < 7:
            print('Bad entry on line ' + str(line_number) + ' (ignored): ' + line.strip())
            continue
        
        hash = entry[0]
        depth = int(entry[1])
        score = int(entry[2])
        fen = ' '.join(entry[3:])
        
        if hash in content:
            old_entry = content[hash].split()
            old_depth = int(old_entry[1]) 
            old_fen = ' '.join(old_entry[3:]) 
            
            if fen == old_fen:
                if depth > old_depth:
                    content[hash] = line
            else:
                print('Colision!')
                print(content[hash].strip())
                print(line.strip())
                sys.exit(-1)
        else:
            content[hash] = line
    
    hashfile.seek(0)
    hashfile.truncate()
      
    for entry in sorted(content.items(), key=lambda x: x[0]):
        field = entry[1].split()
        sanitized_str = '{} {} {} {} {} {} {}\n'.format(field[0], field[1], field[2], field[3], field[4], field[5], field[6]) 
        hashfile.write(sanitized_str)
     
    hashfile.close()
    print('Done!')