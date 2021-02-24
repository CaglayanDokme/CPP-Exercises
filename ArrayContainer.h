// Description :    A template container class with aray data structure.
//                  Provides copy construction, array-copy features.
//                  Provides array comparison.
//                  Provides informative exception messages.
// Author :         Caglayan DOKME
// Date :           February 23, 2021 -> First release
//                  February 24, 2021 -> Array comparison added.

#include <iostream>
#include <exception>
#include <string>

template<class T>
class Array{
public:
    Array(const size_t arraySize);  // Construct by size
    virtual ~Array();               // DTor defined virtual to provide efficient polymorphism

    T   operator[](const size_t index) const;   // Subscript operator for const objects returns rValue
    T&  operator[](const size_t index);         // Subscript operator for non-const objects returns lValue

    bool operator==(const Array<T>& rightArr) const;    // Array comparison
    bool operator!=(const Array<T>& rightArr) const;    // Array comparison by inequality

    size_t getSize(void) const
    { return (container == nullptr) ? 0 : size; }

private:
    const size_t size   = 0;        // Size will be initialized at constructor, and will never change again
    T* container        = nullptr;  // Pointer will be used for pointing the allocated area
};

template<class T>
Array<T>::Array(size_t arraySize)
: size(arraySize), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];
}

template<class T>
Array<T>::~Array()
{
    delete [] container;    // Deleting a nullptr is safe, don't worry
}

template<class T>
T Array<T>::operator[](const size_t index) const
{
    if(index < size)    // Check for out-of-range random access
        return container[index];

    if(container == nullptr)
        throw std::logic_error("Container deleted or has not been allocated properly!");

    /*  In case of an attempt to access an out-of-range element
        Throw an exception with related information messages.   */
    std::string errorMessage = "Out-of-Range Exception Occured ";
                errorMessage += "(Size = "  + std::to_string(size)  + ") ";
                errorMessage += "(Index = " + std::to_string(index) + ") ";
    throw std::range_error(errorMessage);
}

template<class T>
T& Array<T>::operator[](const size_t index)
{
    if(index < size)    // Check for out-of-range random access
        return container[index];

    if(container == nullptr)
        throw std::logic_error("Container deleted or has not been allocated properly!");

    /*  In case of an attempt to access an out-of-range element
        Throw an exception with related information messages.   */
    std::string errorMessage = "Out-of-Range Exception Occured ";
                errorMessage += "(Size = "  + std::to_string(size)  + ") ";
                errorMessage += "(Index = " + std::to_string(index) + ") ";
    throw std::range_error(errorMessage);
}

template<class T>
bool Array<T>::operator==(const Array<T>& rightArr) const
{
    if(rightArr.size != size)           // Size should be the same to make a proper comparison
        return false;

    if(rightArr.container == nullptr)   // Empty arrays cannot be equal to anything
        return false;

    if(rightArr.container == container) // Self comparison
        return true;

    for(size_t index = 0; index < size; index++)    // Iterate on both arrays
        if(container[index] != rightArr[index])     // operator== must have been overloaded for non-built-in types
            return false;   // Return false in case of any little difference

    return true;    // Arrays are the same
}

template<class T>
bool Array<T>::operator!=(const Array<T>& right) const
{   // Inequality operator returns the opposite of equality operator
    return !(*this == right);   // Invokes Array::operator==
}
