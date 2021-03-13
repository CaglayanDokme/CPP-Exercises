/**
 * @file        ArrayContainer.h
 * @details     A template container class with array data structure.
 *              Provides copy construction, array-copy features, array comparison etc.
 *              Provides informative exception messages.
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        February 23, 2021 -> First release
 *              February 24, 2021 -> Array comparison and assignment added.
 *                                -> Copy and move constructor added.
 *                                -> Construction with C-Style array added.
 *                                -> Stream insertion operators overloaded.
 *                                -> Initializer list constructor added.
 *                                -> Constructor exception mechanism enhanced.
 *              February 25, 2021 -> File documented with doxygen.
 *              March 13, 2021    -> Recursive inclusion preventer added.
 *                                -> Element access functions added.
 *
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright. Code is open source.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <iostream>
#include <exception>

template<class T>
class Array{
public:
    /*** Constructors and Destructors ***/
    Array() = delete;                                   // Default constructor is prohibited
    Array(const size_t arraySize);                      // Construct by size
    Array(const Array& copyArr);                        // Copy constructor
    Array(Array&& moveArr);                             // Move constructor
    Array(const T* const source, const size_t size);    // Construct via C-Style array
    Array(std::initializer_list<T> initializerList);    // Construct with initializer list

    virtual ~Array(); // Destructor defined virtual to support efficient polymorphism

    /*** Operator Overloadings ***/
    const T& operator[](const size_t index) const;          // Subscript operator for const objects returns rValue
    T&  operator[](const size_t index);                     // Subscript operator for non-const objects returns lValue

    bool operator==(const Array& rightArr) const;           // Array comparison
    bool operator!=(const Array& rightArr) const;           // Array comparison by inequality

    const Array& operator=(const Array& rightArr);          // Copy assignment

    /*** Element Access ***/
    T& at(const size_t position)                { return operator[](position); }    // Invoke subscript operator
    const T& at(const size_t position) const    { return operator[](position); }    // Invoke subscript operator
    T& front()             { return operator[](0); }    // Invoke subscript operator for first element
    const T& front() const { return operator[](0); }    // Invoke subscript operator for first element

    /*** Status Checkers ***/
    size_t getSize(void) const  { return (container == nullptr) ? 0 : size; }

private:
    /*** Members ***/
    const size_t size   = 0;        // Size will be initialized at constructor
    T* container        = nullptr;  // Pointer will be used for addressing the allocated area
};

/**
 * @brief   Constructs the internal array of given size
 * @param   arraySize Allocation size
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const size_t arraySize)
: size(arraySize), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];
}

/**
 * @brief   Copy constructor
 * @param   copyArr     Source array
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const Array<T>& copyArr)
: size(copyArr.getSize()), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    container = new T[size];    // Allocate space to copy elements

    // Element wise copy
    for(size_t index = 0; index < copyArr.getSize(); index++)
        container[index] = copyArr[index];
}

/**
 * @brief   Move constructor
 * @param   moveArr     Source array, created locally
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(Array<T>&& moveArr)
: size(moveArr.getSize()), container(moveArr.container)
{
    /* No need to make an element wised copy as the source is
       a constant array. Assigning nullptr to moveArr's container
       prevents destroying its content as we used its resources
       to construct the new one.*/
    moveArr.container = nullptr;
}

/**
 * @brief   Construct with C-Style array
 * @param   source  Source array
 * @param   size    Source array size
 * @throws  std::logic_error When size is zero
 * @throws  std::logic_error When source is invalid
 */
template<class T>
Array<T>::Array(const T* const source, const size_t size)
: size(size), container(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");
    else if(source == nullptr)
        throw std::logic_error("Invalid source!");
    else{ }

    container = new T[size];    // Allocate space to copy elements

    for(size_t index = 0; index < size; index++)    // Element wise copy
        (*this)[index] = source[index];
}

/**
 * @brief   Construction with initializer list
 * @param   initializerList   Initializer list
 * @throws  std::logic_error When size of initializer list is zero
 */
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

/**
 * @brief Destructor
 */
template<class T>
Array<T>::~Array()
{
    delete [] container;    // Deleting a nullptr is safe, don't worry
}


/**
 * @brief   Subscript operator for rValue return
 * @param   index   Index of element to be fetched
 * @return  rValue reference to the data at given index
 * @throws  std::range_error When given index is out of container range
 */
template<class T>
const T& Array<T>::operator[](const size_t index) const
{
    if(index < size)    // Check for out-of-range random access
        return container[index];

    /*  In case of an attempt to access an out-of-range element
        Throw an exception with related information messages.   */
    throw std::range_error("Out-of-range exception occured!");
}

/**
 * @brief   Subscript operator for lValue return
 * @param   index   Index of element to be fetched
 * @return  lValue reference to the data at given index
 * @throws  std::range_error When given index is out of container range
 */
template<class T>
T& Array<T>::operator[](const size_t index)
{
    if(index < size)    // Check for out-of-range random access
        return container[index];

    /*  In case of an attempt to access an out-of-range element
        Throw an exception with related information messages.   */
    throw std::range_error("Out-of-range exception occured!");
}

/**
 * @brief   Overloaded comparison operator
 * @param   rightArr Array to be compared against
 * @return  true     If arrays are equal.
 *          false    If any difference is detected.
 */
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
        if((*this)[index] != rightArr[index])       // operator== must have been overloaded for non-built-in types
            return false;   // Return false in case of an inequal element

    return true;    // Arrays are the same
}

/**
 * @brief   Overloaded comparison operator
 * @param   rightArr    Array to be compared against
 * @return  true        If arrays are not equal
 *          false       If arrays are equal
 */
template<class T>
bool Array<T>::operator!=(const Array<T>& right) const
{   // Inequality operator returns the opposite of equality operator
    return !(*this == right);   // Invokes Array::operator==
}


/**
 * @brief   Assigment operator
 * @param   rightArr      Source array
 * @return  rValue reference to resulting array to support cascaded assignments(e.g. arr = arr1 = arr2)
 *
 * @note    The content of left array will be deleted. So, be careful.
 */
template<class T>
const Array<T>& Array<T>::operator=(const Array<T>& rightArr)
{
    if(rightArr.container == container) // Check self assignment
        return *this;

    delete [] container;                                // Destroy left array
    container = new T[rightArr.getSize()];              // Allocate space for incoming elements
    const_cast<size_t&>(size) = rightArr.getSize();     // Determine new array size

    // Element wise copy
    for(size_t index = 0; index < rightArr.getSize(); index++)
        container[index] = rightArr[index];

    return *this;
}

/**
 * @brief   Overloaded output instertion operator
 * @param   stream  Destination output stream for insertion
 * @param   array   Array to be inserted
 * @return  ostream reference to support cascaded insertions.
 *
 * @note    Stream operators must be declared global as the left objects
 *          of them will always be members of type ostream or istream.
 */
template<class T>
std::ostream& operator<<(std::ostream& stream, const Array<T>& array)
{
    for(size_t index = 0; index < array.getSize(); index++)
        stream << array[index] << " ";

    return stream;  // Return reference to support cascade streaming
}

/**
 * @brief   Overloaded input instertion operator
 * @param   stream  Source input stream for insertion
 * @param   array   Array to be inserted
 * @return  istream reference to support cascaded insertions.
 *
 * @note    Stream operators must be declared global as the left objects
 *          of them will always be members of type ostream or istream.
 */
template<class T>
std::istream& operator>>(std::istream& stream, Array<T>& array)
{
    for(size_t index = 0; index < array.getSize(); index++)
        stream >> array[index];

    return stream;  // Return reference to support cascade streaming
}

#endif  // Recursive inclusion preventer endif
