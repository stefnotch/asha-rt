cd external/SimpleBLE/
cmake -H simpleble/ -B build_simpleble
cmake --build build_simpleble
cd ../..
cmake .
make && ./ashart