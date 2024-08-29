/*
 * SimpleT5Register.cpp
 *
 *      Author: sctsim
 */
#include "SimpleT5Register.h"

SimpleT5Register::SimpleT5Register() {
	// nothing-to-do Auto-generated constructor stub
}

SimpleT5Register::~SimpleT5Register() {
	// nothing-to-do Auto-generated destructor stub
}

SimpleT5Register& SimpleT5Register::operator=(SimpleT5Register &&other) {
    if (this != &other) {
        for (size_t i = 0; i < 84; ++i) {
            registerArr[i] = other.registerArr[i];
        }
    }
    return *this;
}

SimpleT5Register& SimpleT5Register::operator=(const SimpleT5Register &other) {
    if (this != &other) {
        for (size_t i = 0; i < 84; ++i) {
            registerArr[i] = other.registerArr[i];
        }
    }
    return *this;
}

SimpleT5Register::SimpleT5Register(SimpleT5Register &&other) {
    for (size_t i = 0; i < 84; ++i) {
        registerArr[i] = other.registerArr[i];
    }
}

SimpleT5Register::SimpleT5Register(const SimpleT5Register &other) {
    for (size_t i = 0; i < 84; ++i) {
        registerArr[i] = other.registerArr[i];
    }
}

