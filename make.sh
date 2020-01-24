#!/bin/bash
mkdir -p bin
rm -f simple-chat-client simple-chat-server simple-chat-server-cpp
cd bin
rm -f simple-chat-client simple-chat-server simple-chat-server-cpp
cmake .. -S .. -DCMAKE_BUILD_TYPE=Debug
make -j5
cd ..
ln -s $(pwd)/bin/simple-chat-client ./simple-chat-client
ln -s $(pwd)/bin/simple-chat-server ./simple-chat-server
ln -s $(pwd)/bin/simple-chat-server-cpp ./simple-chat-server-cpp
