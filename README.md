# Tiny C++ Dynamic Type Library

This library implements run-time `dynamic type`, including constructing, loading, saving, and behaves like `reflection`. It is purely relied on `C++11` standard, staying clear of additional libraries (mostly using features of OOP), and it is about `~1000` lines of code, which is the reason why it is tiny, portable and easy to incorporate. Its logic is also relatively simple and straight-forward without many tricks.

I write this library in 2 days out of sudden inspiration and impulse, but with great passion, because of boredom and worriness during exam week. As a result, there might be bugs or many room for optimization (Currently, as far as I know, the compilation time is a bit long). Any Pull Request is welcome!

*Author*: Jiang Kevin

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
- Supported Primitive DataTypes (`Array` and `Struct` are based on them) include:
    - Int_8
    - Int_16
    - Int_32
    - Int_64
    - Unsigned_Int_8
    - Unsigned_Int_16
    - Unsigned_Int_32
    - Unsigned_Int_64
    - Char
    - Boolean
    - Float_32
    - Float_64

## Example

I write a example code in the [example.cpp](./example.cpp). You can compile and run it.

```shell
$ g++ -std=c++11 ./dynamic_struct.h ./example.cpp
```

## How to use

### 1. Read Data Type and Input Value
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    std::string data_type;
    std::cin >> data_type;
    Primitive_Type type(get_type_from_string(data_type), "type");
    type.init(); // Allocate Memory
    std::cin >> type;
    std::cout << type;
}
```

Output:
```shell
$ ./a.exe
Float_32
1.5
1
$ ./a.exe
Int_8
1.5
1
$ ./a.exe
Boolean
1.5
terminate called after throwing an instance of 'std::invalid_argument'
  what():  Value Error: Cannot set 1.5 into Boolean type 'type'
```

### 2. Create Struct with variable Properties
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    Struct_Type type({}, "type");
    size_t number_of_properties = 0;
    std::cin >> number_of_properties;
    std::string data_type, data_name;

    for (size_t index = 0; index < number_of_properties; index++) {
        std::cin >> data_type >> data_name;
        type.append(Primitive_Type_ptr(get_type_from_string(data_type), data_name));
    }

    type.init(); // After `init`, no further property could be appended.

    for (auto property_name : type.get_Keys()) {
        std::cin >> type[property_name];
    }

    for (auto property_name : type.get_Keys()) {
        std::cout << property_name << ": " << type[property_name] << std::endl;
    }
}
```

Output:
```shell
$ ./a.exe
5
Int_64 x
Int_64 y
Char c
Float_32 f
Boolean b
127
-9999
c
-0.0001  
false
x: 127      
y: -9999    
c: c        
f: -0.000100
b: false
```

### 3. Create Multi-dimensional Array, whose Base Type is based on Input
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    std::string data_type;
    std::cin >> data_type;

    size_t number_of_dimensions = 0;
    Array_Type* array = nullptr;

    std::cin >> number_of_dimensions; // Notice that: The order of dimensions is opposite to the order of input.
    
    for (size_t index = 0; index < number_of_dimensions; ++index) {
        size_t dimension = 0;
        std::cin >> dimension;
        
        if (index == 0) array = new Array_Type(dimension, Primitive_Type_ptr(get_type_from_string(data_type), ""), "array");
        else {
            Array_Type* sub_array = array;
            array = new Array_Type(dimension, sub_array, "array");
            delete sub_array; // Memory is not allocated until calling its `init` method, so this process shouldn't waste much resources
        }
    }
    std::cout << array->type() << std::endl;

    delete array;
}
```

Output:
```shell
$ ./a.exe
Float_32
3
2 3 4
[4][3][2]Float_32 array
$ ./a.exe
Boolean
4
2 3 4 5
[5][4][3][2]Boolean array
```

### 4. Change name of Variable in Struct
```c++
#include "dynamic_struct.h"

using namespace dynamic_struct;

int main() {
    Struct_Type sub_struct({
        Int_64("tmp_name_1"),
        Int_64("tmp_name_2")
    }, "sub_struct");
    Struct_Type type({
        Tensor(32, 32, 3, Float_32(), "tensor"),
        &sub_struct
    }, "type");

    type.init(); // Memory is allocated
    std::string new_name_1, new_name_2;
    std::cin >> new_name_1 >> new_name_2;

    type["sub_struct"]["tmp_name_1"].set_name(new_name_1);
    type["sub_struct"]["tmp_name_2"].set_name(new_name_2);
    
    std::cin >> type["sub_struct"][new_name_1] >> type["sub_struct"][new_name_2];

    for (auto key : type["sub_struct"].get_Keys()) std::cout << key << " " << type["sub_struct"][key] << std::endl;
}
```

Output:
```shell
$ ./a.exe
x y 
-128 127
x -128
y 127
```
### 5. Serialize and Deserialize Type
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
- [ ] Reorganize Error Handle to clean up redundant code.
- [ ] Support Function as Primitive Data Type, perhaps?

## License

This project is open source and freely available under the [MIT](./LICENSE) license.