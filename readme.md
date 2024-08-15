```bash
mkdir build
cd ./build
cmake ..
make format
make cpplint
make clang-tidy
make server
make client_threadpool
cd ./bin
./server 9527
./client_threadpool 9527
```
