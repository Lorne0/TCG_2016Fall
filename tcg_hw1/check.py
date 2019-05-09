import numpy as np

def fac(k):
    r=1
    for i in range(1,k+1):
        r=r*i
    return r

def H(n,r):
    return fac(r+n-1)/(fac(r)*fac(n-1))

dsize=5

cnt=0
ans=np.zeros(dsize)


with open("tcga2016-question.txt") as fp:
    for f in fp:
        if cnt==0:
            cnt+=1
            continue
        if cnt==(dsize+1):
            break
        else:
            f=f.strip('\n').strip('\r')
            a=f.split('\t')
            a=np.array(map(int,a))
            ans[cnt-1] = H(len(a)+1,dsize-(sum(a)+len(a)-1))
            cnt+=1

print ans

r=1
for i in range(dsize):
    r=r*ans[i]
print r

print sum(ans)

