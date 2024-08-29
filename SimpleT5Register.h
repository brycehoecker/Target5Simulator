/*	SimpleT5Register.h
 *      Author: Bryce Hoecker sctsim
 */

#ifndef SIMPLET5REGISTER_H_		// Check if the macro SIMPLET5REGISTER_H_ is not defined. This is to prevent double inclusion.
#define SIMPLET5REGISTER_H_		// Define the macro SIMPLET5REGISTER_H_ to indicate the header has been included.

// Declare the class SimpleT5Register.
class SimpleT5Register {
public: 															// The following members are public and can be accessed outside the class.
	SimpleT5Register();												// Default constructor declaration.
	virtual ~SimpleT5Register();									// Virtual destructor declaration. The 'virtual' keyword means this destructor can be overridden in derived classes.
	SimpleT5Register& operator=(SimpleT5Register &&other);			// Move assignment operator declaration. It takes an rvalue reference to another SimpleT5Register.
	SimpleT5Register& operator=(const SimpleT5Register &other);		// Copy assignment operator declaration. It takes a const lvalue reference to another SimpleT5Register.
	SimpleT5Register(SimpleT5Register &&other);						// Move constructor declaration. It takes an rvalue reference to another SimpleT5Register.
	SimpleT5Register(const SimpleT5Register &other);				// Copy constructor declaration. It takes a const lvalue reference to another SimpleT5Register.


    // Member function to retrieve a value from registerArr.
    uint32_t getRegisterValue(int index) const { // Declares a constant member function because it doesn't modify the object's state.
        // Check if the provided index is valid (i.e., within the bounds of the array).
        if (index >= 0 && index < 84) {
            // If the index is valid, return the value stored at that index in registerArr.
            return registerArr[index];
        }
        else{
        	std::cout << "Index out of bounds" << std::endl;
        	return registerArr[0];
        }// If the index is out of bounds, throw an exception.
        // std::out_of_range is a standard C++ exception for indicating that an index is out of valid range.
        //throw std::out_of_range("Index out of bounds");
    }

    // Member function to set a value in registerArr.
    void setRegisterValue(int index, uint32_t value) { // Takes two arguments: an index and a value.
        // Check if the provided index is valid.
        if (index >= 0 && index < 84) {
            // If the index is valid, set the value at that index in registerArr to the provided value.
            registerArr[index] = value;
            // Exit the function after successfully setting the value.
            return;
        }
        else{
        	std::cout << "Index out of bounds" << std::endl;
        }// If the index is out of bounds, throw an exception.
        //throw std::out_of_range("Index out of bounds");
    }

    // Member function to reset all values in registerArr to zero.
    void resetRegisters() {
        // Iterate through each element in registerArr using a loop.
        for (int i = 0; i < 84; i++) {
            // Set the value of registerArr at the current index (i) to zero.
            registerArr[i] = 0;
        }
    }

private:
	uint32_t registerArr[84];	// Array of all 84 32-bit-Registers as written in the Target5 Module Definition.
								// Individual bits are not defined here.
								// Should be defined in the reader/harvester of the data as to not have to send the definitions with the data.
};

#endif /* SIMPLET5REGISTER_H_ */	// End the check for the macro. This ensures the content in between is only included once.
