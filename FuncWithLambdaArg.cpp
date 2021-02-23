// Description: Function examples with lambda expressions as input parameters
// Author:      Caglayan DOKME
// Date:        February 20, 2021 -> First release
//              February 23, 2021 -> std::function example added.

#include <iostream>
#include <vector>
#include <iterator>
#include <functional>

using namespace std;

template<typename ContainerType, typename LambdaType>
void Function(ContainerType& container, LambdaType lambda)
{
    /*  Iterate over the given container and call the lambda expression with the current element. */
    for(auto element : container)
        if(lambda(element) == true)
            cout << element << endl;
}

int main(int argc, char const *argv[]) {
    vector<int> v1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    /*  Example Lambda Expression:
     *  [capture list] (parameter list) -> return type {function body} */

    /** Inline Lambda Expression **/
    cout << "Printing values(>5) with inline lambda: " << endl;
    Function(v1, [](int value) { return (value > 5); });
    cout << endl;

    /** Lambda Expression Object **/
    auto lambdaFunc =   []                          /* Capture list is empty    */
                        (int value)                 /* Function-like parameters */
                        { return (value < 5); };    /* Function body            */
    cout << "Printing values(<5) with lambda expression object: " << endl;
    Function(v1, lambdaFunc);
    cout << endl;

    /** Lambda Expression Object with Capture List **/
    const int limit = 3;
    auto lambdaFuncCapture =    [limit]                         /* Capture list is empty    */
                                (int value)                     /* Function-like parameters */
                                { return (value > limit); };    /* Function body            */
    cout << "Printing values(>limit(" << limit
         << ")) using lambda expression with non-empty capture list : " << endl;
    Function(v1, lambdaFuncCapture);
    cout << endl;

    /** Lambda Expression Object with Return Type **/
    auto lambdaFuncRetType =    [limit]                         /* Capture list is empty    */
                                (int value)                     /* Function-like parameters */
                                -> bool                         /* Return type added        */
                                { return (value > limit); };    /* Function body            */
    cout << "Printing values using lambda expression with explicit return type : " << endl;
    Function(v1, lambdaFuncRetType);
    cout << endl;

    /** STD::FUNCTION with Lambda Expression **/
    // Do not forget to include the standard library <functional>
    // Example syntax: std::function</* Signature of Function */>
    // Signature of a function includes its return type and input parameters
    function<bool(int)> stdFuncLabmda = [limit]                         /* Capture list is empty    */
                                        (int value)                     /* Function-like parameters */
                                        -> bool                         /* Return type added        */
                                        { return (value > limit); };    /* Function body            */


    cout << "Printing values using lambda expression of type std::function : " << endl;
    Function(v1, stdFuncLabmda);
    cout << endl;

    return 0;
}
