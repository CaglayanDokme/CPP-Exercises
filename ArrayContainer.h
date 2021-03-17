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
 *                                -> Modifier functions added.
 *                                -> Container iterators added.
 *              March 14, 2021    -> Iterator classes enhanced.
 *              March 17, 2021    -> Move assignment operator added.
 *                                -> const specifier removed from size member.
 *                                -> Header files explicitly included.
 *                                -> Iterators simplified.
 *                                -> Non-zero initial size requirement removed.
 *                                -> [[nodiscard]] attribute added to related functions.
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <ostream>
#include <istream>
#include <cstddef>
#include <stdexcept>
#include <initializer_list>
#include <cassert>

template<class T>
class Array{
public:
    /*** Constructors and Destructors ***/
    Array(const size_t arraySize = 0) noexcept;         // Construct by size
    Array(const Array& copyArr) noexcept;               // Copy constructor
    Array(const Array&& moveArr) noexcept;              // Move constructor
    Array(const T* const source, const size_t size);    // Construct via C-Style array
    Array(std::initializer_list<T> initializerList);    // Construct with initializer list

    virtual ~Array(); // Destructor defined virtual to support efficient polymorphism

    /*** Operator Overloadings ***/
    [[nodiscard]] const T& operator[](const size_t index) const;          // Subscript operator for const objects returns rValue
    [[nodiscard]] T&  operator[](const size_t index);                     // Subscript operator for non-const objects returns lValue

    [[nodiscard]] bool operator==(const Array& rightArr) const noexcept;           // Array comparison
    [[nodiscard]] bool operator!=(const Array& rightArr) const noexcept;           // Array comparison by inequality

    Array& operator=(const Array& rightArr) noexcept;   // Copy assignment
    Array& operator=(const Array&& rightArr) noexcept;  // Move assignment

    /*** Element Access ***/
    [[nodiscard]] T& at(const size_t position) noexcept               { return (*this)[position]; }    // Invoke subscript operator
    [[nodiscard]] const T& at(const size_t position) const noexcept   { return (*this)[position]; }    // Invoke subscript operator
    [[nodiscard]] T& First() noexcept             { return (*this)[0]; }            // Invoke subscript operator for the first element
    [[nodiscard]] const T& First() const noexcept { return (*this)[0]; }            // Invoke subscript operator for the first element
    [[nodiscard]] T& Last() noexcept              { return (*this)[size - 1]; }     // Invoke subscript operator for the last element
    [[nodiscard]] const T& Last() const noexcept  { return (*this)[size - 1]; }     // Invoke subscript operator for the last element

    /*** Modifiers ***/
    Array& Fill(const T& fillValue) noexcept;
    Array& Swap(Array& anotherArray) noexcept;

    /*** Status Checkers ***/
    [[nodiscard]] size_t getSize() const noexcept  { return size; }

    /*** Iterators ***/
    /* Pointers can be used as iterator as the data structure of the container is completely linear */
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin()                { return data;          }
    iterator end()                  { return data + size;   }
    const_iterator cbegin() const   { return data;          }
    const_iterator cend()   const   { return data + size;   }

private:
    /*** Members ***/
    size_t size = 0;        // Size will be initialized at constructor
    T* data     = nullptr;  // Pointer will be used for addressing the allocated area
};

/**
 * @brief   Constructs the internal array of given size
 * @param   arraySize Allocation size
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const size_t arraySize) noexcept
    : size(arraySize), data(new T[size])
{
    /* No operation required */
}

/**
 * @brief   Copy constructor
 * @param   copyArr     Source array
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const Array<T>& copyArr) noexcept
: size(copyArr.getSize()), data(new T[size])
{
    for(size_t index = 0; index < size; ++index)    // Element wise copy
        data[index] = copyArr[index];
}

/**
 * @brief   Move constructor
 * @param   moveArr     Source array, created locally
 */
template<class T>
Array<T>::Array(const Array<T>&& moveArr) noexcept
: size(moveArr.getSize()), data(moveArr.data)
{
    /* No need to make an element wise copy as the source is
       a constant array. Assigning a nullptr to moveArr's container
       prevents destroying its content as we used its resources
       to construct the new one.*/
    const_cast<Array<T>&>(moveArr).data = nullptr;
}

/**
 * @brief   Construct with C-Style array
 * @param   source  Source array
 * @param   size    Source array size
 * @throws  std::logic_error When source is invalid
 */
template<class T>
Array<T>::Array(const T* const source, const size_t size)
: size(size), data(new T[size])
{
    if(source == nullptr)
        throw std::logic_error("Invalid source!");

    for(size_t index = 0; index < size; ++index)    // Element wise copy
        data[index] = source[index];
}

/**
 * @brief   Construction with initializer list
 * @param   initializerList   Initializer list
 */
template<class T>
Array<T>::Array(std::initializer_list<T> initializerList)
: size(initializerList.size()), data(new T[size])
{
    size_t index = 0;   // Element wise copy
    for(const T& element : initializerList)
        data[index++] = element;
}

/**
 * @brief Destructor
 */
template<class T>
Array<T>::~Array()
{
    delete [] data;        // Deleting a nullptr is safe, don't worry
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
        return data[index];

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
        return data[index];

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
bool Array<T>::operator==(const Array<T>& rightArr) const noexcept
{
    if(rightArr.size != size) // Size should be the same to make a proper comparison
        return false;

    if(rightArr.data == data) // Self comparison
        return true;

    for(size_t index = 0; index < size; ++index)    // Iterate on both arrays
        if((*this)[index] != rightArr[index])       // operator== must have been overloaded for non-built-in types
            return false;                           // Return false in case of an inequal element

    return true;    // Arrays are the same
}

/**
 * @brief   Overloaded incomparison operator
 * @param   rightArr    Array to be compared against
 * @return  true        If arrays are not equal
 *          false       If arrays are equal
 */
template<class T>
bool Array<T>::operator!=(const Array<T>& right) const noexcept
{   // Inequality operator returns the opposite of equality operator
    return !(*this == right);   // Invokes Array::operator==
}

/**
 * @brief   Copy assigment operator
 * @param   rightArr      Source array
 * @return  lValue reference to resulting array to support cascaded assignments(e.g. arr = arr1 = arr2)
 *
 * @note    The content of left array will be deleted. So, be careful.
 */
template<class T>
Array<T>& Array<T>::operator=(const Array<T>& rightArr) noexcept
{
    if(rightArr.data == data) // Check self assignment
        return *this;

    delete [] data;                     // Destroy left array
    data = new T[rightArr.getSize()];   // Allocate space for incoming elements
    size = rightArr.getSize();          // Determine new array size

    // Element wise copy
    for(size_t index = 0; index < rightArr.getSize(); ++index)
        data[index] = rightArr[index];

    return *this;
}

/**
 * @brief   Move assignment operator
 * @param   rightArr    Source array
 * @return  lValue reference to resulting array to support cascaded assignments(e.g. arr = arr1 = arr2)
 */
template<class T>
Array<T>& Array<T>::operator=(const Array&& rightArr) noexcept
{
    if(this == &rightArr)
        return *this;

    // Release the allocated resource
    delete [] data;

    // Steal the resource of the right array
    data = rightArr.data;
    size = rightArr.size;

    // Prevent destrutcion of the stolen resource
    const_cast<Array<T>&>(rightArr).data = nullptr;

    return *this;
}

/**
 * @brief   Fills the array with the given value
 * @param   fillValue Value to be used to fill the array
 * @return  lValue reference to support cascaded calls
 */
template<class T>
Array<T>& Array<T>::Fill(const T& fillValue) noexcept
{
    for (size_t index = 0; index < size; ++index)
        data[index] = fillValue;

    return *this;
}

/**
 * @brief   Swaps the content of two different array
 * @param   anotherArray Array to be swapped with this
 * @return  lValue reference to support cascaded calls
 */
template<class T>
Array<T>& Array<T>::Swap(Array<T>& anotherArray) noexcept
{
    if(anotherArray.data == data)
        return *this;

    T* tempPtr      = data;         // Save left container
    size_t tempSize = size;         // Save left size

    data = anotherArray.data;       // Assign to left container
    size = anotherArray.size;       // Assign to left size

    anotherArray.data = tempPtr;    // Assign to right container
    anotherArray.size = tempSize;   // Assign to right size

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
std::ostream& operator<<(std::ostream& stream, const Array<T>& array) noexcept
{
    for(size_t index = 0; index < array.getSize(); ++index)
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
std::istream& operator>>(std::istream& stream, Array<T>& array) noexcept
{
    for(size_t index = 0; index < array.getSize(); ++index)
        stream >> array[index];

    return stream;  // Return reference to support cascade streaming
}

#endif  // Recursive inclusion preventer endif
