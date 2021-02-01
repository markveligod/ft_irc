make -C ../;
qmake;
make;
./test_gui 1080 123;
make distclean