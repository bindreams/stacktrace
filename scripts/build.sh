rm -rf dist && 
cmake --build "build" --config $1 &&
cmake --install "build" --prefix dist --config $1