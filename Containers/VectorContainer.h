/**
 * @file        VectorContainer.h
 * @details     A template vector container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        March 30, 2021      -> First release
 *              March 31, 2021      -> Single value insert(..) added.
 *              April 1, 2021       ->
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright.
 */

/*** Recursive inclusion preventer ***/
#ifndef VECTOR_CONTAINER_H
#define VECTOR_CONTAINER_H

/*** Libraries ***/
#include <cstddef>              // For std::size_t, ptrdiff_t
#include <initializer_list>     // For std::initializer_list
#include <stdexcept>            // For exceptions
#include <utility>              // For std::move
#include <new>                  // For ::operator new
#include <ostream>              // For std::cout

/*** Container Class ***/
template<class T>
class Vector{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    typedef T                value_type;
    typedef T&               reference;
    typedef const T&         const_reference;
    typedef T*               iterator;
    typedef const T*         const_iterator;
    typedef std::ptrdiff_t   difference_type;
    typedef std::size_t      size_type;

    /*** Constructors and Destructors ***/
    Vector();                                                               // Default constructor
    Vector(const size_type numberOfElements);                               // Fill constructor
    Vector(const size_type numberOfElements, const value_type& fillValue);  // Fill construcotr with fill value

    template<class InputIterator>
    Vector(InputIterator first, InputIterator last);                        // Range constructor
    Vector(const Vector& copyVector);                                       // Copy constructor
    Vector(Vector&& moveVector);                                            // Move constructor
    Vector(std::initializer_list<value_type> initializerList);              // Initializer list constructor

    ~Vector();  // Destructor

    /*** Operator Overloadings ***/
    Vector& operator=(const Vector& copyVector);                            // Copy assignment operator
    Vector& operator=(Vector&& moveVector);                                 // Move assignment operator
    Vector& operator=(std::initializer_list<value_type> initializerList);   // Initializer list assignment operator

    reference       operator[](const size_type position)        { return data[position]; }  // Element access by lValue
    const_reference operator[](const size_type position) const  { return data[position]; }  // Element access by const lValue

    /*** Element Access ***/
    reference       at(const size_type position);           // Random access with range check
    const_reference at(const size_type position) const;     // Random access with range check

    reference       front()         { return data[0]; }     // Access to the first element
    const_reference front() const   { return data[0]; }     // Access to the first element

    reference       back()          { return data[sz - 1]; }    // Access to the last element
    const_reference back() const    { return data[sz - 1]; }    // Access to the last element

    /*** Iterators ***/
    iterator begin()                { return data;      }   // Iterator starting from the first element
    iterator end()                  { return data + sz; }   // Iterator starting from the next of the last element
    const_iterator begin()  const   { return data;      }   // Iterator starting from the first element
    const_iterator end()    const   { return data + sz; }   // Iterator starting from the next of the last element
    const_iterator cbegin() const   { return data;      }   // Iterator starting from the first element
    const_iterator cend()   const   { return data + sz; }   // Iterator starting from the next of the last element

    /*** Modifiers ***/
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last);                   // Range assign
    void assign(size_type numberOfElements, const value_type& fillValue);   // Fill assign
    void assign(std::initializer_list<value_type> initializerList);         // Initializer list assign

    void push_back(const value_type& value);    // Push element next to the last element
    void push_back(value_type&& value);         // Push element next to the last element by moving

    void pop_back();    // Remove last element

    template <class InputIterator>
    iterator insert(iterator position, InputIterator first, InputIterator last);                // Range insertion
    iterator insert(iterator position, const value_type& value);                                // Single element insertion
    iterator insert(iterator position, size_type numberOfElements, const value_type& value);    // Multiple insertion and fill
    iterator insert(iterator position, value_type&& value);                                     // Move insertion
    iterator insert(iterator position, std::initializer_list<value_type> il);                   // Initializer list insertion

    iterator erase(iterator position);              // Single element erase
    iterator erase(iterator first, iterator last);  // Iterator based multiple erase

    void swap(Vector& swapVector);  // Swap
    void clear() noexcept { sz = 0; }

    template <class... Args>
    iterator emplace(iterator position, Args&&... args);

    void resize(const size_type newSize);                               // Simple resize
    void resize(const size_type newSize, const value_type& fillValue);  // Resize and fill
    void reserve(const size_type reservationSize);
    void shrink_to_fit();

    /*** Size and Capacity Checkers ***/
    size_type size()        const { return sz;          }
    size_type capacity()    const { return cap;         }
    // size_type max_size() const {                     }        // TODO Search for implementation
    bool empty()            const { return (sz == 0);   }

private:
    /*** Members ***/
    size_type sz    = 0;
    size_type cap   = 0;
    T* data         = nullptr;

    /*** Helper Functions ***/
    template<class InputIterator>
    void assignRangeForward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void assignRangeBackward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void moveRangeForward(InputIterator from, InputIterator to, iterator destination);

    template<class InputIterator>
    void moveRangeBackward(InputIterator from, InputIterator to, iterator destination);

    void destroyRange(iterator from, iterator to);
    void destroyPointer(iterator ptr);
};

// Finds the next power of 2 which is greater than N.
std::size_t nextPowerOf2(std::size_t N)
{
    if(N == 0)
        return 1;

    std::size_t maxValuedBit = 1;

    while(N != 0)
    {
        N = N >> 1;
        maxValuedBit = maxValuedBit << 1;
    }

    return maxValuedBit;
}

template<class T>
Vector<T>::Vector() : sz(0), cap(0), data(nullptr)
{ /* Empty constructor */ }

template<class T>
Vector<T>::Vector(const size_type numberOfElements)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    // Construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index)
        new(data + index) value_type;
}

template<class T>
Vector<T>::Vector(const size_type numberOfElements, const value_type& fillValue)
: sz(numberOfElements), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    // Construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index)
        new(data + index) value_type(fillValue);
}

template<class T>
template<class InputIterator>
Vector<T>::Vector(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = last - first;

    if(numberOfElements > 0)
    {
        sz      = numberOfElements;
        cap     = nextPowerOf2(sz);

        // Allocate space for incoming elements
        // Construct will take place at each element insertion
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; (index < cap) && (first != last); ++index, ++first)
            new(data + index) value_type(*first);
    }
    else
        throw(std::logic_error("Wrong iterator sequence!"));
}

template<class T>
Vector<T>::Vector(const Vector& copyVector)
: sz(copyVector.size()), cap(copyVector.capacity()), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data    = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    const_iterator sourceIt = copyVector.cbegin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);

}

template<class T>
Vector<T>::Vector(Vector&& moveVector)
: sz(moveVector.size()), cap(moveVector.capacity()), data(moveVector.data)
{
    moveVector.sz   = 0;
    moveVector.cap  = 0;
    moveVector.data = nullptr;  // Source stolen
}

template<class T>
Vector<T>::Vector(std::initializer_list<value_type> initializerList)
: sz(initializerList.size()), cap(nextPowerOf2(sz)), data(nullptr)
{
    // Allocate space for incoming elements
    // Construct will take place at each element insertion
    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);
}

template<class T>
Vector<T>::~Vector()
{
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(data));

    sz  = 0;
    cap = 0;
}

template<class T>
Vector<T>& Vector<T>::operator=(const Vector& copyVector)
{
    if(this == &copyVector) // Check self assignment
        return *this;

    // Destroy resource of the left vector

    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(data));

    // Allocate space for incoming elements
    sz      = copyVector.size();
    cap     = copyVector.capacity();

    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    // Copy construct the elements at predetermined locations
    const_iterator sourceIt = copyVector.cbegin();

    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(Vector&& moveVector)
{
    if(this == &moveVector) // Check self assignment
        return *this;

    // Destroy resource of the left vector
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(data));

    // Steal resources of move(right) vector
    sz      = moveVector.size();
    cap     = moveVector.cap;
    data    = moveVector.data;

    // Prevent destruction of stolen resource
    moveVector.sz   = 0;
    moveVector.cap  = 0;
    moveVector.data = nullptr;

    return *this;
}

template<class T>
Vector<T>& Vector<T>::operator=(std::initializer_list<value_type> initializerList)
{
    // Destroy resource of the left vector
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(data));

    // Adjust new resource
    sz      = initializerList.size();
    cap     = nextPowerOf2(sz);

    if(cap > 0)
        data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);

    return *this;
}

template<class T>
T& Vector<T>::at(const size_type position)
{
    if(position < sz)
        return data[position];

    throw(std::out_of_range("Index is out-of-range!"));
}

template<class T>
const T& Vector<T>::at(const size_type position) const
{
    if(position < sz)
        return data[position];

    throw(std::out_of_range("Index is out-of-range!"));
}

template<class T>
template<class InputIterator>
void Vector<T>::assign(InputIterator first, InputIterator last)
{
    const difference_type numberOfElements = last - first;

    if(numberOfElements > 0)
    {        
        // Destroy the elements of the vector
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        sz = numberOfElements;  // Determine new size

        if(numberOfElements > capacity())  // Is a bigger space needed?
        {
            /* The allocated space will not be used anymore.
             * We shall release the resource for further usage */
            ::operator delete(static_cast<void*>(data));

            cap = nextPowerOf2(numberOfElements);

            // Allocate new resource
            if(cap > 0)
                data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
        }

        // Copy construct the elements at predetermined locations
        for(size_type index = 0; (index < cap) && (first != last); ++index, ++first)
            new(data + index) value_type(*first);
    }
    else
        throw(std::logic_error("Wrong iterator sequence!"));
}

template<class T>
void Vector<T>::assign(size_type numberOfElements, const value_type& fillValue)
{
    if(numberOfElements > 0)
    {
        // Destroy the elements of the vector
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        sz = numberOfElements;  // Determine new size

        if(numberOfElements > capacity())  // Is a bigger space needed?
        {
            /* The allocated space will not be used anymore.
             * We shall release the resource for further usage */
            ::operator delete(static_cast<void*>(data));

            cap = nextPowerOf2(numberOfElements);

            // Allocate new resource
            if(cap > 0)
                data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
        }

        for(size_type index = 0; index < size_type(numberOfElements); ++index)
            new(data + index) value_type(fillValue);
    }
    else
        throw(std::invalid_argument("Assignment size error!"));
}

template<class T>
void Vector<T>::assign(std::initializer_list<T> initializerList)
{
    // Destroy the elements of the vector
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for(size_type index = 0; index < sz; ++index)
        (data + index)->~value_type();

    sz = initializerList.size();  // Determine new size

    if(initializerList.size() > capacity())  // Is a bigger space needed?
    {
        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        cap = nextPowerOf2(initializerList.size());

        // Allocate new resource
        if(cap > 0)
            data = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));
    }

    const_iterator sourceIt = initializerList.begin();

    // Copy construct the elements at predetermined locations
    for(size_type index = 0; index < sz; ++index, ++sourceIt)
        new(data + index) value_type(*sourceIt);
}

template<class T>
void Vector<T>::push_back(const value_type& value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; index < size(); ++index)
            new(newData + index) value_type(*(data + index));   // Copy construct the new elements

        // Destroy older ones
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        data = newData;
    }

    // Copy construct new element with the incoming
    new(data + sz++) value_type(value);
}

template<class T>
void Vector<T>::push_back(value_type&& value)
{
    if(size() == capacity())    // Size is about to surpass the capacity
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; index < size(); ++index)
            new(newData + index) value_type(*(data + index));   // Copy construct the new elements

        // Destroy older ones
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        data = newData;
    }

    // Move construct new element with the incoming
    new(data + sz++) value_type(std::move(value));
}

template<class T>
void Vector<T>::pop_back()
{
    if(size() > 0)
    {
        --sz;
        (data + sz)->~value_type(); // Destroy popped element
    }
}

template<class T>
template <class InputIterator>
T* Vector<T>::insert(iterator position, InputIterator first, InputIterator last)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    difference_type numberOfElements    = last - first;
    difference_type positionAsIndex     = position - begin();

    if(numberOfElements <= 0)
        throw(std::logic_error("Wrong iterator sequence!"));

    value_type* newData = nullptr;  // A reallocation may be needed

    /* If there will be reallocations for multiple times,
     * we better reallocate for the final size in a single operation
     * to enhance optimization of insertion. */
    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(size() + numberOfElements);
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        moveRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        moveRangeForward(position, end(), newData + positionAsIndex + numberOfElements);   // Gap for the incoming ones
    else
    {
        /* If there wasn't a reallocation, then copy constructing the elements
         * that will locate after the current size is enough. The remaining
         * ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - numberOfElements, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - numberOfElements, position + numberOfElements);
    }

    if(newData != nullptr)  // If a reallocation happened
        moveRangeForward(first, last, newData + positionAsIndex);   // Copy construct new elements at given position
    else
        assignRangeForward(first, last, position);  // Copy assign new elements at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz += numberOfElements;

    return (data + positionAsIndex);
}

template<class T>
T* Vector<T>::insert(iterator position, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() may be changed
    }

    const size_type positionAsIndex = size_type(position - begin());
    value_type* newData = nullptr;  // A reallocation may be needed

    if(size() == capacity())    // Reallocation needed
    {
        // New space for incoming data
        cap = nextPowerOf2(capacity()); // Capacity changed
        newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        // Move the elements on the left side
        moveRangeForward(begin(), position, newData);   // Copy construct existing elements at their new place
    }

    // Move the elements on the right side
    if(newData != nullptr)  // If a reallocation happened
        moveRangeForward(position, end(), newData + positionAsIndex + 1);   // 1 element gap for the incoming one
    else
    {
        /* If there wasn't a reallocation, then copy constructing the last element is enough.
         * The remaining ones should only be copy assigned(right shifted). */
        moveRangeForward(end() - 1, end(), end());

        // Copy assign the remaining ones
        assignRangeBackward(position, end() - 1, position + 1);
    }

    if(newData != nullptr)  // If a reallocation happened
        new(newData + positionAsIndex) value_type(value);   // Copy construct new element at given position
    else
        *(position) = value;    // Copy assign new element at given position

    if(newData != nullptr)  // Change data pointer if a reallocation happened
    {
        destroyRange(begin(), end());   // Explicitly destroy range
        destroyPointer(data);           // Destroy data pointer

        data = newData;                 // Assign new pointer
    }

    sz++;

    return (data + positionAsIndex);  // Data may be changed
}

template<class T>
T* Vector<T>::insert(iterator position, size_type numberOfElements, const value_type& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(numberOfElements == 0)
        throw(std::invalid_argument("At least one element must be inserted!"));

    difference_type positionAsIndex     = position - begin();

    /* If there will be reallocations for multiple times,
     * we better reallocate for the final size in a single operation
     * to enhance optimization of insertion. */
    if(size() + numberOfElements > capacity())    // Is bigger space needed?
    {
        /* A bigger place needed for current and incoming elements */
        cap = nextPowerOf2(size() + numberOfElements);
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; index < size(); ++index)
            new(newData + index) value_type(*(data + index));   // Copy construct the new elements

        // Destroy older ones
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        data = newData;
    }

    // Shift right elements after given position
    for(size_type index = sz + numberOfElements - 1; index != size_type(positionAsIndex) + size_type(numberOfElements) - 1; --index)
        new(data + index) value_type(*(data + index - numberOfElements));

    // Insert incoming elements
    for(size_type index = 0; index < size_type(numberOfElements); ++index)
        new(data + positionAsIndex + index) value_type(value);

    sz += numberOfElements; // Adjust size

    return position;
}

template<class T>
T* Vector<T>::insert(iterator position, value_type&& value)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == end())
    {
        push_back(value);

        return (end() - 1);     // end() may be changed
    }

    // Move objects after the position
    if(size() == capacity())    // Allocation needed
    {
        cap = nextPowerOf2(capacity());
        value_type* newData = static_cast<value_type*>(::operator new(sizeof(value_type) * cap));

        for(size_type index = 0; begin() + index != position; ++index)      // Move elements before position
            new(newData + index) value_type(*(data + index));               // Copy construct the new elements

        // Place new value
        iterator newPosition = newData + (position - begin());              // Save data position for function return
        new(newData + (position - begin())) value_type(std::move(value));   // Copy construct the insertion element

        for(size_type index = position - begin() + 1; begin() + index != end(); ++index)    // Move elements after position
            new(newData + index) value_type(*(data + index));                               // Copy construct the new elements

        // Destroy older ones
        /* The operator delete[] wouldn't work appropriately as we
         * used the placement new operator and constructed each element
         * at the time of the insertion*/
        for(size_type index = 0; index < sz; ++index)
            (data + index)->~value_type();

        /* The allocated space will not be used anymore.
         * We shall release the resource for further usage */
        ::operator delete(static_cast<void*>(data));

        data = newData; // Replace data

        ++sz;   // Increase size

        return newPosition;
    }
    else    // Allocation not needed
    {
        // Shift right elements after given position
        for(size_type index = sz; index != size_type(position - begin()); --index)
            new(data + index) value_type(*(data + index - 1));

        new(position) value_type(std::move(value)); // Construct element at position
        ++sz;   // Increase size

        return position;
    }
}

template<class T>
T* Vector<T>::insert(iterator position, std::initializer_list<value_type> initializerList)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    return insert(position, initializerList.begin(), initializerList.end());
}

template<class T>
T* Vector<T>::erase(iterator position)
{
    if((position < begin()) || (position >= end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    if(position == (end() - 1))
    {
        pop_back();

        return position;
    }

    for(iterator it = position; it != end(); ++it)  // Shift left each element
        *it = *(it + 1);

    --sz;
    return position;
}

template<class T>
T* Vector<T>::erase(iterator first, iterator last)
{
    if((first < begin()) || (last > end()))
        throw(std::invalid_argument("Iterators must rely inside the container!"));

    const difference_type distance = last - first;

    if(distance > 0)
    {
        const difference_type numberOfMovingElements = end() - last;

        for(size_type index = 0; index < size_type(numberOfMovingElements); ++index)
            *(first + index) = *(last + index);

        sz -= size_type(distance);

        return first;
    }
    else
        throw(std::invalid_argument("Invalid iterator sequence!"));
}

template<class T>
void Vector<T>::swap(Vector& swapVector)
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

template<class T>
template <class... Args>
T* Vector<T>::emplace(iterator position, Args&&... args)
{
    if((position < begin()) || (position > end()))
        throw(std::invalid_argument("Position must rely inside the container!"));

    // Move objects after the position
    if(size() == capacity())    // Allocation needed
    {
        cap = nextPowerOf2(capacity());
        value_type* newData = new value_type[cap];

        size_type newIndex = 0;
        for(iterator it = begin(); it != position; ++it)    // Move elements before position
            newData[newIndex++] = *it;

        // Place new value
        iterator newPosition = newData + newIndex;  // Save data position for function return
        newData[newIndex++] = T(args...);

        for(iterator it = position; it != end(); ++it)  // Move elements after position
            newData[newIndex++] = *it;

        delete [] data; // Destroy previous data
        data = newData; // Replace data

        ++sz;   // Increase size
        return newPosition;
    }
    else    // Allocation not needed
    {
        for(iterator it = end(); it != position; --it)  // Move elements after position
            *it = *(it - 1);

        *position = T(args...);  // Insert element
        ++sz;       // Increase size

        return position;
    }
}

template<class T>
void Vector<T>::resize(const size_type newSize)
{
    if(newSize == capacity())
        return;

    value_type* newArea = new value_type[newSize];

    for(size_type index = 0; (index < newSize) && (index < size()); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = newSize;
    sz      = (newSize < sz) ? newSize : sz;
    data    = newArea;
}

template<class T>
void Vector<T>::resize(const size_type newSize, const value_type& fillValue)
{
    if(newSize == capacity())
        return;

    value_type* newArea = new value_type[newSize];

    size_type index = 0;
    for(index = 0; (index < newSize) && (index < size()); ++index)
        newArea[index] = data[index];

    for( ; index < newSize; ++index)
        newArea[index] = fillValue;

    delete [] data; // Destroy old resource

    cap     = newSize;
    sz      = (newSize < sz) ? newSize : sz;
    data    = newArea;
}

template<class T>
void Vector<T>::reserve(const size_type reservationSize)
{
    if(reservationSize <= capacity())
        return;

    value_type* newArea = new value_type[reservationSize];

    for(size_type index = 0; index < size(); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = reservationSize;
    data    = newArea;
}

template<class T>
void Vector<T>::shrink_to_fit()
{
    if((sz == 0) || (sz == cap))
        return;

    value_type* newArea = new value_type[sz];

    for(size_type index = 0; index < size(); ++index)
        newArea[index] = data[index];

    delete [] data; // Destroy old resource

    cap     = sz;
    data    = newArea;
}

template<class T>
template<class InputIterator>
void Vector<T>::assignRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        *destination = *from;
}

template<class T>
template<class InputIterator>
void Vector<T>::assignRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward assigning will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        *destination = *(to - 1);   // Copy assign element
}

template<class T>
template<class InputIterator>
void Vector<T>::moveRangeForward(InputIterator from, InputIterator to, iterator destination)
{
    for( ; from != to; ++from, ++destination)
        new(destination) value_type(std::move(*from));
}

template<class T>
template<class InputIterator>
void Vector<T>::moveRangeBackward(InputIterator from, InputIterator to, iterator destination)
{
    /* Backward copying will help to prevent corruption of data
     * when two ranges overlap each other. */
    destination += to - from - 1;   // Start from the last

    for( ; to != from; --to, --destination)
        new(destination) value_type(std::move(*(to - 1)));
}

template<class T>
void Vector<T>::destroyRange(iterator from, iterator to)
{
    /* The operator delete[] wouldn't work appropriately as we
     * used the placement new operator and constructed each element
     * at the time of the insertion*/
    for( ; from != to; ++from)
        from->~value_type();    // Explicitly call the destructor
}

template<class T>
void Vector<T>::destroyPointer(iterator ptr)
{
    /* The allocated space will not be used anymore.
     * We shall release the resource for further usage */
    ::operator delete(static_cast<void*>(ptr));
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const Vector<T>& vector)
{
    for(const T& element : vector)
        stream << element << " ";

    return stream;
}

#endif
