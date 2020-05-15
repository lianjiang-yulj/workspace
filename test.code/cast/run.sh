g++ -c class.cpp -fPIC
ar r libclass.a class.o 

echo "[main with -Wl,-E]"
g++ main.cpp libclass.a -ldl -Wl,-E
echo " => lib without link"
g++ test.cpp -o libtest.so -shared -fPIC
./a.out
echo " => lib with static link"
g++ test.cpp libclass.a -o libtest.so -shared -fPIC
./a.out
echo " => lib with static link + -Bsymbolic"
g++ test.cpp libclass.a -o libtest.so -shared -fPIC -Wl,-Bsymbolic
./a.out
echo

echo "[main without -Wl,-E]"
g++ main.cpp libclass.a -ldl 
echo " => lib without link"
g++ test.cpp -o libtest.so -shared -fPIC
./a.out
echo " => lib with static link"
g++ test.cpp libclass.a -o libtest.so -shared -fPIC
./a.out
echo " => lib with static link + -Bsymbolic"
g++ test.cpp libclass.a -o libtest.so -shared -fPIC -Wl,-Bsymbolic
./a.out
