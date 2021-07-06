/**
 * @file        QueueContainer.h
 * @details     A template queue container class
 * @author      Caglayan DOKME, caglayandokme@gmail.com
 * @date        July 6, 2021 -> First release
 * @note        Feel free to contact for questions, bugs or any other thing.
 * @copyright   No copyright.
 */

/*** Recursive inclusion preventer ***/
#pragma once

/*** Libraries ***/
#include <memory>       // std::allocator, std::allocator_traits
#include <cstring>      // std::memcpy
#include <algorithm>    // std::swap

/*** Special definitions ***/
// If the C++ version is greater or equal to 2017xx
#if __cplusplus >= 201703l
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

/*** Container Class ***/
template<class T, class Allocator = std::allocator<T>, std::size_t C_SIZE = 128>
class Queue{
public:
    /*** C++ Standard Named Requirements for Containers ***/
    using value_type        = T;
    using reference         = T&;
    using const_reference   = const T&;
    using iterator          = T*;
    using const_iterator    = const T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using allocator_type    = Allocator;

    /*** Constructors and Destructor ***/
    Queue();

    ~Queue();

    /*** Element Access ***/
    NODISCARD const_reference front() const;
    NODISCARD reference       front();
    NODISCARD const_reference back() const;
    NODISCARD reference       back();

    /*** Modifiers ***/
    void push(const value_type& value);
    void push(value_type&& val);

    template <class... Args>
    void emplace(Args&&... args);
    void pop();
    void swap(Queue& x) noexcept;

    /*** Status Checkers ***/
    bool        empty() const { return (0 == sz);   }
    size_type   size()  const { return sz;          }

private:
    /*** Members ***/
    size_type       sz;                 // General size
    size_type       idxInFrontChunk;    // Front index in the front chunk (index of the least recently added element)
    size_type       idxInBackChunk;     // Back index in the back chunk (index after the last added element)
    size_type       numOfChunks;        // Number of chunks (0 is the front)
    value_type**    chunks;             // Pointers of discrete chunks
    Allocator       allocator;          // Allocator policy

    /*** Helper Functions ***/
    bool isFrontChunkConsumed() const { return (C_SIZE == idxInFrontChunk); }
    bool isNewChunkNeeded()     const { return ((C_SIZE == idxInBackChunk) || (0 == numOfChunks));  }
    void createNewChunk();
    void removeFrontChunk();
};

template<class T, class Allocator, std::size_t C_SIZE>
Queue<T, Allocator, C_SIZE>::Queue()
    : sz(0), idxInFrontChunk(0), idxInBackChunk(0), numOfChunks(0), chunks(nullptr)
{ /* No operation */ }

template<class T, class Allocator, std::size_t C_SIZE>
Queue<T, Allocator, C_SIZE>::~Queue()
{
    if(!empty())
    {
        if(1 == numOfChunks)
        {
            // Destroy each element at front chunk(Front chunk is the only chunk in queue)
            for(size_type idx = idxInFrontChunk; idx < idxInBackChunk; ++idx)
                std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idx);
        }
        else if(1 < numOfChunks)
        {
            // Destroy each element at front chunk
            for(; idxInFrontChunk < C_SIZE; ++idxInFrontChunk)
                std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idxInFrontChunk);

            // Destroy all elements of other chunks except the back one
            for(size_type chunkIdx = 1; chunkIdx < (numOfChunks - 2); ++chunkIdx)    // Traverse chunks
            {
                for(size_type elemIdx = 0; elemIdx < C_SIZE; ++elemIdx)   // Traverse elements
                    std::allocator_traits<Allocator>::destroy(allocator, chunks[chunkIdx] + elemIdx);
            }

            // Destroy each element at back chunk
            for(size_type elemIdx = 0; elemIdx < idxInBackChunk; ++elemIdx)   // Traverse elements
                std::allocator_traits<Allocator>::destroy(allocator, chunks[numOfChunks-1] + elemIdx);
        }

        // Destroy each chunk
        for(size_type chunkIdx = 0; chunkIdx < numOfChunks; ++chunkIdx)
            std::allocator_traits<Allocator>::deallocate(allocator, chunks[chunkIdx], C_SIZE);
    }

    // Destroy chunk pointer array
    delete [] chunks;
}

template<class T, class Allocator, std::size_t C_SIZE>
const T& Queue<T, Allocator, C_SIZE>::front() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return chunks[0][idxInFrontChunk];
}

template<class T, class Allocator, std::size_t C_SIZE>
template <class... Args>
void Queue<T, Allocator, C_SIZE>::emplace(Args&&... args)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, std::forward<Args>(args)...);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, class Allocator, std::size_t C_SIZE>
T& Queue<T, Allocator, C_SIZE>::front()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    return chunks[0][idxInFrontChunk];
}

template<class T, class Allocator, std::size_t C_SIZE>
const T& Queue<T, Allocator, C_SIZE>::back() const
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    if(idxInBackChunk == 0)
        return chunks[numOfChunks-2][C_SIZE-1];
    else
        return chunks[numOfChunks-1][idxInBackChunk-1];
}

template<class T, class Allocator, std::size_t C_SIZE>
T& Queue<T, Allocator, C_SIZE>::back()
{
    if(empty())
        throw std::logic_error("Queue is empty!");

    if(idxInBackChunk == 0)
        return chunks[numOfChunks-2][C_SIZE-1];
    else
        return chunks[numOfChunks-1][idxInBackChunk-1];
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::push(const value_type& value)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, value);

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::push(value_type&& value)
{
    if(isNewChunkNeeded() == true)
    {
        createNewChunk();
    }

    if((0 == numOfChunks) || (nullptr == chunks) || (C_SIZE == idxInBackChunk))
        throw std::logic_error("Chunks are corrupted!");

    // Copy construct the new element at the back
    std::allocator_traits<Allocator>::construct(allocator, chunks[numOfChunks-1] + idxInBackChunk, std::move(value));

    // Adjust size variables
    ++sz;
    ++idxInBackChunk;
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::pop()
{
    if(0 == sz) // Nothing to pop
        return;

    // Destroy the front element
    std::allocator_traits<Allocator>::destroy(allocator, chunks[0] + idxInFrontChunk);

    // Adjust size variables
    ++idxInFrontChunk;
    --sz;

    if(isFrontChunkConsumed() == true)
    {
        removeFrontChunk();
    }
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::swap(Queue& x) noexcept
{
    // Swap all members
    std::swap(sz,               x.sz                );
    std::swap(idxInBackChunk,   x.idxInBackChunk    );
    std::swap(idxInFrontChunk,  x.idxInFrontChunk   );
    std::swap(numOfChunks,      x.numOfChunks       );
    std::swap(chunks,           x.chunks            );
    std::swap(allocator,        x.allocator         );
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::createNewChunk()
{
    // Check the status of container
    // This function should only be called when there is need for a new chunk
    if(isNewChunkNeeded() == false)
        throw std::logic_error("Early call to chunk creator!");

    // Allocate space for the new chunk
    value_type* newChunk = std::allocator_traits<Allocator>::allocate(allocator, C_SIZE);

    // Allocate space for chunk pointers
    value_type** newChunks = new value_type*[numOfChunks + 1];

    if((nullptr == newChunks) || (nullptr == newChunk))
        throw std::logic_error("Cannot allocate space for the new chunk!");

    // Move the current chunk pointers
    if(0 < numOfChunks)
    {
        if(nullptr == chunks)
            throw std::logic_error("Chunks lost!");

        std::memcpy(newChunks, chunks, numOfChunks * sizeof(value_type*));   // Copy pointers directly

        // Deallocate old chunk array
        delete [] chunks;
    }

    // Append the new chunk
    newChunks[numOfChunks] = newChunk;

    // Change chunk array with the new one
    chunks = newChunks;

    // Adjust current chunk variables
    ++numOfChunks;
    idxInBackChunk = 0;
}

template<class T, class Allocator, std::size_t C_SIZE>
void Queue<T, Allocator, C_SIZE>::removeFrontChunk()
{
    if(isFrontChunkConsumed() == false)
        throw std::logic_error("Front chunks is not consumed yet!");

    if((nullptr == chunks) || (0 == numOfChunks))
        throw std::logic_error("Cannot remove a non-exist chunk!");

    // No need to remove the chunk if there is only one left, just reset indexes
    if(1 == numOfChunks)
    {
        if((C_SIZE != idxInBackChunk) || (C_SIZE != idxInFrontChunk) || (0 != sz))
            throw std::logic_error("Indexes are corrupted!");

        idxInFrontChunk = 0;
        idxInBackChunk = 0;

        return;
    }

    // Deallocate front chunk
    std::allocator_traits<Allocator>::deallocate(allocator, chunks[0], C_SIZE);

    /* There is no need to allocate new space for chunk pointers
     * as the currently allocated area will be enough to contain
     * the cropped version of previous pointer array */

    // Move chunk pointers backwards
    for(size_type idx = 0; idx < (numOfChunks-1); ++idx)
        chunks[idx] = chunks[idx+1];

    --numOfChunks;  // Decrement the number of chunks

    idxInFrontChunk = 0;    // Reset front index
}


















