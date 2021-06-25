#include "dynamic_struct.h"
#include <iostream>

using namespace dynamic_struct;

int main() {
    std::string base_type_name;
    std::cin >> base_type_name;
    Struct_Type point({
        Primitive_Type_ptr(get_type_from_string(base_type_name), "x"),
        Primitive_Type_ptr(get_type_from_string(base_type_name), "y")
    }, "point");
    Struct_Type triangle({
        Struct_Clone(point, "point_a"),
        Struct_Clone(point, "point_b"),
        Struct_Clone(point, "point_c")
    }, "triangle");
    triangle.init();
    std::cout << triangle.type() << std::endl;
    std::cout << "Serialize to: " << Serialize(&triangle) << std::endl;
    std::cout << "Deserialize to: " << Deserialize(Serialize(&triangle))->type() << std::endl;
    std::cin >> triangle["point_a"]["x"];
    std::cin >> triangle["point_a"]["y"];

    std::cin >> triangle["point_b"]["x"];
    std::cin >> triangle["point_b"]["y"];

    std::cin >> triangle["point_c"]["x"];
    std::cin >> triangle["point_c"]["y"];

    std::cout << "point_a: x " << triangle["point_a"]["x"] << " y " << triangle["point_a"]["y"] << std::endl;
    std::cout << "point_b: x " << triangle["point_b"]["x"] << " y " << triangle["point_b"]["y"] << std::endl;
    std::cout << "point_c: x " << triangle["point_c"]["x"] << " y " << triangle["point_c"]["y"] << std::endl;
}