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
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright.
 */

/** Recursive inclusion preventer **/
#ifndef ARRAY_CONTAINER_H
#define ARRAY_CONTAINER_H

/** Libraries **/
#include <iostream>

template<class T>
class Array{
public:
    /*** Constructors and Destructors ***/
    Array() = delete;                                   // Default constructor is prohibited
    Array(const size_t arraySize);                      // Construct by size
    Array(const Array& copyArr);                        // Copy constructor
    Array(const Array&& moveArr) noexcept;              // Move constructor
    Array(const T* const source, const size_t size);    // Construct via C-Style array
    Array(std::initializer_list<T> initializerList);    // Construct with initializer list

    virtual ~Array(); // Destructor defined virtual to support efficient polymorphism

    /*** Operator Overloadings ***/
    const T& operator[](const size_t index) const;          // Subscript operator for const objects returns rValue
    T&  operator[](const size_t index);                     // Subscript operator for non-const objects returns lValue

    bool operator==(const Array& rightArr) const noexcept;           // Array comparison
    bool operator!=(const Array& rightArr) const noexcept;           // Array comparison by inequality

    Array& operator=(const Array& rightArr) noexcept;   // Copy assignment
    Array& operator=(const Array&& rightArr) noexcept;  // Move assignment

    /*** Element Access ***/
    T& at(const size_t position) noexcept               { return (*this)[position]; }    // Invoke subscript operator
    const T& at(const size_t position) const noexcept   { return (*this)[position]; }    // Invoke subscript operator
    T& First() noexcept             { return (*this)[0]; }            // Invoke subscript operator for the first element
    const T& First() const noexcept { return (*this)[0]; }            // Invoke subscript operator for the first element
    T& Last() noexcept              { return (*this)[size - 1]; }     // Invoke subscript operator for the last element
    const T& Last() const noexcept  { return (*this)[size - 1]; }     // Invoke subscript operator for the last element

    /*** Modifiers ***/
    Array& Fill(const T& fillValue) noexcept;
    Array& Swap(Array& anotherArray) noexcept;

    /*** Status Checkers ***/
    size_t getSize() const noexcept  { return (data == nullptr) ? 0 : size; }

    /*** Iterators ***/
    class iterator{
        friend class Array;

    protected:  // Prevent creation of iterator objects solely by the user
        iterator() = delete;
        iterator(Array& array, const size_t position) : array(array), currentPos(position)
        {
            if(array.getSize() < position)
                throw std::logic_error("Iterator cannot reside outside of the array.");
        }

    public:
        // Positional operators
        iterator& operator++()       { if(currentPos < array.getSize())  currentPos++; return *this; }  // Prefix increment
        iterator& operator++(int)    { if(currentPos < array.getSize())  currentPos++; return *this; }  // Postfix increment
        iterator& operator--()       { if(currentPos > 0)                currentPos--; return *this; }  // Prefix decrement
        iterator& operator--(int)    { if(currentPos > 0)                currentPos--; return *this; }  // Postfix decrement

        // Comparison operators
        bool operator==(const iterator& anotherIt) const { return ((anotherIt.currentPos == currentPos) && (anotherIt.array == array)); }
        bool operator!=(const iterator& anotherIt) const { return !operator==(anotherIt); }

        // Element access by lValue reference
        T& operator*() { return array[currentPos]; }

    protected:
        Array& array        = nullptr;
        size_t currentPos   = 0;
    };

    class const_iterator{
        friend class Array;

    protected:  // Prevent creation of iterator objects solely by the user
        const_iterator() = delete;
        const_iterator(const Array& array, const size_t position) : array(array), currentPos(position)
        {
            if(array.getSize() < position)
                throw std::logic_error("Iterator cannot reside outside of the array.");
        }

    public:
        // Positional operators
        const_iterator& operator++()       { if(currentPos < array.getSize())  currentPos++; return *this; }  // Prefix increment
        const_iterator& operator++(int)    { if(currentPos < array.getSize())  currentPos++; return *this; }  // Postfix increment
        const_iterator& operator--()       { if(currentPos > 0)                currentPos--; return *this; }  // Prefix decrement
        const_iterator& operator--(int)    { if(currentPos > 0)                currentPos--; return *this; }  // Postfix decrement

        // Comparison operators
        bool operator==(const const_iterator& anotherIt) const { return ((anotherIt.currentPos == currentPos) && (anotherIt.array == array)); }
        bool operator!=(const const_iterator& anotherIt) const { return !operator==(anotherIt); }

        // Element access by rValue reference
        const T& operator*() const { return  this->array[this->currentPos]; }

    protected:
        const Array& array  = nullptr;
        size_t currentPos   = 0;
    };

    iterator begin()                { return iterator(*this, 0);            }
    iterator end()                  { return iterator(*this, size);         }
    const_iterator cbegin() const   { return const_iterator(*this, 0);      }
    const_iterator cend()   const   { return const_iterator(*this, size);   }

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
Array<T>::Array(const size_t arraySize)
: size(arraySize), data(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    data = new T[size];
}

/**
 * @brief   Copy constructor
 * @param   copyArr     Source array
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const Array<T>& copyArr)
: size(copyArr.getSize()), data(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    data = new T[size];    // Allocate space to copy elements

    // Element wise copy
    for(size_t index = 0; index < copyArr.getSize(); index++)
        data[index] = copyArr[index];
}

/**
 * @brief   Move constructor
 * @param   moveArr     Source array, created locally
 * @throws  std::logic_error When size is zero
 */
template<class T>
Array<T>::Array(const Array<T>&& moveArr) noexcept
: size(moveArr.getSize()), data(moveArr.data)
{
    /* No need to make an element wised copy as the source is
       a constant array. Assigning nullptr to moveArr's container
       prevents destroying its content as we used its resources
       to construct the new one.*/
    const_cast<Array<T>&>(moveArr).data = nullptr;
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
: size(size), data(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");
    else if(source == nullptr)
        throw std::logic_error("Invalid source!");
    else{ }

    data = new T[size];    // Allocate space to copy elements

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
: size(initializerList.size()), data(nullptr)
{
    if(size == 0)    // Create array only if the size is valid(positive)
        throw std::logic_error("Array size cannot be zero!");

    data = new T[size];    // Allocate space to copy elements

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
    if(rightArr.size != size)           // Size should be the same to make a proper comparison
        return false;

    if(rightArr.data == nullptr)   // Empty arrays cannot be equal to anything
        return false;

    if(rightArr.data == data) // Self comparison
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

    delete [] data;                                // Destroy left array
    data = new T[rightArr.getSize()];              // Allocate space for incoming elements
    size = rightArr.getSize();     // Determine new array size

    // Element wise copy
    for(size_t index = 0; index < rightArr.getSize(); index++)
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
    size = 0;

    // Steal the resource of the right array
    data = rightArr.data;
    size = rightArr.size;

    // Prevent destrutcion of stolen resource
    const_cast<Array<T>&>(rightArr).data = nullptr;
    const_cast<Array<T>&>(rightArr).size = 0;

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
    for (size_t index = 0; index < size; index++)
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
