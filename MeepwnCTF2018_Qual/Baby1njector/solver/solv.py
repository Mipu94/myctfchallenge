import rc4

content=""
with open('flag', 'rb') as content_file:
    content = content_file.read(72)

def genP():
    key = "MeePwnCTF{XXXXXXXXXXXXXXXXXXXXXXXXX}"
    p1 = [0] * 36
    p2 = [0] * 36
    POS = [53,46,55,59,64,22,30,4,20,15,31,41,51,68,27,14,34,66,21,48,33,70,3,17,35,25,12,49,44,63,56,60,1,11,19,10,50,62,57,52,36,8,42,2,6,26,45,40,71,69,47,61,24,9,39,32,65,5,0,58,13,28,23,67,18,54,29,16,43,38,7,37]
    for i in range(72):
        k = POS[i]/2
        if(i < 36):
            if( POS[i] % 2 == 0 ):
                p1[i%36] = 0xf & (ord(key[k]))
            else:
                p1[i%36] = (ord(key[k])) >> 4
        else:
            if( POS[i] % 2 == 0 ):
                p2[i%36] = 0xf & (ord(key[k]))
            else:
                p2[i%36] = (ord(key[k])) >> 4

    return p1, p2
    return "".join([chr(i) for i in p1]), "".join([chr(i) for i in p2])
    
def degenP(p1, p2):
    POS = [53,46,55,59,64,22,30,4,20,15,31,41,51,68,27,14,34,66,21,48,33,70,3,17,35,25,12,49,44,63,56,60,1,11,19,10,50,62,57,52,36,8,42,2,6,26,45,40,71,69,47,61,24,9,39,32,65,5,0,58,13,28,23,67,18,54,29,16,43,38,7,37]
    key = [0]*36
    
    for i in range(72):
        k = POS[i]/2
        
        if i < 36:
            if POS[i] % 2 == 0:
                key[k] = (key[k]) | p1[i % 36]
            else:
                key[k] = (key[k]) | p1[i % 36] << 4
        else:
            if POS[i] % 2 == 0:
                key[k] = (key[k]) | p2[i % 36]
            else:
                key[k] = (key[k]) | p2[i % 36] << 4
                
    flag = "".join(chr(i & 0xff) for i in key)
    return flag
        
def brute():
    p1, p2 = genP()

    print p1.encode("hex")
    print p2.encode("hex")

    c1 = "".join([chr(i) for i in C1]).encode("hex")
    c2 = "".join([chr(i) for i in C2]).encode("hex")

    for i in range(0xffff):
        key = str(i)
        key = [ord(char) for char in key]
        k = rc4.initialize(key)
        encoded = rc4.run_rc4(list(k), p1).encode("hex")
        if encoded == c1:
            print "PID1", hex(i)
            break
            
    for i in range(0xffff):
        key = str(i)
        key = [ord(char) for char in key]
        k = rc4.initialize(key)
        encoded = rc4.run_rc4(list(k), p2).encode("hex")
        if encoded == c2:
            print "PID2", hex(i)
            break
    
def trans(key, text):
    key = str(key)
    key = [ord(char) for char in key]
    k = rc4.initialize(key)
    
    des = rc4.run_rc4(list(k), text)
    des = [ord(i) for i in des]
    
    return des
    
def solv():
    c1 = content[0:36]
    c2 = content[36:72]
    print c1.encode("hex") , len(c1)
    print c2.encode("hex"), len(c2)

    POS = [53,46,55,59,64,22,30,4,20,15,31,41,51,68,27,14,34,66,21,48,33,70,3,17,35,25,12,49,44,63,56,60,1,11,19,10,50,62,57,52,36,8,42,2,6,26,45,40,71,69,47,61,24,9,39,32,65,5,0,58,13,28,23,67,18,54,29,16,43,38,7,37]
    
    org_p1 = [0xfff]*36
    org_p2 = [0xfff]*36
    
    p1, p2 = genP()
    for i in range(72):
        k = POS[i] / 2
        
        if k > 9:
            continue
         
        if i < 36:
            if POS[i] % 2 != 0:
                org_p1[i%36] = p1[i%36]
        else:
            if POS[i] % 2 != 0:
                org_p2[i%36] = p2[i%36]
   
    p1_list = []
    for pid1 in range(0xffff):
        p1 = trans(pid1, c1)
        correct = True
        for i in range(len(p1)):
            if org_p1[i] != 0xfff and p1[i] != org_p1[i]:
                correct = False
                break
        if correct:
            print pid1
            p1_list.append(p1)
            
    print p1_list
    
    p2_list = []
    for pid2 in range(0xffff):
        p2 = trans(pid2, c2)
        correct = True
        for i in range(len(p2)):
            if org_p2[i] != 0xfff and p2[i] != org_p2[i]:
                correct = False
                break
        if correct:
            print pid2
            p2_list.append(p2)
            
    print p2_list
            
    for p1 in p1_list:
        for p2 in p2_list:
            print degenP(p1, p2)
    

solv()
# print genP()
    


