#include "dynamic_struct.h"

using namespace dynamic_struct;

int test_1() {
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
            delete sub_array; // Memory is not allocated until calling its `init` method, so this process shouldn't waste much resources
        }
    }
    std::cout << array->type() << std::endl;

    delete array;
}

int test_2() {
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