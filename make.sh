#!/bin/bash
mkdir -p bin
rm -f simple-chat-client simple-chat-server
cd bin
rm -f simple-chat-client simple-chat-server
cmake .. -S .. -DCMAKE_BUILD_TYPE=Debug
make -j5
cd ..
ln -s $(pwd)/bin/simple-chat-client ./simple-chat-client
ln -s $(pwd)/bin/simple-chat-server ./simple-chat-server
