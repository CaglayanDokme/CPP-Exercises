/* Description :    A template container class with array data structure.
 *                  Provides copy construction, array-copy features, array comparison etc.
 *                  Provides informative exception messages.
 * Author :         Caglayan DOKME, caglayandokme@gmail.com
 * Date :           February 23, 2021 -> First release
 *                  February 24, 2021 -> Array comparison and assignment added.
 *                                       Copy and move constructor added.
 *                                       Construction with traditional array added.
 *                                       Stream insertion operators overloaded.
 *                                       Initializer list constructor added.
 *                                       Constructor exception mechanism enhanced.
 */

#include <iostream>
#include <exception>
#include <string>

template<class T>
class Array{
public:
    Array(const size_t arraySize);          // Construct by size
    Array(const Array<T>& copyArr);         // Copy constructor
    Array(Array<T>&& moveArr);              // Move constructor
    Array(const T* const source, const size_t size);    // Construct via traditional array
    Array(std::initializer_list<T> initializerList);

    virtual ~Array(); // DTor defined virtual to support efficient polymorphism

    T   operator[](const size_t index) const;   // Subscript operator for const objects returns rValue
    T&  operator[](const size_t index);         // Subscript operator for non-const objects returns lValue

    bool operator==(const Array<T>& rightArr) const;    // Array comparison
    bool operator!=(const Array<T>& rightArr) const;    // Array comparison by inequality

    const Array<T>& operator=(const Array<T>& rightArr);    // Array assignment

    /* Declaring a function as a friend inside of a template class
       corrupts its usage. You may want to check the holy StackOverflow :)
       stackoverflow.com/questions/4660123 */
    template<class _T>
    friend std::ostream& operator<<(std::ostream& stream, const Array<_T>& array);

    template<class _T>
    friend std::istream& operator>>(std::istream& stream, Array<_T>& array);

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
Array<T>::Array(const Array<T>& copyArr)
: size(copyArr.getSize()), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];    // Allocate space to copy elements

    // Element wise copy
    for(size_t index = 0; index < copyArr.getSize(); index++)
        (*this)[index] = copyArr[index];
}

template<class T>
Array<T>::Array(Array<T>&& moveArr)
: size(moveArr.getSize()), container(moveArr.container)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    /* No need to make an element wised copy as the source is
       a constant array. Assigning nullptr to moveArr's container
       prevents destroying its content as we used its resources
       to construct the new one.*/
    moveArr.container = nullptr;
}

template<class T>
Array<T>::Array(const T* const source, const size_t size)
: size(size), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];    // Allocate space to copy elements

    for(size_t index = 0; index < size; index++)    // Element wise copy
        (*this)[index] = source[index];
}

template<class T>
Array<T>::Array(std::initializer_list<T> initializerList)
: size(initializerList.size()), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];    // Allocate space to copy elements

    size_t index = 0;   // Element wise copy
    for(const T& element : initializerList)
        container[index++] = element;
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
        if((*this)[index] != rightArr[index])     // operator== must have been overloaded for non-built-in types
            return false;   // Return false in case of any little difference

    return true;    // Arrays are the same
}

template<class T>
bool Array<T>::operator!=(const Array<T>& right) const
{   // Inequality operator returns the opposite of equality operator
    return !(*this == right);   // Invokes Array::operator==
}

template<class T>
const Array<T>& Array<T>::operator=(const Array<T>& rightArr)
{   // Return a const reference to support cascade assignments(e.g. arr = arr1 = arr2)
    delete [] container;    // Destroy left array

    container = new T[rightArr.getSize()];              // Allocate space for incoming elements
    const_cast<size_t&>(size) = rightArr.getSize();     // Determine new array size

    // Element wise copy
    for(size_t index = 0; index < rightArr.getSize(); index++)
        (*this)[index] = rightArr[index];

    return *this;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const Array<T>& array)
{
    /* Stream operators must be declared global as the
       left objects of them will always be members of
       type ostream or istream.*/
    if(array.container == nullptr)
        stream << "Array is empty!";

    for(size_t index = 0; index < array.getSize(); index++)
        stream << array[index] << " ";

    return stream;  // Return reference to support cascade streaming
}

template<class T>
std::istream& operator>>(std::istream& stream, Array<T>& array)
{
    /* Stream operators must be declared global as the
       left objects of them will always be members of
       type ostream or istream.*/
    if(array.container == nullptr)
        throw "Non-initialized array cannot get inputs!";

    for(size_t index = 0; index < array.getSize(); index++)
        stream >> array[index];

    return stream;  // Return reference to support cascade streaming
}
