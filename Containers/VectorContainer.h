/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 30, 2021 -> First release
 *              March 31, 2021 -> Single value insert(..) added.
 *              April 12, 2021 -> Allocation policy optimizated using placement new.
 *              April 14, 2021 -> Doxygen completed for all methods.
 *                             -> Nodiscard attribute added to related functions.
 *                             -> std::distance used for calculating the number of elements using input iterators
 *                             -> A grow(..) function added for common operations inside the container.
 *              April 15, 2021 -> noexcept specifiers added.
 *              April 18, 2021 -> Perfect forwarding issue inside the emplace methods fixed.
 *              April 19, 2021 -> Allocator policy added.
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright.
 */

/*** Recursive inclusion preventer ***/
#ifndef VECTOR_CONTAINER_H
#define VECTOR_CONTAINER_H

/*** Libraries ***/
#include <initializer_list>     // std::initializer_list
#include <type_traits>          // For exception conditions
#include <stdexcept>            // exceptions
#include <iterator>             // std::distance
#include <cstddef>              // std::size_t, ptrdiff_t
#include <utility>              // std::move
#include <ostream>              // std::cout
#include <memory>               // std::allocator, std::allocator_traits

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, class Allocator = std::allocator<T>>
class Vector{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type      = T             ;
    using reference       = T&            ;
    using const_reference = const T&      ;
    using iterator        = T*            ;
    using const_iterator  = const T*      ;
    using difference_type = std::ptrdiff_t;
    using size_type       = std::size_t   ;

    /*** Constructors and Destructors ***/
    Vector() noexcept;                          // Default constructor
    Vector(const size_type numberOfElements);   // Fill constructor
    Vector(const size_type numberOfElements, const value_type& fillValue);  // Fill construcotr with fill value

    template<class InputIterator>
    Vector(InputIterator first, InputIterator last);                        // Range constructor
    Vector(const Vector& copyVector);                                       // Copy constructor
    Vector(Vector&& moveVector) noexcept(std::is_nothrow_swappable_v<T*>);  // Move constructor
    Vector(std::initializer_list<value_type> initializerList);              // Initializer list constructor

    ~Vector() noexcept(std::is_nothrow_destructible_v<T>);  // Destructor

    /*** Operator Overloadings ***/
    Vector& operator=(const Vector& copyVector);                            // Copy assignment operator
    Vector& operator=(std::initializer_list<value_type> initializerList);   // Initializer list assignment operator
    Vector& operator=(Vector&& moveVector) noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_swappable_v<T*>);     // Move assignment operator

    NODISCARD reference       operator[](const size_type position)        { return data[position]; }  // Element access by lValue
    NODISCARD const_reference operator[](const size_type position) const  { return data[position]; }  // Element access by const lValue

    bool operator==(const Vector& rightVector) const;
    bool operator!=(const Vector& rightVector) const { return !(*this == rightVector); }

    /*** Element Access ***/
    NODISCARD reference       at(const size_type index);                    // Random access with range check
    NODISCARD const_reference at(const size_type index) const;              // Random access with range check
    NODISCARD reference       front()         { return data[0]; }           // Access to the first element
    NODISCARD const_reference front() const   { return data[0]; }           // Access to the first element
    NODISCARD reference       back()          { return data[sz - 1]; }      // Access to the last element
    NODISCARD const_reference back() const    { return data[sz - 1]; }      // Access to the last element

    /*** Iterators ***/
    NODISCARD iterator begin() noexcept     { return data;      }   // Iterator starting from the first element
    NODISCARD iterator end() noexcept       { return data + sz; }   // Iterator starting from the next of the last element
    NODISCARD const_iterator begin()  const noexcept { return data;      }   // Iterator starting from the first element
    NODISCARD const_iterator end()    const noexcept { return data + sz; }   // Iterator starting from the next of the last element
    NODISCARD const_iterator cbegin() const noexcept { return data;      }   // Iterator starting from the first element
    NODISCARD const_iterator cend()   const noexcept { return data + sz; }   // Iterator starting from the next of the last element

    /*** Modifiers ***/
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last);                   // Range assign
    void assign(size_type numberOfElements, const_reference fillValue);   // Fill assign
    void assign(std::initializer_list<value_type> initializerList);         // Initializer list assign

    void push_back(const_reference value);    // Push back by copying
    void push_back(value_type&& value);         // Push back by moving
    void pop_back() noexcept(std::is_nothrow_destructible_v<T>);    // Remove last element

    template<class InputIterator>
    iterator insert(iterator position, InputIterator first, InputIterator last);                // Range insertion
    iterator insert(iterator position, const_reference value);                                // Single element insertion
    iterator insert(iterator position, value_type&& value);                                     // Move insertion
    iterator insert(iterator position, size_type numberOfElements, const_reference value);    // Multiple insertion and fill
    iterator insert(iterator position, std::initializer_list<value_type> il);                   // Initializer list insertion

    iterator erase(iterator position);              // Single element erase
    iterator erase(iterator first, iterator last);  // Iterator based multiple erase
    void swap(Vector& swapVector) noexcept(std::is_nothrow_swappable_v<T*>);  // Swap
    void clear() noexcept(std::is_nothrow_destructible_v<T>) { destroyRange(begin(), end()); sz = 0; }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args);

    template <class... Args>
    void emplace_back(Args&&... args);
    void resize(const size_type newSize);  // Simple resize
    void resize(const size_type newSize, const_reference fillValue);  // Resize and fill
    void reserve(const size_type reservationSize);
    void shrink_to_fit();

    /*** Size and Capacity Checkers ***/
    NODISCARD bool empty()         const noexcept { return (sz == 0);   }
    NODISCARD size_type size()     const noexcept { return sz;          }
    NODISCARD size_type capacity() const noexcept { return cap;         }
    NODISCARD size_type max_size() const noexcept { return std::allocator_traits<Allocator>::max_size(alloc); }

private:
    /*** Members ***/
    size_type sz    = 0;
    size_type cap   = 0;
    T* data         = nullptr;
    Allocator alloc;

    /*** Helper Functions ***/
    template<class InputIterator>
    void assignRangeForward(InputIterator from, InputIterator to, iterator destination);
    void assignRangeForward(iterator from, iterator to, const_reference value);

    template<class InputIterator>
    void assignRangeBackward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void moveRangeForward(InputIterator from, InputIterator to, iterator destination);
    void moveRangeForward(iterator from, iterator to, const_reference value);

    template<class InputIterator>
    void moveRangeBackward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void copyRangeForward(InputIterator from, InputIterator to, iterator destination);
    void copyRangeForward(iterator from, iterator to, const_reference value);

    template<class InputIterator>
    void copyRangeBackward(InputIterator from, InputIterator to, iterator destination);

    void grow(size_type newCap, bool copy = false, size_type gapIndex = 0, size_type gapSize = 0);

    void destroyRange(iterator from, iterator to);
    void destroyPointer(iterator ptr);
};

// Finds the next power of 2 which is greater than N.
std::size_t nextPowerOf2(std::size_t N)
{
    if(0 == N)
        return 1;

    std::size_t maxValuedBit = 1;

    while(0 != N)
    {
        N = N >> 1;
        maxValuedBit = maxValuedBit << 1;
    }

    return maxValuedBit;
}

/**
 * @brief Default constructor
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector() noexcept
: sz(0), cap(0), data(nullptr)
{ /* Empty constructor */ }

/**
 * @brief Fill constructor
 * @param numberOfElements Initial size of vector
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector(const size_type numberOfElements)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construction will take place at each element insertion
    data = std::allocator_traits<Allocator>::allocate(alloc, cap);

    // Construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index)
        std::allocator_traits<Allocator>::construct(alloc, data + index);
}

/**
 * @brief Fill constructor with a reference fill value
 * @param numberOfElements  Initial size of vector
 * @param fillValue         Reference value for the construction of initial elements
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector(const size_type numberOfElements, const_reference fillValue)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construction will take place at each element insertion
    data = std::allocator_traits<Allocator>::allocate(alloc, cap);

    // Copy construct the elements at predetermined locations
    copyRangeForward(begin(), begin() + numberOfElements, fillValue);
}

/**
 * @brief Range constructor
 * @param first Starting element of source
 * @param last  Last element of source(excluded)
 */
template<class T, class Allocator>
template<class InputIterator>
Vector<T, Allocator>::Vector(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = std::distance(first, last);

    sz      = numberOfElements;
    cap     = nextPowerOf2(sz);

    // Allocate space for incoming elements
    // Construction will take place at each element insertion
    data = std::allocator_traits<Allocator>::allocate(alloc, cap);

    copyRangeForward(first, last, data);
}

/**
 * @brief Copy constructor
 * @param copyVector Vector to be copied from
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector(const Vector& copyVector)
: sz(copyVector.size()), cap(copyVector.capacity()), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = std::allocator_traits<Allocator>::allocate(alloc, cap);

    copyRangeForward(copyVector.begin(), copyVector.end(), begin());
}

/**
 * @brief Move constructor
 * @param moveVector Vector to be used for resource stealing
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector(Vector&& moveVector) noexcept(std::is_nothrow_swappable_v<T*>)
: sz(moveVector.size()), cap(moveVector.capacity()), data(moveVector.data)
{
    moveVector.sz   = 0;
    moveVector.cap  = 0;
    moveVector.data = nullptr;  // Source stolen
}

/**
 * @brief Initializer List constructor
 * @param std::initializerList source list
 */
template<class T, class Allocator>
Vector<T, Allocator>::Vector(std::initializer_list<value_type> initializerList)
: sz(initializerList.size()), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = std::allocator_traits<Allocator>::allocate(alloc, cap);

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        std::allocator_traits<Allocator>::construct(data + index, *sourceIt);
}

/**
 * @brief   Destructor
 * @note    Calls the destructors of each element individually
 */
template<class T, class Allocator>
Vector<T, Allocator>::~Vector() noexcept(std::is_nothrow_destructible_v<T>)
{
    destroyRange(begin(), end());
    destroyPointer(data);

    sz      = 0;
    cap     = 0;
    data    = nullptr;
}

/**
 * @brief   Copy assignment operator
 * @param   copyVector Vector to be copied from
 * @return  lvalue reference to the left vector to support cascaded calls
 * @note    The elements of the left vector will be destroyed
 */
template<class T, class Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& copyVector)
{
    if(this == &copyVector) // Check self assignment
        return *this;

    if(copyVector.size() > capacity()) // Reallocation needed?
    {
        grow(nextPowerOf2(copyVector.size()));
        sz = 0;
    }

    /* Currently used posisitions inside the container are assignable.
     * The rest of them needs construction by copying. */
    if(0 == sz)
        copyRangeForward(copyVector.begin(), copyVector.begin() + copyVector.size(), begin());
    else if(copyVector.size() < sz)
    {
        assignRangeForward(copyVector.begin(), copyVector.begin() + copyVector.size(), begin());
        destroyRange(begin() + copyVector.size(), end());
    }
    else
    {
        assignRangeForward(copyVector.begin(), copyVector.begin() + sz, begin());
        copyRangeForward(copyVector.begin() + sz, copyVector.end(), begin() + sz);
    }

    sz = copyVector.size(); // Adjust size

    return *this;
}

/**
 * @brief   Move assignment operator
 * @param   moveVector Vector to be used for resource swapping
 * @return  lvalue reference to the left vector to support cascaded calls
 * @note    The elements of the left vector will be destroyed
 * @note    The resource of the right vector will be stolen
 */
template<class T, class Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& moveVector) noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_swappable_v<T*>)
{
    if(this == &moveVector)     // Check self assignment
        return *this;

    clear();            // Clear own content
    swap(moveVector);   // Steal resources of move(right) vector

    return *this;
}

/**
 * @brief   Element wise comparison
 * @param   rightVector Vector on the right side of comparison operator
 * @return  true if vectors are equal
 */
template<class T, class Allocator>
bool Vector<T, Allocator>::operator==(const Vector<T, Allocator>& rightVector) const
{
    if(this == &rightVector)            // Self comparison
        return true;

    if(size() != rightVector.size())    // Size must match
        return false;

    for(size_type index = 0; index < sz; ++index)
        if(data[index] != rightVector[index])
            return false;

    return true;
}

/**
 * @brief   Initializer List assignment operator
 * @param   std::initializer_list Source list
 * @return  lvalue reference to the left vector to support cascaded calls
 * @note    The elements of the left vector will be destroyed
 */
template<class T, class Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(std::initializer_list<value_type> initializerList)
{
    if(initializerList.size() > capacity()) // Reallocation needed?
    {
        grow(nextPowerOf2(initializerList.size()));
        sz = 0;
    }

    /* Currently used posisitions inside the container are assignable.
     * The rest of them needs construction by copying. */
    if(0 == sz)
        copyRangeForward(initializerList.begin(), initializerList.begin() + initializerList.size(), begin());
    else if(initializerList.size() < sz)
    {
        assignRangeForward(initializerList.begin(), initializerList.begin() + initializerList.size(), begin());
        destroyRange(begin() + initializerList.size(), end());
    }
    else /* (initializerList.size() > sz) */
    {
        assignRangeForward(initializerList.begin(), initializerList.begin() + sz, begin());
        copyRangeForward(initializerList.begin() + sz, initializerList.end(), begin() + sz);
    }

    sz = initializerList.size(); // Adjust size

    return *this;
}

/**
 * @brief   Element access method
 * @param   position Position of the element to be accessed
 * @return  lvalue reference to the element at index
 * @throws  std::out_of_range if the index is bigger than the size
 */
template<class T, class Allocator>
T& Vector<T, Allocator>::at(const size_type index)
{
    if(index < sz)
        return data[index];

    throw(std::out_of_range("Index is out-of-range!"));
}

/**
 * @brief   Element access method
 * @param   position Position of the element to be accessed
 * @return  const lvalue reference to the element at index
 * @throws  std::out_of_range if the index is bigger than the size
 */
template<class T, class Allocator>
const T& Vector<T, Allocator>::at(const size_type index) const
{
    if(index < sz)
        return data[index];

    throw(std::out_of_range("Index is out-of-range!"));
}

/**
 * @brief   Range assign method
 * @param   first   Source start point
 * @param   last    Source end point
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::assign(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = std::distance(first, last);

    if(size_type(numberOfElements) > capacity())  // Is a bigger space needed?
    {
        grow(nextPowerOf2(numberOfElements));   // Grow and destroy current elements
    }
    else
    {
        destroyRange(begin(), end());   // Destroy elements, preserve allocated space
    }

    copyRangeForward(first, last, data);

    sz = numberOfElements;  // Determine new size
}

/**
 * @brief   Fill assign with predetermined number of elements
 * @param   numberOfElements    Number of elements to be assigned
 * @param   fillValue           Assignment value
 * @throws  std::logic_error    If zero elements wanted to be assigned
 */
template<class T, class Allocator>
void Vector<T, Allocator>::assign(size_type numberOfElements, const_reference fillValue)
{
    if(size_type(numberOfElements) > capacity())  // Is a bigger space needed?
    {
        grow(nextPowerOf2(numberOfElements));   // Grow and destroy current elements
    }
    else
    {
        destroyRange(begin(), end());   // Destroy elements, preserve allocated space
    }

    copyRangeForward(begin(), begin() + numberOfElements, fillValue);
    sz = numberOfElements;
}

/**
 * @brief Initializer list assignment
 * @param initializerList   Source list
 */
template<class T, class Allocator>
void Vector<T, Allocator>::assign(std::initializer_list<T> initializerList)
{
    if(initializerList.size() > capacity())  // Is a bigger space needed?
    {
        grow(nextPowerOf2(initializerList.size()));   // Grow and destroy current elements
    }
    else
    {
        destroyRange(begin(), end());   // Destroy elements, preserve allocated space
    }

    copyRangeForward(initializerList.begin(), initializerList.end(), begin());

    sz = initializerList.size();  // Determine new size
}

/**
 * @brief   Adds a new element at the end of the vector, after its current last element.
 * @param   value   Value to be copied to the new element.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::push_back(const_reference value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
        grow(nextPowerOf2(capacity()), true);   // Grow and copy the old content

    new(data + sz) value_type(value); // Copy construct new element with the incoming one

    sz++;   // Size will not be incremented in case of a previous exception
}

/**
 * @brief   Adds a new element at the end of the vector, after its current last element.
 * @param   value   Value to be moved to the new element.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::push_back(value_type&& value)
{
    emplace_back(std::move(value)); // Construct element at the back by moving
}

/**
 * @brief   Removes the last element in the vector by destroying it.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::pop_back() noexcept(std::is_nothrow_destructible_v<T>)
{
    if(size() > 0)
    {
        --sz;
        (data + sz)->~value_type(); // Destroy popped element
    }
}

/**
 * @brief   Range insertion method
 * @param   position Position in the vector where the new elements are inserted.
 * @param   first   Iterator specifying the starting point of elements.
 * @param   last    Iterator specifying the ending point of elements.   (excluded)
 * @return  An iterator that points to the first of the newly inserted elements.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 * @throws  std::logic_error        If the iterators are in a wrong order
 */
template<class T, class Allocator>
template <class InputIterator>
T* Vector<T, Allocator>::insert(iterator position, InputIterator first, InputIterator last)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    const difference_type numberOfElements    = std::distance(first, last);
    const difference_type positionAsIndex     = std::distance(begin(), position);

    if(numberOfElements <= 0)
        throw(std::logic_error("Wrong iterator sequence!"));

    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* If there will be reallocations for multiple times,
         * we better reallocate for the final size in a single operation
         * to enhance optimization of insertion. */
        grow(nextPowerOf2(size() + numberOfElements), true, positionAsIndex, numberOfElements);

        copyRangeForward(first, last, data + positionAsIndex);  // Copy construct new elements at given position
    }
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - numberOfElements, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - numberOfElements, position + numberOfElements);

        assignRangeForward(first, last, data + positionAsIndex);  // Copy assign new elements to the given position
    }

    sz += numberOfElements;

    return (data + positionAsIndex);
}

/**
 * @brief   Fill insertion method
 * @param   position            Position in the vector where the new elements are inserted.
 * @param   numberOfElements    Number of elements to be filled
 * @param   value               Value to be copied to the inserted element
 * @return  An iterator that points to the first of the newly inserted elements.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 * @throws  std::invalid_argument   If the number of elements to be inserted is equal to zero.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::insert(iterator position, size_type numberOfElements, const_reference value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(numberOfElements == 0)
        throw(std::invalid_argument("At least one element must be inserted!"));

    difference_type positionAsIndex = std::distance(begin(), position);

    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* If there will be reallocations for multiple times,
         * we better reallocate for the final size in a single operation
         * to enhance optimization of insertion. */
        grow(nextPowerOf2(size() + numberOfElements), true, positionAsIndex, numberOfElements);

        copyRangeForward(begin() + positionAsIndex, begin() + positionAsIndex + numberOfElements, value);  // Copy construct new elements at given position
    }
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - numberOfElements, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - numberOfElements, position + numberOfElements);

        assignRangeForward(begin() + positionAsIndex, begin() + positionAsIndex + numberOfElements, value);  // Copy assign new elements to the given position
    }

    sz += numberOfElements;

    return (data + positionAsIndex);
}

/**
 * @brief   Copy insertion method
 * @param   position    Position in the vector where the new elements are inserted.
 * @param   value       Value to be copied to the inserted element.
 * @return  An iterator that points to the newly inserted element.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::insert(iterator position, const_reference value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() may be changed
    }

    const size_type positionAsIndex = size_type(std::distance(begin(), position));

    if(size() == capacity())    // Is bigger space needed?
    {
        grow(nextPowerOf2(capacity()), true, positionAsIndex, 1);

        new(data + positionAsIndex) value_type(value);
    }
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);

        *(data + positionAsIndex) = value;
    }

    ++sz;   // Increment size by one

    return (data + positionAsIndex);  // Data may be changed
}

/**
 * @brief   Move insertion method
 * @param   position    Position in the vector where the new elements are inserted.
 * @param   value       Value to be moved to the inserted element.
 * @return  An iterator that points to the newly inserted element.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::insert(iterator position, value_type&& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(std::move(value));

        return (end() - 1);     // end() may be changed
    }

    const size_type positionAsIndex = std::distance(begin(), position);

    if(size() == capacity())    // Is bigger space needed?
    {
        grow(nextPowerOf2(capacity()), true, positionAsIndex, 1);

        new(data + positionAsIndex) value_type(std::move(value));
    }
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);

        *(data + positionAsIndex) = std::move(value);
    }

    ++sz;   // Increment size by one

    return (data + positionAsIndex);  // Data may be changed
}

/**
 * @brief   Initializer list insertion method
 * @param   position        Position in the vector where the new elements are inserted.
 * @param   initializerList Source list
 * @return  An iterator that points to the newly inserted element.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::insert(iterator position, std::initializer_list<value_type> initializerList)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    return insert(position, initializerList.begin(), initializerList.end());
}

/**
 * @brief   Erases the element from the given position by destroing it.
 * @param   position    Position in the vector where the elements to be inserted relies.
 * @return  An iterator pointing to the element that follows the erased element.
 * @throws  std::invalid_argument   If the given position does not rely inside the container.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::erase(iterator position)
{
    if((position < begin()) || (position >= end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == (end() - 1))
    {
        pop_back();

        return position;
    }

    assignRangeForward(position + 1, end(), position);  // Shift left the elements on the right
    destroyRange(end() - 1, end());                     // Destroy outsider elements
    --sz;   // Decrement size

    return position;
}

/**
 * @brief   Range erase method
 * @param   first   Position where the erasing begins from
 * @param   last    Position where the erasing ends at(excluded)
 * @return  An iterator pointing to the element that follows the erased elements.
 * @throws  std::invalid_argument   If the given positions does not rely inside the container.
 * @throws  std::invalid_argument   If the given positions doesn't have valid order.
 */
template<class T, class Allocator>
T* Vector<T, Allocator>::erase(iterator first, iterator last)
{
    if((first < begin()) || (last > end()))
        throw(std::invalid_argument("Iterators must rely inside the container!"));

    if(last == first)
        throw(std::invalid_argument("Invalid iterator sequence!"));

    const difference_type distance = std::distance(first, last);

    assignRangeForward(first + distance, end(), first); // Shift left the elements on the right
    destroyRange(end() - distance, end());
    sz -= distance;

    return first;
}

/**
 * @brief   Swaps the contents of two vectors
 * @param   swapVector  Vector to be swapped with
 */
template<class T, class Allocator>
void Vector<T, Allocator>::swap(Vector& swapVector) noexcept(std::is_nothrow_swappable_v<T*>)
{
    if(this == &swapVector) // Check self swap
        return;

    value_type* tempData;
    size_type tempSzCap;

    tempData        = data;
    data            = swapVector.data;
    swapVector.data = tempData;

    tempSzCap       = sz;
    sz              = swapVector.size();
    swapVector.sz   = tempSzCap;

    tempSzCap       = cap;
    cap             = swapVector.capacity();
    swapVector.cap  = tempSzCap;
}

/**
 * @brief   Constructs and inserts element at given position
 * @param   position    Position in the vector where the new elements are inserted.
 * @param   args        Arguments to be used when constructing the new element
 * @return  An iterator that points to the newly inserted element.
 */
template<class T, class Allocator>
template <class... Args>
T* Vector<T, Allocator>::emplace(iterator position, Args&&... args)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    const size_type positionAsIndex = size_type(std::distance(begin(), position));

    if(size() == capacity())    // Is bigger space needed?
    {
        grow(nextPowerOf2(capacity()), true, positionAsIndex, 1);

        std::allocator_traits<Allocator>::construct(alloc, data + positionAsIndex, std::forward<Args>(args)...);
    }
    else
    {
        /* If there wasn't a reallocation, then move constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);

        std::allocator_traits<Allocator>::destroy(alloc, position); // Destroy old element

        // Construct new element with the given arguments
        std::allocator_traits<Allocator>::construct(alloc, position, std::forward<Args>(args)...);
    }

    sz++;

    return (data + positionAsIndex);  // Data may be changed
}

/**
 * @brief   Constructs and inserts element at the end of the container.
 * @param   args    Arguments to be used when constructing the new element
 */
template<class T, class Allocator>
template <class... Args>
void Vector<T, Allocator>::emplace_back(Args&&... args)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        grow(nextPowerOf2(capacity()), true);
    }

    // Copy construct new element with the incoming one
    std::allocator_traits<Allocator>::construct(alloc, data + sz, std::forward<Args>(args)...);

    ++sz;   // Size will not be incremented in case of a previous exception
}

/**
 * @brief   Resizes the container so that it contains given number of elements.
 * @param   newSize Requested size.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::resize(const size_type newSize)
{
    if(0 == newSize)
        return clear();
    else if(size() == newSize)
        return;
    else { }

    if(newSize < size())
    {
        destroyRange(begin() + newSize, end());
    }
    else
    {
        if(newSize < capacity())
        {
            for(size_type index = size(); index < newSize; ++index)
                std::allocator_traits<Allocator>::construct(alloc, data + index);
        }
        else    // Reallocation needed
        {
            grow(nextPowerOf2(newSize), true);  // Grow and preserve the content

            for(size_type index = size(); index < newSize; ++index)
                std::allocator_traits<Allocator>::construct(alloc, data + index);
        }
    }

    sz = newSize;
}

/**
 * @brief   Resizes the container so that it contains given number of elements.
 * @param   newSize     Requested size.
 * @param   fillValue   Value to be copied into newly added elements.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::resize(const size_type newSize, const_reference fillValue)
{
    if(0 == newSize)
        return clear();
    else if(size() == newSize)
        return;
    else { }

    if(newSize < size())
    {
        destroyRange(begin() + newSize, end());
    }
    else
    {
        if(newSize < capacity())
        {
            for(size_type index = size(); index < newSize; ++index)
                std::allocator_traits<Allocator>::construct(alloc, data + index, fillValue);
        }
        else    // Reallocation needed
        {
            grow(nextPowerOf2(newSize), true);  // Grow and preserve the content

            for(size_type index = size(); index < newSize; ++index)
                std::allocator_traits<Allocator>::construct(alloc, data + index, fillValue);
        }
    }

    sz = newSize;
}

/**
 * @brief   Requests that the vector capacity be at least enough to contain requested number of elements.
 * @param   reservationSize     Minimum capacity for the vector.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::reserve(const size_type reservationSize)
{
    if(reservationSize <= capacity())
        return;

    return grow(reservationSize, true);
}

/**
 * @brief   Requests the container to reduce its capacity to fit its size
 * @note    Caueses reallocation if the current size is not equal to the current capacity.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::shrink_to_fit()
{
    if(size() == capacity())
        return;

    cap = size();
    value_type* newData = std::allocator_traits<Allocator>::allocate(alloc, cap);

    copyRangeForward(begin(), end(), newData);

    destroyRange(begin(), end());
    destroyPointer(data);

    data = newData;
}

/**
 * @brief   Helper method for assigning ranges in a forward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Do not use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::assignRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        *destination = *from;
}

/**
 * @brief   Helper method for assigning value to a range of elements in a forward order.
 * @param   from    Starting point of destination range
 * @param   to      Ending point of destination range(excluded)
 * @param   value   Value to be copied to the elements in the destination range.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::assignRangeForward(iterator from, iterator to, const_reference value)
{
    for( ; from != to; ++from)
        *from = value;
}

/**
 * @brief   Helper method for assigning ranges in a forward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::assignRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward assigning will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        *destination = *(to - 1);   // Copy assign element
}

/**
 * @brief   Helper method for moving ranges in a forward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Do not use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::moveRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        std::allocator_traits<Allocator>::construct(alloc, destination, std::move(*from));
}

/**
 * @brief   Helper method for move assigning value to a range of elements in a forward order.
 * @param   from    Starting point of destination range
 * @param   to      Ending point of destination range(excluded)
 * @param   value   Value to be move assigned to the elements in the destination range.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::moveRangeForward(iterator from, iterator to, const_reference value)
{
    for( ; from != to; ++from)
        *from = std::move(value);
}

/**
 * @brief   Helper method for moving ranges in a backward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::moveRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward copying will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        std::allocator_traits<Allocator>::construct(alloc, destination, std::move(*(to - 1)));
}

/**
 * @brief   Helper method for copying ranges in a forward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Do not use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::copyRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        std::allocator_traits<Allocator>::construct(alloc, destination, *from);
}

/**
 * @brief   Helper method for copy assigning value to a range of elements in a forward order.
 * @param   from    Starting point of destination range
 * @param   to      Ending point of destination range(excluded)
 * @param   value   Value to be copy assigned to the elements in the destination range.
 */
template<class T, class Allocator>
void Vector<T, Allocator>::copyRangeForward(iterator from, iterator to, const_reference value)
{
    for( ; from != to; ++from)
        std::allocator_traits<Allocator>::construct(alloc, from, value);
}

/**
 * @brief   Helper method for copying ranges in a backward order.
 * @param   from            Starting point of source range
 * @param   to              Ending point of source range(excluded)
 * @param   destination     Starting point of destination range
 * @note    Use if the ranges overlaps each other
 */
template<class T, class Allocator>
template<class InputIterator>
void Vector<T, Allocator>::copyRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward copying will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        std::allocator_traits<Allocator>::construct(alloc, destination, *(to - 1));
}

/**
 * @brief   Grows the size of the vector either by copying the content or not
 * @param   newCap      Requested capacity
 * @param   copy        Copy the content or not
 * @param   gapIndex    Starting index of the gap if needed
 * @param   gapSize     Size of the requested gap
 * @throw   std::invalid_argument   If the requested capacity is smaller than potential capacity
 * @throw   std::invalid_argument   If the gap index is outside of the container
 */
template<class T, class Allocator>
void Vector<T, Allocator>::grow(size_type newCap, bool copy, size_type gapIndex, size_type gapSize)
{
    if(newCap < cap + gapSize)
        throw std::invalid_argument("Cannot grow to a smaller capacity!");

    if(gapIndex > size())
        throw std::invalid_argument("Cannot create gap outside of the container!");

    // Allocate new space
    cap = newCap;
    value_type* newData = std::allocator_traits<Allocator>::allocate(alloc, cap);

    if(true == copy)    // If copying the old items is needed
    {
        if(gapSize > 0) // Is a gap needed?
        {
            copyRangeForward(begin(),               begin() + gapIndex, newData);                       // Copy items on the left side of the gap
            copyRangeForward(begin() + gapIndex,    end(),              newData + gapIndex + gapSize);  // Copy items on the right side of the gap
        }
        else
        {
            copyRangeForward(begin(), end(), newData);
        }
    }

    destroyRange(begin(), end());
    destroyPointer(data);

    data = newData;
}

template<class T, class Allocator>
void Vector<T, Allocator>::destroyRange(iterator from, iterator to)
{
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for( ; from != to; ++from)
        std::allocator_traits<Allocator>::destroy(alloc, from);
}

template<class T, class Allocator>
void Vector<T, Allocator>::destroyPointer(iterator ptr)
{
    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    std::allocator_traits<Allocator>::deallocate(alloc, ptr, sz);
}

template<class T, class Allocator>
std::ostream& operator<<(std::ostream& stream, const Vector<T, Allocator>& vector)
{
    for(const T& element : vector)
        stream << element << " ";

    return stream;
}

#endif
