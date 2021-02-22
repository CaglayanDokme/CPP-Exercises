// Description: Examples of ostream_iterator's use cases
// Date:        February 16, 2021
// Author:      Caglayan DOKME

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
    /*  Ostream iterators are constructed from a basic_ostream object,
     *  to which they become associated, so that whenever an assignment
     *  operator (=) is used on the ostream_iterator (dereferenced or not)
     *  it inserts a new element into the stream. */
    ostream_iterator<int> screenLog(cout, " ");         // Space between each integer output
    ostream_iterator<string> errorLog(cerr, "\r\n");    // Linefeed between each error log

    int array[5] = {0, 1, 2, 3, 4};
    vector<string> errors{"Error 1", "Error 2", "Error 3"};

    cout << "Printing integers with copy(..): ";
    copy(array, array + 5, screenLog);  // Copy is an overloaded template algorithm to print each element individually
    cout << endl;

    cout << "Printing integers with assignment: ";
    for(const int& number : array)
        screenLog = number;
    cout << endl;

    cout << "Printing errors with copy(..): " << endl;
    copy(errors.begin(), errors.end(), errorLog);
    cout << endl;

    cout << "Printing errors with assignment: " << endl;
    for(const string& errorMessage : errors)
        errorLog = errorMessage;
    cout << endl;

    cout << "Program ended!" << endl;

    return 0;
}
