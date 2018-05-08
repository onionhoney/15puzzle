em++ -O3 *.cpp -std=c++11 --post-js post.js -o solver.js -s TOTAL_MEMORY=1073741824\
    -s EXPORTED_FUNCTIONS="['_solve']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'

cp ./solver.js.mem ../../
