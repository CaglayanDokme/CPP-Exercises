// Description :    A template container class with aray data structure.
//                  Provides copy construction, array-copy features.
//                  Provides informative exception messages.
// Author :         Caglayan DOKME
// Date :           February 23, 2021 -> First release

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
