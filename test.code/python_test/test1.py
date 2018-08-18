def f(n):
    if n <= 1:
        print "f=",n
        return n
    else:
        m = n * f(n - 1)
        print "f=",m
        return m
def buildConnectionString(params):
    """Build a connection string from a dictionary of parameters.
    Returns string."""
    print params;
    for k,v in params.items():
        print k,v
        print k+v
    for k in params:
        print k,params[k]
#    return ";".join(["%s=%s" % (k, v) for k, v in params.items()])
    return ["%s=%s" % (k, v) for k, v in params.items()]
#    for k,v in params.items()
#        print "%s=%s" % (k, v)

if __name__ == "__main__":
    myParams = {"server":"mpilgrim", \
            "database":"master", \
            "uid":"sa", \
            "pwd":"secret" \
            }
    print buildConnectionString(myParams)
    myParams.clear()
    print buildConnectionString(myParams)
    print buildConnectionString.__doc__
    print str(f(5));
    print str('22');
