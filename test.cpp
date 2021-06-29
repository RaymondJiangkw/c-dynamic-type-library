#include "dynamic_struct.h"

using namespace dynamic_struct;

void test_1() {
    std::string data_type;
    std::cin >> data_type;
    Primitive_Type type(get_type_from_string(data_type), "type");
    type.init(); // Allocate Memory
    std::cin >> type;
    std::cout << type;
}

void test_2() {
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

void test_3() {
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

void test_4() {
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

void test_5() {
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

int main() {
    test_1();
}