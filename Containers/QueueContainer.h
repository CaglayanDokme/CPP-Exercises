/**
 * @file        QueueContainer.h
 * @details     A template queue container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 6, 2021        -> First release
 *              July 8, 2021        -> Copy and move constructors added.
 *              July 9, 2021        -> Doxygen added.
 *              July 23, 2021       -> Helper functions enhanced.
 *                                  -> Comparison operators implemented.
 *              September 19, 2021  -> Assignment operator implemented.
 *                                  -> flush() method added.
 *              October 2, 2021     -> [[no_unique_address]] attribute added for allocator members.
 *                                  -> Using directives enhanced.
 *                                  -> A new allocator created for chunk pointers by rebinding the data allocator.
 *                                  -> Constructors simplified using the default member initialization.
 *                                  -> select_on_container_copy_construction(..) added to copy constructor.
 *                                  -> lvalue return type added to modifier functions to supoort cascaded calls.
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <memory>       // std::allocator, std::allocator_traits
#include <cstring>      // std::memcpy
#include <algorithm>    // std::swap
#include <cstddef>      // std::size_t

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#if __cplusplus >= 202002L
#define NO_UNIQUE_ADDR [[no_unique_address]]
#else
#define NO_UNIQUE_ADDR
#endif  // C++20 check
#else
#define NODISCARD
#endif  // C++17 check

/*** Container Class ***/
template<class T, std::size_t C_SIZE = 128, class Allocator = std::allocator<T>>
class Queue{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator          = T*;
    using const_iterator    = const T*;
    using pointer           = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer     = typename std::allocator_traits<Allocator>::const_pointer;
    using difference_type   = typename std::allocator_traits<Allocator>::difference_type;
    using size_type         = typename std::allocator_traits<Allocator>::size_type;
    using allocator_type    = Allocator;
    using ch_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<pointer>;

    /*** Constructors and Destructor ***/
    // Default constructor
    Queue() = default;

    // Allocator Constructor
    explicit Queue(const Allocator& alloc);

    // Copy Constructor
    Queue(const Queue& copyQ);

    // Move Constructor
    Queue(Queue&& moveQ) noexcept;

    // Destructor
    ~Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    template <class... Args>
    Queue& emplace(Args&&... args);
    Queue& push(const value_type& value);
    Queue& push(value_type&& value);
    Queue& pop();
    Queue& swap(Queue& swapQ) noexcept;
    Queue& flush();

    /*** Status Checkers ***/
    NODISCARD bool        empty() const { return (0 == sz); }
    NODISCARD size_type   size()  const { return sz;        }

    /*** Operators ***/
    Queue& operator=(const Queue& rightQ);
    NODISCARD bool operator==(const Queue& rightQ) const;
    NODISCARD bool operator!=(const Queue& rightQ) const;

private:
    /*** Members ***/
    size_type       sz              = 0;    // General size
    size_type       frontIdx        = 0;    // Front index in the front chunk (index of the oldest element)
    size_type       nextBackIdx     = 0;    // Back index in the back chunk (index 'after' the last added element)
    size_type       numOfChunks     = 0;    // Number of chunks (0 is the front)
    pointer*        chunks          = nullptr;          // Pointers of discrete chunks
    NO_UNIQUE_ADDR  Allocator           allocator;      // Allocator policy for storing the data
    NO_UNIQUE_ADDR  ch_allocator_type   chAllocator;    // Rebinded allocator policy for storing pointers

    /*** Helper Functions ***/
    NODISCARD const_pointer backChunk()  const;
    NODISCARD const_pointer frontChunk() const;
    NODISCARD pointer backChunk();
    NODISCARD pointer frontChunk();
    NODISCARD bool isFrontChunkConsumed() const { return (C_SIZE == frontIdx); }
    NODISCARD bool isNewChunkNeeded()     const { return ((C_SIZE == nextBackIdx) || (0 == numOfChunks)); }
    void createNewChunk();
    void removeFrontChunk();
};

/**
 * @brief Allocator constructor
 * @param alloc Allocator object
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(const Allocator& alloc)
    : allocator(std::allocator_traits<allocator_type>::select_on_container_copy_construction(alloc))
{ /* No operation */ }

/**
 * @brief   Copy constructor
 * @param   copyQ Source Queue for copying
 * @throws  std::logic_error    If the source queue was in an inconsistent state.
 * @throws  std::runtime_error  If the allocator fails to allocate chunks
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(const Queue& copyQ)
    : allocator(std::allocator_traits<allocator_type>::select_on_container_copy_construction(copyQ.allocator))
{
    // Check if the source Queue is empty
    if(!copyQ.empty())
    {
        if((0 == copyQ.numOfChunks) || (nullptr == copyQ.chunks))
            throw std::logic_error("Source Queue is corrupted!");

        // Allocate space for chunk pointers
        numOfChunks = ((copyQ.sz-1) / C_SIZE) + 1;
        chunks      = std::allocator_traits<ch_allocator_type>::allocate(chAllocator, numOfChunks);

        if(nullptr == chunks)
            throw std::runtime_error("Cannot allocate chunks array!");

        // Allocate space for chunks without constructing the objects
        for(size_type idx = 0; idx < numOfChunks; ++idx)
        {
            chunks[idx] = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

            if(nullptr == chunks[idx])
                throw std::runtime_error("Cannot allocate chunks!");
        }

        // Copy construct objects
        size_type sourceChunk = 0, destChunk = 0;
        size_type sourceElemIdx = copyQ.frontIdx, destElemIdx = 0;
        for( ; sz < copyQ.sz; ++sz)
        {
            std::allocator_traits<Allocator>::construct(allocator, chunks[destChunk] + destElemIdx++, copyQ.chunks[sourceChunk][sourceElemIdx++]);

            if(C_SIZE == sourceElemIdx)     // Skip to the next chunk of source?
            {
                sourceElemIdx = 0;
                ++sourceChunk;
            }

            if(C_SIZE == destElemIdx)       // Skip to the next chunk of destination?
            {
                destElemIdx = 0;
                ++destChunk;
            }
        }

        // Adjust indexes
        nextBackIdx     = destElemIdx;
        frontIdx        = 0;
    }
}

/**
 * @brief Move constructor
 * @param moveQ     Source queue for stealing resources
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::Queue(Queue&& moveQ) noexcept
    : allocator(std::move(moveQ.allocator))
{
    // Swap all members
    swap(moveQ);
}

/**
 * @brief Destructor
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>::~Queue()
{
    if(!empty())
    {
        if(1 == numOfChunks)    // Is front chunk and back chunk is the same?
        {
            // Destroy each element at that single chunk
            for(size_type idx = frontIdx; idx < nextBackIdx; ++idx)
                std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + idx);
        }
        else if(1 < numOfChunks)
        {
            // Destroy each element at the front chunk
            for(; frontIdx < C_SIZE; ++frontIdx)  // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + frontIdx);

            // Destroy all elements of other chunks except the back one
            for(size_type chunkIdx = 1; chunkIdx < (numOfChunks - 2); ++chunkIdx)    // Traverse chunks
            {
                for(size_type elemIdx = 0; elemIdx < C_SIZE; ++elemIdx)   // Traverse elements
                    std::allocator_traits<Allocator>::destroy(allocator, chunks[chunkIdx] + elemIdx);
            }

            // Destroy each element at back chunk
            for(size_type elemIdx = 0; elemIdx < nextBackIdx; ++elemIdx)   // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, backChunk() + elemIdx);
        }

        // Destroy each chunk
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx)
            std::allocator_traits<Allocator>::deallocate(allocator, chunks[chunkIdx], C_SIZE);
    }

    // Destroy chunk pointer array
    std::allocator_traits<ch_allocator_type>::deallocate(chAllocator, chunks, numOfChunks);
}

/**
 * @brief   Returns a const reference to the front element of Queue
 * @return  Constant lValue reference to the front element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::front() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return frontChunk()[frontIdx];
}

/**
 * @brief   Returns a reference to the front element of Queue
 * @return  lValue reference to the front element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::front()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return frontChunk()[frontIdx];
}

/**
 * @brief   Returns a const reference to the back element of Queue
 * @return  Constant lValue reference to the back element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
const T& Queue<T, C_SIZE, Allocator>::back() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return backChunk()[nextBackIdx-1];
}

/**
 * @brief   Returns a reference to the back element of Queue
 * @return  lValue reference to the back element
 * @throws  std::logic_error    If the Queue is empty
 */
template<class T, std::size_t C_SIZE, class Allocator>
T& Queue<T, C_SIZE, Allocator>::back()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return backChunk()[nextBackIdx-1];
}

/**
 * @brief   Pushes the element by constructing it in-place with the given arguments
 * @param   args  Arguments to be forwarded to the constructor of the new element
 * @return  lvalue reference to support cascaded calls
 * @throws  std::logic_error    If the source queue was in an inconsistent state.
 */
template<class T, std::size_t C_SIZE, class Allocator>
template <class... Args>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::emplace(Args&&... args)
{
    if(isNewChunkNeeded())
        createNewChunk();

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == nextBackIdx))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, backChunk() + nextBackIdx, std::forward<Args>(args)...);

    // Adjust size variables
    ++sz;
    ++nextBackIdx;

    return *this;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value Constant lValue reference to the object to be pushed
 * @return  lvalue reference to support cascaded calls
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::push(const value_type& value)
{
    if(isNewChunkNeeded())
        createNewChunk();

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == nextBackIdx))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, backChunk() + nextBackIdx, value);

    // Adjust size variables
    ++sz;
    ++nextBackIdx;

    return *this;
}

/**
 * @brief   Pushes the element to the Queue
 * @param   value rValue reference to the object to be pushed
 * @return  lvalue reference to support cascaded calls
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::push(value_type&& value)
{
    return emplace(std::move(value));
}

/**
 * @brief Pops the front element of the Queue
 * @return  lvalue reference to support cascaded calls
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::pop()
{
    if(empty()) // Nothing to pop
        return *this;

    // Destroy the front element
    std::allocator_traits<Allocator>::destroy(allocator, frontChunk() + frontIdx);

    // Adjust size variables
    ++frontIdx;
    --sz;

    if(isFrontChunkConsumed())
    {
        removeFrontChunk();
    }

    return *this;
}

/**
 * @brief Swaps the content of two Queues
 * @param swapQ     Queue to be swapped with
 * @return  lvalue reference to support cascaded calls
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::swap(Queue& swapQ) noexcept
{
    // Swap all members
    std::swap(sz,               swapQ.sz             );
    std::swap(nextBackIdx,   swapQ.nextBackIdx );
    std::swap(frontIdx,  swapQ.frontIdx);
    std::swap(numOfChunks,      swapQ.numOfChunks    );
    std::swap(chunks,           swapQ.chunks         );
    std::swap(allocator,        swapQ.allocator      );

    return *this;
}

/**
 * @brief Flushes the content of the Queue
 * @return  lvalue reference to support cascaded calls
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::flush()
{
    while(!empty())
        pop();

    return *this;
}

/**
 * @brief   Assignment operator
 * @param   rightQ The Queue that appears on the right side of the operator
 * @return  lvalue reference to support cascaded calls
 * @throws  std::logic_error    When the source Queue is in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
Queue<T, C_SIZE, Allocator>& Queue<T, C_SIZE, Allocator>::operator=(const Queue& rightQ)
{
    // Pop all the elements first
    flush();

    // Replace the allocator if needed
    if(allocator != rightQ.allocator)
        allocator = rightQ.allocator;

    // Prepare source indexes
    size_type chunkIdxRight = 0;
    size_type idxRight      = rightQ.frontIdx;

    if(0 == rightQ.numOfChunks)     // No data in the right Queue
    {
        if(rightQ.size() != 0)
            throw std::logic_error("Source Queue was in an inconsistent state!");

        return *this;
    }
    else if(1 == rightQ.numOfChunks)
    {
        // Traverse the single chunk of right Queue
        for( ; idxRight < rightQ.nextBackIdx; ++idxRight)
            push(rightQ.frontChunk()[idxRight]);

        return *this;
    }
    else
    {
        // Traverse each chunk of the right Queue
        for( ; chunkIdxRight < rightQ.numOfChunks; ++chunkIdxRight)
        {
            // If the current chunk is not the last one, traverse each element
            if(chunkIdxRight != (rightQ.numOfChunks - 2))
            {
                for( ; idxRight < C_SIZE; ++idxRight)
                    push(rightQ.chunks[chunkIdxRight][idxRight]);
            }
            else    // If the current chunk is the last one, traverse until the last element
            {
                for( ; idxRight < rightQ.nextBackIdx; ++idxRight)
                    push(rightQ.chunks[chunkIdxRight][idxRight]);
            }

            idxRight = 0;
        }

        // Index adjustments made in push(..) method
        return *this;
    }
}

/**
 * @brief   Comparison operator
 * @param   rightQ Queue that appears on the right side of the operator.
 * @return  true    If both Queue's are equal.
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD bool Queue<T, C_SIZE, Allocator>::operator==(const Queue& rightQ) const
{
    if(rightQ.sz != sz)
        return false;

    size_type chunkIdxRight    = 0;
    size_type chunkIdxLeft     = 0;
    size_type idxRight  = rightQ.frontIdx;
    size_type idxLeft   = frontIdx;

    for(size_type compCounter = 0; compCounter < sz; ++compCounter)
    {
        if(chunks[chunkIdxLeft][idxLeft++] != rightQ.chunks[chunkIdxRight][idxRight++])
            return false;

        if(C_SIZE == idxLeft)
        {
            idxLeft = 0;    // Reset element index
            ++chunkIdxLeft; // Increment left chunk index
        }

        if(C_SIZE == idxRight)
        {
            idxRight = 0;    // Reset element indexs
            ++chunkIdxRight; // Increment left chunk index
        }
    }

    return true;
}

/**
 * @brief   Inequality operator
 * @param   rightQ  Queue that appears on the right side of the operator.
 * @return  true    If Queue's are not equal
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD bool Queue<T, C_SIZE, Allocator>::operator!=(const Queue& rightQ) const
{
    return !(*this == rightQ);
}

/**
 * @brief   Helper method for accessing back chunk of the queue
 * @return  Const lValue reference to the back chunk of the queue.
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD typename Queue<T, C_SIZE, Allocator>::const_pointer Queue<T, C_SIZE, Allocator>::backChunk() const
{
    if(0 == numOfChunks)
        throw;

    return chunks[numOfChunks-1];
}

/**
 * @brief   Helper method for accessing back chunk of the queue
 * @return  lValue reference to the back chunk of the queue.
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD typename Queue<T, C_SIZE, Allocator>::pointer Queue<T, C_SIZE, Allocator>::backChunk()
{
    if(0 == numOfChunks)
        throw;

    return chunks[numOfChunks-1];
}

/**
 * @brief   Helper method for accessing front chunk of the queue
 * @return  Const lValue reference to the front chunk of the queue.
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD typename Queue<T, C_SIZE, Allocator>::const_pointer Queue<T, C_SIZE, Allocator>::frontChunk() const
{
    if(0 == numOfChunks)
        throw;

    return chunks[0];
}

/**
 * @brief   Helper method for accessing front chunk of the queue
 * @return  lValue reference to the front chunk of the queue.
 */
template<class T, std::size_t C_SIZE, class Allocator>
NODISCARD typename Queue<T, C_SIZE, Allocator>::pointer Queue<T, C_SIZE, Allocator>::frontChunk()
{
    if(0 == numOfChunks)
        throw;

    return chunks[0];
}

/**
 * @brief   Creates a new chunk when the size is about surpass the capacity of the back chunk
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 * @throws  std::runtime_error  If the allocator fails to allocate chunks
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::createNewChunk()
{
    // Check the status of container
    // This function should only be called when there is need for a new chunk
    if(!isNewChunkNeeded())
        throw std::logic_error("Early call to chunk creator!");

    // Allocate space for the new chunk
    pointer newChunk = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

    // Allocate space for chunk pointers
    pointer* newChunks = std::allocator_traits<ch_allocator_type>::allocate(chAllocator, numOfChunks+1);

    if((nullptr == newChunks) || (nullptr == newChunk))
        throw std::runtime_error("Cannot allocate space for the new chunk!");

    // Move the current chunk pointers
    if(0 < numOfChunks)
    {
        if(nullptr == chunks)
            throw std::logic_error("Chunks lost!");

        std::memcpy(newChunks, chunks, numOfChunks * sizeof(pointer));   // Copy pointers directly

        // Deallocate old chunk array
        std::allocator_traits<ch_allocator_type>::deallocate(chAllocator, chunks, numOfChunks);
    }

    // Append the new chunk
    newChunks[numOfChunks] = newChunk;

    // Change chunk array with the new one
    chunks = newChunks;

    // Adjust current chunk variables
    ++numOfChunks;
    nextBackIdx = 0;
}

/**
 * @brief   Removes the front chunk when it has no more elements to fetch
 * @throws  std::logic_error    If the Queue was in an inconsistent state
 */
template<class T, std::size_t C_SIZE, class Allocator>
void Queue<T, C_SIZE, Allocator>::removeFrontChunk()
{
    if(isFrontChunkConsumed() == false)
        throw std::logic_error("Front chunks is not consumed yet!");

    if((nullptr == chunks) || (0 == numOfChunks))
        throw std::logic_error("Cannot remove a non-exist chunk!");

    // No need to remove the chunk if there is only one left, just reset indexes
    if(1 == numOfChunks)
    {
        if((C_SIZE != nextBackIdx) || (C_SIZE != frontIdx) || (0 != sz))
            throw std::logic_error("Indexes are corrupted!");

        frontIdx = 0;
        nextBackIdx = 0;

        return;
    }

    // Deallocate front chunk
    std::allocator_traits<Allocator>::deallocate(allocator, frontChunk(), C_SIZE);

    /* There is no need to allocate new space for chunk pointers
     * as the currently allocated area will be enough to contain
     * the cropped version of previous pointer array */

    // Move chunk pointers backwards
    for(size_type idx = 0; idx < (numOfChunks-1); ++idx)
        chunks[idx] = chunks[idx+1];

    --numOfChunks;  // Decrement the number of chunks

    frontIdx = 0;    // Reset front index
}
