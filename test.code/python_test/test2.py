import test1, sys
sys.path.append("/home/yulj/tools/tmptest/python_test/")
params={1:2,3:4}
print test1.buildConnectionString(params)
print test1.buildConnectionString.__doc__
print sys.path
print __name__
print test1.__name__

list=[1,2,3,4]
#for (i=0;i<10;i++)
#    list.append(i)
print list
for k in params.keys():
    print k
for k in params:
    print k
for k,v in params.items():
    print k
for i in list:
    print i
    
