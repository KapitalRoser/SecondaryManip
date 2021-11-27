#include <iostream>

using namespace std;

class Person
{
    string m_firstName; // object of type string
    string m_lastName;
    string m_address;

  public:

    Person(string first, string last)
    // there is no initi line:
    // the data fields are implicitly initialized to empty strings
    // because a string has a "default constructor"
    // that takes no arguments and initializes the string to be empty
    // equivalent to writing
    //: m_firstName(),
    //  m_lastName(),
    //  m_address()
     {  // constructor
        setFirstName(first);
        setLastName(last);
        cout <<"constructor1 for " << first << " " << last << endl;
      }

    Person(string first, string last, string address)
    : m_firstName(first),
      m_lastName(last),
      m_address(address) // init line - this is more correct
      {
        cout <<"constructor2 for " << first << " " << last << endl;
      } // constructor
 

    string getFirstName() { return m_firstName;}
 
    void setFirstName(string first) {
      m_firstName = string(first); // uses the copy constructor
    }

    string getLastName() { return m_lastName;}

    void setLastName(string last) {
      m_lastName = last; // invokes string(last)
    }

    string getAddress() { return m_address;}

    void setAddress(string address) {
      m_lastName = address; // invokes string(last)
    }

    void printInfo() {
      // endl is declared in namespece std
      cout << "Name: " << getFirstName() << " " << getLastName() << endl;
      if(m_address.length()>0)
        cout << "Address: " << getAddress() << endl;
      else
        cout << "Address not found" << endl;
    }

    ~Person() {
      cout << "destructor for " << getFirstName() << " " << getLastName() << endl;
    } // m_firstName, m_lastName, m_address are local variables, no need to destroy them
};



int main() {
  // a local object of Person
  Person p0 = Person("Clark", "Kent");
  Person p1 = Person("Peter", "Parker", "20 Ingram Street");

  cout << "--------" << endl;
  p0.printInfo();
  cout << "--------" << endl;
  p1.printInfo();
  cout << "--------" << endl;

  return 0;
  // p0 and p1 are deleted from the stack
}

