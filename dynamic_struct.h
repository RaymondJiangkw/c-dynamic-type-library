/**
 * Copyright 2021 Jiang Kevin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef DYNAMIC_STRUCT_H
#define DYNAMIC_STRUCT_H

#include <initializer_list>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <memory>
#include <map>
#include <stack>
#include <functional>
#include <algorithm>

namespace dynamic_struct {
    enum class Primitive_Data_Types {
        Int_8,
        Int_16,
        Int_32,
        Int_64,

        Unsigned_Int_8,
        Unsigned_Int_16,
        Unsigned_Int_32,
        Unsigned_Int_64,

        Char,

        Boolean,

        Float_32,
        Float_64
    };

    inline Primitive_Data_Types get_type_from_string(std::string type_name) {
        if (type_name == "Int_8") return Primitive_Data_Types::Int_8;
        else if (type_name == "Int_16") return Primitive_Data_Types::Int_16;
        else if (type_name == "Int_32") return Primitive_Data_Types::Int_32;
        else if (type_name == "Int_64") return Primitive_Data_Types::Int_64;
        else if (type_name == "Unsigned_Int_8") return Primitive_Data_Types::Unsigned_Int_8;
        else if (type_name == "Unsigned_Int_16") return Primitive_Data_Types::Unsigned_Int_16;
        else if (type_name == "Unsigned_Int_32") return Primitive_Data_Types::Unsigned_Int_32;
        else if (type_name == "Unsigned_Int_64") return Primitive_Data_Types::Unsigned_Int_64;
        else if (type_name == "Char") return Primitive_Data_Types::Char;
        else if (type_name == "Boolean") return Primitive_Data_Types::Boolean;
        else if (type_name == "Float_32") return Primitive_Data_Types::Float_32;
        else if (type_name == "Float_64") return Primitive_Data_Types::Float_64;
        else {
            throw std::invalid_argument(("Cannot parse type name '" + type_name + "'").c_str());
        }
    }

    inline std::string get_string_from_type(Primitive_Data_Types type) {
        if (type == Primitive_Data_Types::Int_8) return "Int_8";
        else if (type == Primitive_Data_Types::Int_16) return "Int_16";
        else if (type == Primitive_Data_Types::Int_32) return "Int_32";
        else if (type == Primitive_Data_Types::Int_64) return "Int_64";
        else if (type == Primitive_Data_Types::Unsigned_Int_8) return "Unsigned_Int_8";
        else if (type == Primitive_Data_Types::Unsigned_Int_16) return "Unsigned_Int_16";
        else if (type == Primitive_Data_Types::Unsigned_Int_32) return "Unsigned_Int_32";
        else if (type == Primitive_Data_Types::Unsigned_Int_64) return "Unsigned_Int_64";
        else if (type == Primitive_Data_Types::Char) return "Char";
        else if (type == Primitive_Data_Types::Boolean) return "Boolean";
        else if (type == Primitive_Data_Types::Float_32) return "Float_32";
        else if (type == Primitive_Data_Types::Float_64) return "Float_64";
        else {
            throw std::invalid_argument("Cannot parse type");
        }
    }

    const std::string FORBIDDEN_VARIABLE_NAME_CHARS = "()[]{},";

    /**
     * Generally Supported Data Type
     */
    enum class Type_Class {
        Primitive,
        Array,
        Struct
    };

    /**
     * Class representation for `Type`
     * Specifically, `Type` could be `Primitive Data Types`, `Array of Any Type`, `Struct of Stacked Types`
     */
    class Type {
    private:
        friend class Primitive_Type;
        friend class Array_Type;
        friend class Struct_Type;
    protected:
        Type_Class type_class;
        /**
         * Pointer to data chunk
         */
        void* data;
        bool hold_or_possess; // true - hold | false - possess
        std::string name;
        Type* parent_type;
        /**
         * Check for legitimacy of variable name
         */
        bool check_name(std::string name) {
            return name.find_first_of(FORBIDDEN_VARIABLE_NAME_CHARS) == std::string::npos;
        }
        virtual std::string _type(std::string prefix, bool with_name) const = 0;
        /**
         * For Struct Type
         */
        virtual void change_key(std::string target, std::string origin) = 0;

        Type(Type_Class type, std::string _name):type_class(type), name(""), data(nullptr), hold_or_possess(true), parent_type(nullptr) {
            if (!check_name(_name)) throw std::invalid_argument(("Value Error: Cannot assign name '" + _name + "' to type").c_str());
            else name = _name;
        }
    public:
        std::string type() const {
            return _type("", true);
        }
        virtual Type* clone() const = 0;
        /* Return the Bytes of `Type` */
        virtual size_t size_of() const = 0;
        Type_Class get_Type_Class() const {
            return type_class;
        }
        /**
         * For Struct Type
         */
        virtual Type& operator[](std::string key) = 0;
        Type& get(std::string key) {
            return (*this)[key];
        }
        /* Get names of components of Struct */
        virtual std::vector<std::string> get_Keys() const = 0;
        virtual Type& append(Type* type) = 0;
        /**
         * For Array Type
         */
        virtual std::unique_ptr<Type> operator[](size_t pos) = 0;
        std::unique_ptr<Type> at(size_t pos) {
            return (*this)[pos];
        }
        virtual Type& get_Element_Type() = 0;
        /* Get Size of Array */
        virtual size_t get_Size() const = 0;
        /**
         * For Primitive Type
         */
        /* Get Raw Primitive Type */
        virtual Primitive_Data_Types get_Type() const = 0;
        
        /* `init` malloc a piece of memory to hold data, which will be deleted automatically if not used. */
        virtual void init() {
            release();
            data = malloc(size_of());
            if (data == nullptr) {
                throw std::overflow_error(("Memory Error: Fail to allocate memory for type '" + name + "'").c_str());
            }
            memset(data, 0, size_of());
            hold_or_possess = false;
        }
        /* `hold` will pass a pointer of data, which will not be deleted automatically if this object is not used. */
        virtual void hold(void* _data) {
            release();
            data = _data;
            hold_or_possess = true;
        }
        virtual void release() {
            if (data != nullptr && hold_or_possess == false) free(data);
            data = nullptr;
        }
        void* get_data() { return data; }
        ~Type() {
            release();
        }
        
        /* Notice that this function does not have type checking */
        virtual void set(void* src) = 0;
        virtual void set(int8_t src) = 0;
        virtual void set(int16_t src) = 0;
        virtual void set(int32_t src) = 0;
        virtual void set(int64_t src) = 0;

        virtual void set(uint8_t src) = 0;
        virtual void set(uint16_t src) = 0;
        virtual void set(uint32_t src) = 0;
        virtual void set(uint64_t src) = 0;

        virtual void set(char src) = 0;

        virtual void set(float src) = 0;
        virtual void set(double src) = 0;

        virtual void set(bool src) = 0;

        /* Useful Function, which will parse `src` automatically into appropriate internal data type */
        virtual void set(std::string src) = 0;

        virtual int8_t* get_Int_8() const = 0;
        virtual int16_t* get_Int_16() const = 0;
        virtual int32_t* get_Int_32() const = 0;
        virtual int64_t* get_Int_64() const = 0;

        virtual uint8_t* get_Unsigned_Int_8() const = 0;
        virtual uint16_t* get_Unsigned_Int_16() const = 0;
        virtual uint32_t* get_Unsigned_Int_32() const = 0;
        virtual uint64_t* get_Unsigned_Int_64() const = 0;

        virtual char* get_Char() const = 0;

        virtual float* get_Float_32() const = 0;
        virtual double* get_Float_64() const = 0;

        virtual bool* get_Boolean() const = 0;

        Type* set_name(std::string _name) {
            if (!check_name(_name)) throw std::invalid_argument(("Value Error: Cannot assign name '" + _name + "' to type").c_str());
            // Change Upper side Type
            if (parent_type != nullptr && parent_type->type_class == Type_Class::Struct) {
                parent_type->change_key(_name, name);
            }
            name = _name;
            return this;
        }
        std::string get_name() const {
            return this->name;
        }

        virtual std::string string() const = 0;

        friend std::ostream& operator<< (std::ostream& stream, const Type& operand) {
            stream << operand.string();
            return stream;
        }

        friend std::istream& operator>> (std::istream& stream, Type& operand) {
            std::string str;
            stream >> str;
            operand.set(str);
            return stream;
        }
    };

    #define GET_POINTER_MACRO(pointer_type, type_name) if (this->data == nullptr) {\
                throw std::invalid_argument(("Nullpointer Error: Cannot turn null pointer into " + std::string(#type_name) + " pointer of type '" + name + "'").c_str());\
            }\
            if (this->primitive_data_type != Primitive_Data_Types::type_name) {\
                throw std::invalid_argument(("Value Error: Cannot get " + std::string(#type_name) + " pointer from type '" + name + "', whose data type is " + type()).c_str());\
            }\
            return static_cast<pointer_type*>(data);
    
    #define SET_MACRO(type_name) if (this->primitive_data_type != Primitive_Data_Types::type_name) {\
                throw std::invalid_argument(("Value Error: Cannot set " + std::string(#type_name) + " to type '" + name + "', whose data type is " + type()).c_str());\
            }\
            set(static_cast<void*>(&src));

    class Primitive_Type: public Type {
    private:
        Primitive_Data_Types primitive_data_type;
    protected:
        virtual std::string _type(std::string prefix, bool with_name) const {
            std::string suffix = with_name == true ? " " + name : "";
            return prefix + get_string_from_type(primitive_data_type) + suffix;
        }
        virtual void change_key(std::string target, std::string origin) {
            throw std::invalid_argument(("Compile Error: Cannot change key of a Primitive Type '" + name + "'").c_str());
        }
    public:
        Primitive_Type(Primitive_Data_Types type, std::string name):primitive_data_type(type), Type(Type_Class::Primitive, name) {}
        virtual Primitive_Type* clone() const {
            return new Primitive_Type(primitive_data_type, name);
        }
        virtual size_t size_of() const {
            switch (primitive_data_type) {
            case Primitive_Data_Types::Int_8: return sizeof(int8_t);
            case Primitive_Data_Types::Int_16: return sizeof(int16_t);
            case Primitive_Data_Types::Int_32: return sizeof(int32_t);
            case Primitive_Data_Types::Int_64: return sizeof(int64_t);

            case Primitive_Data_Types::Unsigned_Int_8: return sizeof(uint8_t);
            case Primitive_Data_Types::Unsigned_Int_16: return sizeof(uint16_t);
            case Primitive_Data_Types::Unsigned_Int_32: return sizeof(uint32_t);
            case Primitive_Data_Types::Unsigned_Int_64: return sizeof(uint64_t);

            case Primitive_Data_Types::Char: return sizeof(char);

            case Primitive_Data_Types::Boolean: return sizeof(bool);

            case Primitive_Data_Types::Float_32: return sizeof(float);
            case Primitive_Data_Types::Float_64: return sizeof(double);
            }
        }
        virtual Type& operator[](std::string key) {
            throw std::invalid_argument(("Compile Error: Primitive_Type '" + name + "' cannot be indexed with `key`").c_str());
        }
        virtual std::unique_ptr<Type> operator[](size_t pos) {
            throw std::invalid_argument(("Compile Error: Primitive_Type '" + name + "'cannot be indexed with `pos`").c_str());
        }
        virtual size_t get_Size() const {
            throw std::invalid_argument(("Compile Error: Cannot get size of Primitive Type '" + name + "'").c_str());
        }
        virtual Type& get_Element_Type() {
            throw std::invalid_argument(("Compile Error: Cannot get Element Type of Primitive Type '" + name + "'").c_str());
        }
        virtual std::vector<std::string> get_Keys() const {
            throw std::invalid_argument(("Compile Error: Cannot get keys of Primitive Type '" + name + "'").c_str());
        }
        virtual Type& append(Type* type) {
            throw std::invalid_argument(("Compile Error: Cannot append to Primitive Type '" + name + "'").c_str());
        }
        virtual void set(void* src) {
            if (this->data == nullptr) {
                throw std::invalid_argument(("Nullpointer Error: Cannot set to null pointer of type '" + name + "'").c_str());
            }
            memcpy(data, src, size_of());
        }
        virtual void set(int8_t src) {
            SET_MACRO(Int_8)
        }
        virtual void set(int16_t src) {
            SET_MACRO(Int_16)
        }
        virtual void set(int32_t src) {
            SET_MACRO(Int_32)
        }
        virtual void set(int64_t src) {
            SET_MACRO(Int_64)
        }

        virtual void set(uint8_t src) {
            SET_MACRO(Unsigned_Int_8)
        }
        virtual void set(uint16_t src) {
            SET_MACRO(Unsigned_Int_16)
        }
        virtual void set(uint32_t src) {
            SET_MACRO(Unsigned_Int_32)
        }
        virtual void set(uint64_t src) {
            SET_MACRO(Unsigned_Int_64)
        }

        virtual void set(char src) {
            SET_MACRO(Char)
        }

        virtual void set(float src) {
            SET_MACRO(Float_32)
        }
        virtual void set(double src) {
            SET_MACRO(Float_64)
        }

        virtual void set(bool src) {
            SET_MACRO(Boolean)
        }

        virtual void set(std::string src) {
            switch (primitive_data_type) {
            case Primitive_Data_Types::Int_8: {
                int src_data = std::stoi(src);
                set(static_cast<int8_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Int_16: {
                int src_data = std::stoi(src);
                set(static_cast<int16_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Int_32: {
                int src_data = std::stoi(src);
                set(static_cast<int32_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Int_64: {
                int64_t src_data = std::stoll(src);
                set(src_data);
                break;
            }
            case Primitive_Data_Types::Unsigned_Int_8: {
                unsigned long src_data = std::stoul(src);
                set(static_cast<uint8_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Unsigned_Int_16: {
                unsigned long src_data = std::stoul(src);
                set(static_cast<uint16_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Unsigned_Int_32: {
                unsigned long src_data = std::stoul(src);
                set(static_cast<uint32_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Unsigned_Int_64: {
                unsigned long long src_data = std::stoull(src);
                set(static_cast<uint64_t>(src_data));
                break;
            }
            case Primitive_Data_Types::Char: {
                if (src.size() != 1) {
                    throw std::invalid_argument(("Value Error: Cannot set string, whose length is not 1, into Char type '" + name + "'").c_str());
                }
                set(src[0]);
                break;
            }
            case Primitive_Data_Types::Boolean: {
                if (src == "true") {
                    set(true);
                } else if (src == "false") {
                    set(false);
                } else {
                    throw std::invalid_argument(("Value Error: Cannot set " + src + " into Boolean type '" + name + "'").c_str());
                }
                break;
            }
            case Primitive_Data_Types::Float_32: {
                float src_data = std::stof(src);
                set(src_data);
                break;
            }
            case Primitive_Data_Types::Float_64: {
                double src_data = std::stod(src);
                set(src_data);
                break;
            }
            }
        }

        virtual int8_t* get_Int_8() const {
            GET_POINTER_MACRO(int8_t, Int_8)
        }
        virtual int16_t* get_Int_16() const {
            GET_POINTER_MACRO(int16_t, Int_16)
        }
        virtual int32_t* get_Int_32() const {
            GET_POINTER_MACRO(int32_t, Int_32)
        }
        virtual int64_t* get_Int_64() const {
            GET_POINTER_MACRO(int64_t, Int_64)
        }

        virtual uint8_t* get_Unsigned_Int_8() const {
            GET_POINTER_MACRO(uint8_t, Unsigned_Int_8)
        }
        virtual uint16_t* get_Unsigned_Int_16() const {
            GET_POINTER_MACRO(uint16_t, Unsigned_Int_16)
        }
        virtual uint32_t* get_Unsigned_Int_32() const {
            GET_POINTER_MACRO(uint32_t, Unsigned_Int_32)
        }
        virtual uint64_t* get_Unsigned_Int_64() const {
            GET_POINTER_MACRO(uint64_t, Unsigned_Int_64)
        }

        virtual char* get_Char() const {
            GET_POINTER_MACRO(char, Char)
        }

        virtual float* get_Float_32() const {
            GET_POINTER_MACRO(float, Float_32)
        }
        virtual double* get_Float_64() const {
            GET_POINTER_MACRO(double, Float_64)
        }

        virtual bool* get_Boolean() const {
            GET_POINTER_MACRO(bool, Boolean)
        }

        virtual Primitive_Data_Types get_Type() const {
            return primitive_data_type;
        }

        virtual std::string string() const {
            if (data == nullptr) return "<void>";
            switch (primitive_data_type) {
            case Primitive_Data_Types::Int_8: return std::to_string(*get_Int_8());
            case Primitive_Data_Types::Int_16: return std::to_string(*get_Int_16());
            case Primitive_Data_Types::Int_32: return std::to_string(*get_Int_32());
            case Primitive_Data_Types::Int_64: return std::to_string(*get_Int_64());

            case Primitive_Data_Types::Unsigned_Int_8: return std::to_string(*get_Unsigned_Int_8());
            case Primitive_Data_Types::Unsigned_Int_16: return std::to_string(*get_Unsigned_Int_16());
            case Primitive_Data_Types::Unsigned_Int_32: return std::to_string(*get_Unsigned_Int_32());
            case Primitive_Data_Types::Unsigned_Int_64: return std::to_string(*get_Unsigned_Int_64());

            case Primitive_Data_Types::Char: return std::string(1, *get_Char());

            case Primitive_Data_Types::Boolean: return *get_Boolean() == true ? "true" : "false";

            case Primitive_Data_Types::Float_32: return std::to_string(*get_Float_32());
            case Primitive_Data_Types::Float_64: return std::to_string(*get_Float_64());
            }
        }
    };

    inline std::unique_ptr<Primitive_Type> _Primitive_Type(Primitive_Data_Types type, std::string name) {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(type, name));
    }
    #define Primitive_Type_ptr(type, name) _Primitive_Type(type, name).get()

    inline std::unique_ptr<Primitive_Type> _Int_8(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Int_8, name));
    }
    #define Int_8(name) _Int_8(name).get()

    inline std::unique_ptr<Primitive_Type> _Int_16(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Int_16, name));
    }
    #define Int_16(name) _Int_16(name).get()

    inline std::unique_ptr<Primitive_Type> _Int_32(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Int_32, name));
    }
    #define Int_32(name) _Int_32(name).get()

    inline std::unique_ptr<Primitive_Type> _Int_64(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Int_64, name));
    }
    #define Int_64(name) _Int_64(name).get()

    inline std::unique_ptr<Primitive_Type> _Unsigned_Int_8(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Unsigned_Int_8, name));
    }
    #define Unsigned_Int_8(name) _Unsigned_Int_8(name).get()

    inline std::unique_ptr<Primitive_Type> _Unsigned_Int_16(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Unsigned_Int_16, name));
    }
    #define Unsigned_Int_16(name) _Unsigned_Int_16(name).get()

    inline std::unique_ptr<Primitive_Type> _Unsigned_Int_32(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Unsigned_Int_32, name));
    }
    #define Unsigned_Int_32(name) _Unsigned_Int_32(name).get()

    inline std::unique_ptr<Primitive_Type> _Unsigned_Int_64(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Unsigned_Int_64, name));
    }
    #define Unsigned_Int_64(name) _Unsigned_Int_64(name).get()

    inline std::unique_ptr<Primitive_Type> _Char(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Char, name));
    }
    #define Char(name) _Char(name).get()

    inline std::unique_ptr<Primitive_Type> _Boolean(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Boolean, name));
    }
    #define Boolean(name) _Boolean(name).get()

    inline std::unique_ptr<Primitive_Type> _Float_32(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Float_32, name));
    }
    #define Float_32(name) _Float_32(name).get()

    inline std::unique_ptr<Primitive_Type> _Float_64(std::string name = "") {
        return std::unique_ptr<Primitive_Type>(new Primitive_Type(Primitive_Data_Types::Float_64, name));
    }
    #define Float_64(name) _Float_64(name).get()

    class Array_Type: public Type {
    private:
        Type* element_type;
        size_t size;
    protected:
        virtual std::string _type(std::string prefix, bool with_name) const {
            return prefix + "[" + std::to_string(size) + "]" + element_type->_type("", false) + (with_name == true? " " + name: "");
        }
        virtual void change_key(std::string target, std::string origin) {
            throw std::invalid_argument(("Compile Error: Cannot change key of an Array Type '" + name + "'").c_str());
        }
    public:
        Array_Type(size_t _size, const Type* type, std::string name):size(_size), element_type(type->clone()), Type(Type_Class::Array, name) {
            element_type->parent_type = this;
        }
        ~Array_Type() { delete element_type; }
        virtual Array_Type* clone() const {
            return new Array_Type(size, element_type, name);
        }
        virtual size_t size_of() const {
            return element_type->size_of() * size;
        }
        virtual Type& operator[](std::string key) {
            throw std::invalid_argument(("Compile Error: Array_Type '" + name + "' cannot be indexed with string `key`").c_str());
        }
        virtual std::unique_ptr<Type> operator[](size_t pos) {
            if (data == NULL) throw std::invalid_argument(("Nullpointer Error: Cannot index into null pointer of type '" + name + "'").c_str());
            // Check for length
            if (pos >= size) throw std::out_of_range(("Index Error: Cannot index over the length of type '" + name + "'").c_str());
            void* shifted_data = static_cast<void*>(static_cast<char*>(data) + element_type->size_of() * pos);
            Type* ptr = element_type->clone();
            ptr->parent_type = this;
            ptr->hold(shifted_data);
            return std::unique_ptr<Type>(ptr);
        }
        virtual size_t get_Size() const {
            return size;
        }
        virtual Type& get_Element_Type() {
            return *element_type;
        }
        virtual std::vector<std::string> get_Keys() const {
            throw std::invalid_argument(("Compile Error: Cannot get keys of Array Type '" + name + "'").c_str());
        }
        virtual Type& append(Type* type) {
            throw std::invalid_argument(("Compile Error: Cannot append to Array Type '" + name + "'").c_str());
        }
        virtual void set(void* src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(int8_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(int16_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(int32_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(int64_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }

        virtual void set(uint8_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(uint16_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(uint32_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(uint64_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }

        virtual void set(char src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }

        virtual void set(float src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual void set(double src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }

        virtual void set(bool src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }

        virtual void set(std::string src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Array type '" + name + "'").c_str());
        }
        virtual int8_t* get_Int_8() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual int16_t* get_Int_16() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual int32_t* get_Int_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual int64_t* get_Int_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }

        virtual uint8_t* get_Unsigned_Int_8() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual uint16_t* get_Unsigned_Int_16() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual uint32_t* get_Unsigned_Int_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual uint64_t* get_Unsigned_Int_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }

        virtual char* get_Char() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }

        virtual float* get_Float_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }
        virtual double* get_Float_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }

        virtual bool* get_Boolean() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Array type '" + name + "'").c_str());
        }

        virtual Primitive_Data_Types get_Type() const {
            throw std::invalid_argument(("Compile Error: Cannot get raw primitive data type directly of Array type '" + name + "'").c_str());
        }

        virtual std::string string() const {
            throw std::invalid_argument(("Compile Error: Cannot turn data of Array type '" + name + "' into string directly").c_str());
        }
    };
    inline std::unique_ptr<Array_Type> _Array_Type(size_t _size, const Type* type, std::string name) {
        return std::unique_ptr<Array_Type>(new Array_Type(_size, type, name));
    }
    #define Array_2(_size, type) _Array_Type(_size, type, "").get()
    #define Array_3(_size, type, name) _Array_Type(_size, type, name).get()
    #define Array_X(x, _size, type, name, FUNC, ...) FUNC
    #define Array(...) Array_X(,##__VA_ARGS__,\
                                Array_3(__VA_ARGS__),\
                                Array_2(__VA_ARGS__)\
                                )
    #define Matrix(row, col, type, name) Array(row, Array(col, type), name)
    #define Tensor(dim_1, dim_2, dim_3, type, name) Array(dim_1, Array(dim_2, Array(dim_3, type)), name)

    class Struct_Type: public Type {
    private:
        std::vector<Type*> types;
        std::vector<std::string> keys;
        std::map<std::string, size_t> type_name_to_pos;
        std::map<std::string, size_t> type_name_to_offset;
    protected:
        virtual std::string _type(std::string prefix, bool with_name) const {
            std::string str = prefix + (with_name == true? name + " " : "") + "{\n";
            for (size_t index = 0; index < types.size(); ++index) {
                str += prefix + types[index]->_type("    ", true);
                if (index != types.size() - 1) str += ", \n";
                else str += "\n";
            }
            str += prefix + "}";
            return str;
        }
        virtual void change_key(std::string target, std::string origin) {
            auto iter = std::find(keys.begin(), keys.end(), origin);
            if (iter == keys.end()) {
                throw std::invalid_argument(("Value Error: Cannot find key '" + origin + "' in Struct Type '" + name + "' for replacement").c_str());
            }
            if (std::find(keys.begin(), keys.end(), target) != keys.end()) {
                throw std::invalid_argument(("Value Error: Cannot change a name of property into an existing one '" + target + "' in Struct Type '" + name + "'").c_str());
            }
            *iter = target;
            type_name_to_pos[target] = type_name_to_pos[origin];
            type_name_to_offset[target] = type_name_to_offset[origin];

            type_name_to_pos.erase(origin);
            type_name_to_offset.erase(origin);
        }
    public:
        /**
         * Sub-variables is arranged corresponding to its position in `_types`.
         */
        Struct_Type(const std::initializer_list<Type*> _types, std::string name):Struct_Type(std::vector<Type*>(_types), name) {}
        /**
         * Sub-variables is arranged corresponding to its position in `_types`.
         */
        Struct_Type(const std::vector<Type*> _types, std::string name):Type(Type_Class::Struct, name) {
            types.resize(_types.size(), nullptr);
            keys.resize(_types.size(), "");
            size_t offset = 0;
            for (size_t index = 0; index < _types.size(); ++index) {
                // Check for Duplication
                if (type_name_to_pos.find(_types[index]->name) != type_name_to_pos.end()) {
                    throw std::invalid_argument(("Value Error: Cannot set duplicate property with name '" + _types[index]->name + "' in type '" + name + "'").c_str());
                }

                types.at(index) = _types[index]->clone();
                types.at(index)->parent_type = this;
                keys.at(index) = types.at(index)->name;

                type_name_to_pos[types.at(index)->name] = index;
                type_name_to_offset[types.at(index)->name] = offset;
                offset += types.at(index)->size_of();
            }
        }
        ~Struct_Type() {
            for (Type* type : types) delete type;
        }
        virtual Struct_Type* clone() const {
            return new Struct_Type(types, name);
        }
        virtual size_t size_of() const {
            size_t sum = 0;
            for (Type* type : types) sum += type->size_of();
            return sum;
        }
        virtual void init() {
            Type::init();
            for (size_t index = 0; index < types.size(); ++index) {
                size_t offset = type_name_to_offset[types[index]->name];
                types[index]->hold(static_cast<void*>(static_cast<char*>(data) + offset));
            }
        }
        virtual void hold(void* _data) {
            Type::hold(_data);
            for (size_t index = 0; index < types.size(); ++index) {
                size_t offset = type_name_to_offset[types[index]->name];
                types[index]->hold(static_cast<void*>(static_cast<char*>(data) + offset));
            }
        }
        virtual void release() {
            Type::release();
            for (size_t index = 0; index < types.size(); ++index) {
                types[index]->release();
            }
        }
        virtual Type& operator[](std::string key) {
            if (type_name_to_pos.find(key) == type_name_to_pos.end()) {
                throw std::invalid_argument(("Value Error: Cannot find key '" + key + "' in type '" + name + "'").c_str());
            }
            size_t pos = type_name_to_pos[key];
            return *types[pos];
        }
        virtual std::unique_ptr<Type> operator[](size_t pos) {
            throw std::invalid_argument(("Compile Error: Struct_Type '" + name + "' cannot be indexed with `pos`").c_str());
        }
        virtual size_t get_Size() const {
            throw std::invalid_argument(("Compile Error: Cannot get size of Struct Type '" + name + "'").c_str());
        }
        virtual Type& get_Element_Type() {
            throw std::invalid_argument(("Compile Error: Cannot get Element Type of Struct Type '" + name + "'").c_str());
        }
        virtual std::vector<std::string> get_Keys() const {
            return keys;
        }
        virtual Type& append(Type* type) {
            if (data != nullptr) {
                throw std::invalid_argument(("Value Error: Cannot append to a Struct Type '" + name + "', which holds or possesses a valid pointer to data").c_str());
            }
            // Check for Duplication
            if (type_name_to_pos.find(type->name) != type_name_to_pos.end()) {
                throw std::invalid_argument(("Value Error: Cannot set duplicate property with name '" + type->name + "' in type '" + name + "'").c_str());
            }
            
            // Must be called first before modifying relevant data fields
            type_name_to_offset[type->name] = size_of();

            types.push_back(type->clone());
            types[types.size() - 1]->parent_type = this;
            keys.push_back(type->name);
            type_name_to_pos[type->name] = types.size() - 1;
            return *this;
        }
        virtual void set(void* src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(int8_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(int16_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(int32_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(int64_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual void set(uint8_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(uint16_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(uint32_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(uint64_t src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual void set(char src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual void set(float src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }
        virtual void set(double src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual void set(bool src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual void set(std::string src) {
            throw std::invalid_argument(("Compile Error: Cannot set directly to data of Struct type '" + name + "'").c_str());
        }

        virtual int8_t* get_Int_8() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual int16_t* get_Int_16() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual int32_t* get_Int_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual int64_t* get_Int_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }

        virtual uint8_t* get_Unsigned_Int_8() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual uint16_t* get_Unsigned_Int_16() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual uint32_t* get_Unsigned_Int_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual uint64_t* get_Unsigned_Int_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }

        virtual char* get_Char() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }

        virtual float* get_Float_32() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }
        virtual double* get_Float_64() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }

        virtual bool* get_Boolean() const {
            throw std::invalid_argument(("Compile Error: Cannot get data directly of Struct type '" + name + "'").c_str());
        }

        virtual Primitive_Data_Types get_Type() const {
            throw std::invalid_argument(("Compile Error: Cannot get raw primitive data type of Struct type '" + name + "'").c_str());
        }

        virtual std::string string() const {
            throw std::invalid_argument(("Compile Error: Cannot turn data of Struct type '" + name + "' into string directly").c_str());
        }
    };

    #define Struct_Clone(struct_ptr, new_name) std::unique_ptr<Struct_Type>(struct_ptr.clone())->set_name(new_name)

    /**
     * Native Support for Serializing Type
     * Descriptor:
     *  Primitive: (#name,#type)
     *  Array: [#name,#array_length,#sub-type descriptor]
     *  Struct: {#name,...(sub-type descriptors)}
     * 
     * For example:
     *      Type:
     *          type {
     *              sub_type {
     *                  Int_8 x,
     *                  Int_8 y
     *              },
     *              [8]Float_32 var
     *          }
     *      is serialized into {type,{sub_type,(x,Int_8),(y,Int_8)},[var,8,(,Float_32)]}
     */
    std::string Serialize(Type* type) {
        if (type->get_Type_Class() == Type_Class::Primitive) {
            return "(" + type->get_name() + "," + get_string_from_type(type->get_Type()) + ")";
        } else if (type->get_Type_Class() == Type_Class::Array) {
            return "[" + type->get_name() + "," + std::to_string(type->get_Size()) + "," + Serialize(&type->get_Element_Type()) + "]";
        } else if (type->get_Type_Class() == Type_Class::Struct) {
            std::string str = "{" + type->get_name();
            for (std::string key : type->get_Keys()) {
                str += "," + Serialize(&type->get(key));
            }
            str += "}";
            return str;
        }
        throw std::invalid_argument("Value Error: Unrecognized Type pointer");
    }
    /**
     * Native Support for Deserializing Type
     * \param src serialized type
     * 
     * Descriptor:
     *  Primitive: (#name,#type)
     *  Array: [#name,#array_length,#sub-type descriptor]
     *  Struct: {#name,...(sub-type descriptors)}
     * 
     * For example:
     *      Type:
     *          {type,{sub_type,(x,Int_8),(y,Int_8)},[var,8,(,Float_32)]}
     *      is deserialized into
     *          type {
     *              sub_type {
     *                  Int_8 x,
     *                  Int_8 y
     *              },
     *              [8]Float_32 var
     *          }
     */
    std::unique_ptr<Type> Deserialize(std::string src) {
        std::string parse_error = "Value Error: Unable to parse " + src;
        // Empty String
        if (src.length() == 0) throw std::invalid_argument(parse_error.c_str());

        char indicator = src[0];
        switch(indicator) {
        case '(': {
            if (src[src.length() - 1] != ')') throw std::invalid_argument(parse_error.c_str());
            size_t name_field_start_pos = 1;
            size_t name_field_end_pos = src.find(',');
            if (name_field_end_pos == std::string::npos) throw std::invalid_argument(parse_error.c_str());
            std::string name = src.substr(name_field_start_pos, name_field_end_pos - name_field_start_pos);
            size_t type_field_start_pos = name_field_end_pos + 1;
            size_t type_field_end_pos = src.length() - 1;
            try {
                std::string type_name = src.substr(type_field_start_pos, type_field_end_pos - type_field_start_pos);
                Primitive_Data_Types type = get_type_from_string(type_name);
                return std::unique_ptr<Type>(new Primitive_Type(type, name));
            } catch(const std::exception& e) {
                throw std::invalid_argument(parse_error.c_str());
            }
            break;
        }
        case '[': {
            if (src[src.length() - 1] != ']') throw std::invalid_argument(parse_error.c_str());
            size_t name_field_start_pos = 1;
            size_t name_field_end_pos = src.find(',');
            if (name_field_end_pos == std::string::npos) throw std::invalid_argument(parse_error.c_str());
            std::string name = src.substr(name_field_start_pos, name_field_end_pos - name_field_start_pos);

            size_t size_field_start_pos = name_field_end_pos + 1;
            size_t size_field_end_pos = src.find(',', size_field_start_pos);
            if (size_field_end_pos == std::string::npos || size_field_end_pos <= size_field_start_pos) throw std::invalid_argument(parse_error.c_str());
            std::string size_str = src.substr(size_field_start_pos, size_field_end_pos - size_field_start_pos);
            size_t size = 0;
            try {
                size = std::stoull(size_str);
            } catch(...){
                throw std::invalid_argument(parse_error.c_str());
            }
            
            size_t type_field_start_pos = size_field_end_pos + 1;
            size_t type_field_end_pos = src.length() - 1;
            std::string type_field_str = src.substr(type_field_start_pos, type_field_end_pos - type_field_start_pos);
            std::unique_ptr<Type> type = Deserialize(type_field_str);

            return std::unique_ptr<Type>(new Array_Type(size, type.get(), name));
            break;
        }
        case '{': {
            if (src[src.length() - 1] != '}') throw std::invalid_argument(parse_error.c_str());
            size_t name_field_start_pos = 1;
            size_t name_field_end_pos = src.find(',');
            if (name_field_end_pos == std::string::npos) throw std::invalid_argument(parse_error.c_str());
            std::string name = src.substr(name_field_start_pos, name_field_end_pos - name_field_start_pos);

            Struct_Type* ret = new Struct_Type({}, name);

            size_t type_field_start_pos = name_field_end_pos + 1;
            size_t type_field_end_pos = src.length() - 1;
            std::string type_field_str = src.substr(type_field_start_pos, type_field_end_pos - type_field_start_pos);
            // Check for Validity
            if (type_field_str[0] != '(' && type_field_str[0] != '[' && type_field_str[0] != '{') throw std::invalid_argument(parse_error.c_str());

            // Use Stack to divide sequence of type
            struct Indicator {
                char indicator;
                size_t pos;
            };
            std::stack<Indicator> Stack;
            std::function<bool(char, char)> is_match = [](char u, char v)->bool{
                return (u == '(' && v == ')') || (u == '[' && v == ']') || (u == '{' && v == '}');
            };

            for (size_t index = 0; index < type_field_str.length(); index++) {
                bool is_open = type_field_str[index] == '(' || type_field_str[index] == '[' || type_field_str[index] == '{';
                bool is_close = type_field_str[index] == ')' || type_field_str[index] == ']' || type_field_str[index] == '}';
                if (is_open) Stack.push({type_field_str[index], index});
                else if (is_close) {
                    if (Stack.empty()) throw std::invalid_argument(parse_error.c_str());
                    Indicator top = Stack.top(); Stack.pop();
                    if (!is_match(top.indicator, type_field_str[index])) throw std::invalid_argument(parse_error.c_str());
                    if (Stack.empty()) { // The Most Outside Type (Exclude Nested Ones)
                        ret->append(Deserialize(type_field_str.substr(top.pos, index - top.pos + 1)).get());
                    }
                }
            }

            return std::unique_ptr<Type>(ret);
            break;
        }
        default:
            throw std::invalid_argument(parse_error.c_str());
        }
    }
};

#endif