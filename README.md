# Tiny C++ Dynamic Type Library

This library implements run-time `dynamic type`, including constructing, loading, saving, and behaves like `reflection`. It is purely relied on `C++11` standard, staying clear of additional libraries (mostly using features of OOP), and it is about `~1000` lines of code, which is the reason why it is tiny, portable and easy to incorporate. Its logic is also relatively simple and straight-forward without many tricks.

I write this library in 2 days out of sudden inspiration and impulse, but with great passion, because of boredom and worriness during exam week. As a result, there might be bugs or many room for optimization (Currently, as far as I know, the compilation time is a bit long). Any Pull Request is welcome!

Author: Jiang Kevin

## Dependencies

I develop this library in the `Windows` platform, but `linux` and `macOS` platforms are not tested yet. I think it should work in both platforms, since I didn't use any platform-specific tricks or libraries.

## Features
- Tiny
- Support Nested `Struct`
- Support `Serialize` and `Deserialize` Type, which means that `Type` could be written to file and recovered from file
- Support Create `Type` dynamically, from `Input`, `File` or in a static way
- Define multiple macros for convenience, avoiding tons of temporary pointers
- Overload operator `>>`, `<<` and `[]` for their intuitive usage
- Throw C++ native Exception for Error Handle

## Example

I write a example code in the [example.cpp](./example.cpp). You can compile and run it.

```shell
$ g++ -std=c++11 ./dynamic_struct.h ./example.cpp
```

## How to use

### 1. Create Multi-dimensional Array based on Input
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    size_t number_of_dimensions = 0;
    Array_Type* array = nullptr;

    std::cin >> number_of_dimensions;
    
    for (size_t index = 0; index < number_of_dimensions; ++index) {
        size_t dimension = 0;
        std::cin >> dimension;
        
        if (index == 0) array = new Array_Type(dimension, Float_32(), "array");
        else {
            Array_Type* sub_array = array;
            array = new Array_Type(dimension, sub_array, "array");
            delete sub_array; // Memory is not allocated 
                              // until calling its `init` method, 
                              // so this process shouldn't waste much resources.
        }
    }
    std::cout << array->type() << std::endl;

    delete array;
}
```

Output:
```shell
$ ./a.exe
3
3 32 64
[64][32][3]Float_32 array
```

### 2. Read and Print Data in Struct
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    Struct_Type sub_struct({
        Int_64("x"),
        Int_64("y")
    }, "sub_struct");
    Struct_Type type({
        Tensor(32, 32, 3, Float_32(), "tensor"),
        &sub_struct
    }, "type");

    type.init(); // Memory is allocated
    
    std::cin >> type["sub_struct"]["x"] >> type["sub_struct"]["y"];
    std::cout << type["sub_struct"]["x"] << " " << type["sub_struct"]["y"];
}
```

Output:
```shell
$ ./a.exe
16 32
16 32
```
### 3. Serialize and Deserialize Type
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    Struct_Type sub_struct({
        Int_8("x"),
        Int_8("y")
    }, "sub_struct");
    Struct_Type type({
        Array(8, Float_32(), "var"),
        &sub_struct
    }, "type");
    
    std::cout << Serialize(&type) << std::endl;
    std::cout << Deserialize(Serialize(&type))->type() << std::endl;
}
```

Output:
```shell
$ ./a.exe
{type,[var,8,(,Float_32)],{sub_struct,(x,Int_8),(y,Int_8)}}
type {
    [8]Float_32 var,
    sub_struct {
        Int_8 x,
        Int_8 y
    }
}
```

## TODO
- [ ] Make `set_name` into a *safe* function. Currently, changing the name of variable in `Struct` will not result in the subsequent change of cached list of variable names in `Struct`, which is definitely a bug.

## License

This project is open source and freely available under the [MIT](./LICENSE) license.