/** @file       ListContainer.h
 *  @details    A template container class with an underlying
 *              doubly linked list structure.
 *  @author     Caglayan DOKME, caglayandokme@gmail.com
 *  @date       February 25, 2021 -> First release
 *              February 26, 2021 -> File documented with doxygen.
 *                                -> Conditional remove functions added.
 *                                -> Swap function added.
 *              February 27, 2021 -> Resize function added.
 *                                   Remove function with lambda predicate added.
 *
 *  @note       Feel free to contact for questions, bugs or any other thing.
 *  @copyright  No copyright. Code is open source.
 */

#include <iostream>

// Forward declaration
template<class T> class ListNode;

template<class T>
class List{
public:
    List(); // Constructor

    virtual ~List();    // Destructor

    List<T>& Append(const T& data);     // Add after the last node
    List<T>& Prepend(const T& data);    // Add before the first node

    const T& First() const; // Get the first data as an rValue
    const T& Last() const;  // Get the last data as an rValue
    T& First();             // Get the first data as an lValue
    T& Last();              // Get the first data as an lValue

    template<class RuleT>
    List<T>& RemoveIf(RuleT Predicate);         // Remove all fulfilling the condition of predicate
    List<T>& EraseAll();                        // Remove all elements
    List<T>& RemoveFirst();                     // Remove the first node
    List<T>& RemoveLast();                      // Remove the last node
    List<T>& RemoveIf(const T& data);           // Remove all samples of a specific data
    List<T>& RemoveFirstOf(const T& data);      // Remove the first sample of a specific data
    List<T>& RemoveLastOf(const T& data);       // Remove the last sample of a specific data
    List<T>& RemoveIfNot(const T& data);        // Remove all samples which are not of a specific data
    List<T>& RemoveFirstNotOf(const T& data);   // Remove the first sample that is not the given data
    List<T>& RemoveLastNotOf(const T& data);    // Remove the last sample that is not the given data

    void Swap(List<T>& anotherList);                            // Exchanges the content of the list by the content of another list
    void Resize(const size_t newSize, const T& data = 0);       // Resizes the list so that it contains newSize of elements

    bool isEmpty() const
    { return (numberOfNodes == 0); }

    /* Declaring a function as a friend inside of a template class
       corrupts the template usage. You may want to check the holy StackOverflow :)
       stackoverflow.com/questions/4660123 */
    template<class _T>
    friend std::ostream& operator<<(std::ostream& stream, List<_T>& list);

    void PrintAll(std::ostream& stream);

    size_t GetNodeCount() const
    { return numberOfNodes; }

private:
    ListNode<T>* Find(const T& data, ListNode<T>* beginByNode);
    ListNode<T>* FindNotOf(const T& data, ListNode<T>* beginByNode);
    ListNode<T>* FindReversed(const T& data, ListNode<T>* beginByNode);
    ListNode<T>* FindNotOfReversed(const T& data, ListNode<T>* beginByNode);
    void RemoveNode(ListNode<T>* removingNode);

    ListNode<T>* firstPtr   = nullptr;  // First node of the list
    ListNode<T>* lastPtr    = nullptr;  // Last node of the list
    size_t numberOfNodes    = 0;        // Node count
};

template<class T>
class ListNode{
    friend class List<T>;

public:
    ListNode(const T& data) : data(data), prevPtr(nullptr), nextPtr(nullptr)
    { /* Empty constructor */ }

private:
    T data;
    ListNode* prevPtr = nullptr;
    ListNode* nextPtr = nullptr;
};

/**
 * @brief Default constructor
 */
template<class T>
List<T>::List()
: firstPtr(nullptr), lastPtr(nullptr), numberOfNodes(0)
{ /* Empty constructor */ }


/**
 * @brief Destroys all nodes one by one
 */
template<class T>
List<T>::~List()
{
    /* Destroy all nodes until there is no node left. */
    while(isEmpty() == false)
        RemoveFirst();
}


/**
 * @brief   Appends the given data next to the last node
 * @param   data      Data to be appended
 * @return  lValue reference to the current list to support cascades
 */
template<class T>
List<T>& List<T>::Append(const T& data)
{
    if(isEmpty() == true)  // If it is the first node
    {
        firstPtr    = new ListNode<T>(data);    // Create the first node
        lastPtr     = firstPtr; // The last and the first points the same node
    }
    else
    {
        lastPtr->nextPtr = new ListNode<T>(data);   // Create and append the node
        lastPtr->nextPtr->prevPtr = lastPtr;        // Adjust prevNode connection
        lastPtr = lastPtr->nextPtr;                 // Update the lastPtr
    }

    numberOfNodes++;    // Increase the number of nodes

    return *this;   // Support cascaded appends
}


/**
 * @brief   Prepends the data before the first node
 * @param   data      Data to be prepended
 * @return  lValue reference to the current list to support cascades
 */
template<class T>
List<T>& List<T>::Prepend(const T& data)
{
    if(isEmpty() == true)   // If it is the first node
    {
        firstPtr    = new ListNode<T>(data);    // Create the first node
        lastPtr     = firstPtr; // The last and the first points the same node
    }
    else
    {
        firstPtr->prevPtr = new ListNode<T>(data);  // Create and prepend the node
        firstPtr->prevPtr->nextPtr = firstPtr;      // Adjust nextNode connection
        firstPtr = firstPtr->prevPtr;               // Update the firstPtr
    }

    numberOfNodes++;    // Increase the number of nodes

    return *this;
}


/**
 * @brief   Iterator to reach to the first element.
 * @return  rValue reference to the data of first node.
 * @throws  std::logic_error If the list is empty
 */
template<class T>
const T& List<T>::First() const
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return firstPtr->data;
}

/**
 * @brief   Iterator to reach to the last element.
 * @return  rValue reference to the data of last node.
 * @throws  std::logic_error If the list is empty
 */
template<class T>
const T& List<T>::Last() const
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return lastPtr->data;
}

/**
 * @brief   Iterator to reach to the first element.
 * @return  lValue reference to the data of first node.
 * @throws  std::logic_error If the list is empty
 */
template<class T>
T& List<T>::First()
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return firstPtr->data;
}

/**
 * @brief   Iterator to reach to the last element.
 * @return  lValue reference to the data of last node.
 * @throws  std::logic_error If the list is empty
 */
template<class T>
T& List<T>::Last()
{
    if(isEmpty() == true)
        throw std::logic_error("List is empty!");

    return lastPtr->data;
}

/**
 * @brief   Removes from the list all the elements for which Predicate returns true.
 * @param   Predicate   Unary predicate that, taking a value of the same type as those
 *                      contained in the list nodes, returns true for those values to be
 *                      removed from the list, and false for those remaining.
 * @return  lValue reference to the current list to support cascaded calls
 * @note    An example call would look like this
 *          userList.RemoveIf([](T value) {return value > XXX;});
 *
 * @note    For more examples, refer to:
 *          github.com/CaglayanDokme/CPP-Exercises/blob/main/FuncWithLambdaArg.cpp
 */
template<class T>
template<class RuleT>
List<T>& List<T>::RemoveIf(RuleT Predicate)
{
    ListNode<T> *currentNode = firstPtr, *tempNode;

    while(currentNode != nullptr)
    {
        tempNode = currentNode->nextPtr;

        if(Predicate(currentNode->data) == true)
            RemoveNode(currentNode);

        currentNode = tempNode;
    }

    return *this; // Support cascaded calls
}

/**
 * @brief   Removes all nodes
 * @return  lValue reference to the empty list to support cascaded calls
 */
template<class T>
List<T>& List<T>::EraseAll()
{
    /* Remove all until the list is empty */
    while(isEmpty() == false)
        RemoveFirst();
}

/**
 * @brief   Removes the first node
 * @return  lValue reference to the current list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveFirst()
{
    if(isEmpty() == false)
    {
        ListNode<T>* tempPtr = firstPtr;    // Save removing node addresss
        firstPtr = firstPtr->nextPtr;       // Update firstPtr
        delete tempPtr;                     // Delete saved firstPtr
        numberOfNodes--;                    // Decrement node count

        if(firstPtr != nullptr)
            firstPtr->prevPtr = nullptr;    // Remove prevNode connection
    }

    return *this;   // Support cascaded remove calls
}

/**
 * @brief   Removes the last node
 * @return  lValue reference to the current list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveLast()
{
    if(isEmpty() == false)
    {
        ListNode<T>* tempPtr = lastPtr;     // Save removing node addresss
        lastPtr = lastPtr->prevPtr;         // Update lastPtr
        delete tempPtr;                     // Delete saved lastPtr
        numberOfNodes--;                    // Decrement node count

        if(lastPtr != nullptr)
            lastPtr->nextPtr = nullptr;    // Remove nextNode connection
    }

    return *this;   // Support cascaded remove calls
}

/**
 * @brief   Removes all samples of a specific kind of data
 * @param   data    Value to be removed
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveIf(const T& data)
{
    ListNode<T>* removingNode;      // Node to be removed
    ListNode<T>* searchStartPoint;  // Node where the search will start

    /* Find and remove all specified nodes until
     * we hit the last of the list */
    removingNode = Find(data, firstPtr);    // Find the first sample
    while(removingNode != nullptr)
    {
        searchStartPoint = removingNode->nextPtr;       // Save the next node
        RemoveNode(removingNode);                       // Remove the node found
        removingNode = Find(data, searchStartPoint);    // Find the next removing node
    }

    return *this;
}

/**
 * @brief   Removes the first sample of given data.
 * @param   data Search key
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveFirstOf(const T& data)
{
    RemoveNode(Find(data, firstPtr));   // Find and remove the first sample

    return *this;
}

/**
 * @brief   Removes the last sample of given data.
 * @param   data Search key
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveLastOf(const T& data)
{
    RemoveNode(FindReversed(data, lastPtr));   // Find and remove the last sample

    return *this;
}

/**
 * @brief   Removes all samples not of a specific kind of data
 * @param   data    Value to be removed
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveIfNot(const T& data)
{
    ListNode<T>* removingNode;      // Node to be removed
    ListNode<T>* searchStartPoint;  // Node where the search will start

    /* Find and remove all inequal nodes until
     * we hit the last of the list */
    removingNode = FindNotOf(data, firstPtr);    // Find the first inequal sample
    while(removingNode != nullptr)
    {
        searchStartPoint = removingNode->nextPtr;           // Save the next node
        RemoveNode(removingNode);                           // Remove the node found
        removingNode = FindNotOf(data, searchStartPoint);   // Find the next removing node
    }

    return *this;
}

/**
 * @brief   Removes the first sample not of given data.
 * @param   data Comparison key
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveFirstNotOf(const T& data)
{
    // Find and remove the first sample not of given data
    RemoveNode(FindNotOf(data, firstPtr));

    return *this;
}

/**
 * @brief   Removes the last sample not of given data.
 * @param   data Comparison key
 * @return  lValue reference to the list to support cascaded calls
 */
template<class T>
List<T>& List<T>::RemoveLastNotOf(const T& data)
{
    // Find and remove the last sample not of given data
    RemoveNode(FindNotOfReversed(data, lastPtr));

    return *this;
}

/**
 * @brief   Swaps two different lists' contents.
 * @param   anotherList     List to be swapped with this.
 * @note    There is no need to make a complete swap.
 *          Interchancing the first and last nodes does the same.
 */
template<class T>
void List<T>::Swap(List<T>& anotherList)
{
    if(*this == anotherList)
        return;     // Self swap is not required

    ListNode<T>* tempPtr;
    size_t tempSize;

    // Swap the first nodes of each list
    tempPtr                 = firstPtr;             // Save the firstPtr of this
    firstPtr                = anotherList.firstPtr; // Replace the firstPtr of this
    anotherList.firstPtr    = tempPtr;             // Replace the firstPtr of the other list

    tempPtr                 = lastPtr;             // Save the lastPtr of this
    lastPtr                 = anotherList.lastPtr; // Replace the lastPtr of this
    anotherList.lastPtr     = tempPtr;             // Replace the lastPtr of the other list

    // Swap the size values
    tempSize                    = numberOfNodes;                // Save the size of this list
    numberOfNodes               = anotherList.numberOfNodes;    // Replace the size of this
    anotherList.numberOfNodes   = tempSize;                     // Replace the size of the other list
}

/**
 * @brief Resizes the list so that it contains n elements
 * @param newSize   New list size, expressed in number of elements
 * @param data      Object whose content is copied to the appended nodes
 */
template<class T>
void List<T>::Resize(const size_t newSize, const T& data)
{
    // Remove excessive nodes if exists
    while(newSize < GetNodeCount())
        RemoveLast();

    // Append new nodes if needed
    while(newSize > GetNodeCount())
        Append(data);
}

/**
 * @brief   Output insertion overloaded to be used with a list
 * @param   stream  Output stream where the list will be inserted to.
 * @param   list    List to be inserted.
 * @return  lValue reference to stream to support cascaded calls.
 */
template<class T>
std::ostream& operator<<(std::ostream& stream, List<T>& list)
{
    if(list.isEmpty() == true)
        stream << "-- empty list --";
    else
        list.PrintAll(stream);

    return stream; // Support cascaded streams
}

/**
 * @brief Prints the data of all nodes
 * @param stream    Output stream where the list will be inserted to.
 */
template<class T>
void List<T>::PrintAll(std::ostream& stream)
{
    for(ListNode<T>* currentNode = firstPtr; currentNode != nullptr; currentNode = currentNode->nextPtr)
        stream << currentNode->data << " ";
}


/**
 * @brief   Finds the address of the first node where the specified data is contained at.
 * @param   data          Search key
 * @param   beginByNode   Search start by the given node
 * @return  Address of the first sample of given search key
 *          Returns nullptr if the data couldn't found.
 * @note    The algorithm used is the linear search as there are no value-based relation between nodes.
 */
template<class T>
ListNode<T>* List<T>::Find(const T& data, ListNode<T>* beginByNode)
{
    // Search begins by the given node
    ListNode<T>* currentNode = beginByNode;

    /* Break the search if the element is found
     * or the last element is hit */
    while(currentNode != nullptr)
    {
        if(currentNode->data == data)
            break;
        else
            currentNode = currentNode->nextPtr;
    }

    return currentNode;
}


/**
 * @brief   Finds the address of the first node where the data is not the specified one.
 * @param   data          Comparison key
 * @param   beginByNode   Search start by the given node
 * @return  Address of the first sample that is not of given search key
 *          Returns nullptr if the data couldn't found.
 * @note    The algorithm used is the linear search as there are no value-based relation between nodes.
 */
template<class T>
ListNode<T>* List<T>::FindNotOf(const T& data, ListNode<T>* beginByNode)
{
    // Search begins by the given node
    ListNode<T>* currentNode = beginByNode;

    /* Break the search if the element is found
     * or the last element is hit */
    while(currentNode != nullptr)
    {
        if(currentNode->data != data)   // Careful! It is the inequality operator
            break;
        else
            currentNode = currentNode->nextPtr;
    }

    return currentNode;
}

/**
 * @brief   Finds the address of the last node where the specified data is contained at.
 * @param   data          Search key
 * @param   beginByNode   Search start by the given node
 * @return  Address of the last sample of given search key
 *          Returns nullptr if the data couldn't found.
 * @note    The algorithm used is the reversed linear search as there are no value-based relation between nodes.
 */
template<class T>
ListNode<T>* List<T>::FindReversed(const T& data, ListNode<T>* beginByNode)
{
    // Search begins by the given node
    ListNode<T>* currentNode = beginByNode;

    /* Break the search if the element is found
     * or the first element is hit */
    while(currentNode != nullptr)
    {
        if(currentNode->data == data)
            break;
        else
            currentNode = currentNode->prevPtr; // Search advances reversely
    }

    return currentNode;
}

/**
 * @brief   Finds the address of the last node where the data is not the specified one.
 * @param   data          Comparison key
 * @param   beginByNode   Search start by the given node
 * @return  Address of the last sample that is not of given search key
 *          Returns nullptr if the data couldn't found.
 * @note    The algorithm used is the reversed linear search as there are no value-based relation between nodes.
 */
template<class T>
ListNode<T>* List<T>::FindNotOfReversed(const T& data, ListNode<T>* beginByNode)
{
    // Search begins by the given node
    ListNode<T>* currentNode = beginByNode;

    /* Break the search if the element is found
     * or the first element is hit */
    while(currentNode != nullptr)
    {
        if(currentNode->data != data)   // Careful! It is the inequality operator
            break;
        else
            currentNode = currentNode->prevPtr; // Search advances reversely
    }

    return currentNode;
}


/**
 * @brief   Removes the given node.
 * @param   removingNode Address of the node to be removed.
 */
template<class T>
void List<T>::RemoveNode(ListNode<T>* removingNode)
{
    if(removingNode == nullptr)         // Return if the node is not valids
        return;
    else if(removingNode == firstPtr)   // First and last pointers are special
        RemoveFirst();                  // as there is no previous or next
    else if(removingNode == lastPtr)    // pointers for nodes. This affects
        RemoveLast();                   // the reconnection strategy.
    else    // If it is a random node
    {
        // Connect previous and next nodes
        removingNode->nextPtr->prevPtr = removingNode->prevPtr;
        removingNode->prevPtr->nextPtr = removingNode->nextPtr;

        delete removingNode;    // Delete the node
        numberOfNodes--;        // Decrement node counter
    }
}
