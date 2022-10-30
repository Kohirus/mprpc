# mprpc

A high-performance distributed network framework, it uses `muduo` to handle network IO
, and uses `protubuf` to do the serialization and deserialization.

# How to use

You just need to type the following command:

```sh
./autobuild.sh
```

Then the library file will then be generated in the `lib` directory.

Alternatively, you can create the `build` directory and enter, then type the following 
command to manually build the project:

```sh
cmake ..
make
```
