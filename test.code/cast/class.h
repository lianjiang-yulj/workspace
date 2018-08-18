class AAA {
public:
	virtual void test();
	int nnn;
};

class BBB : public AAA {
public:
	virtual void test();
	virtual void tttest();
};
