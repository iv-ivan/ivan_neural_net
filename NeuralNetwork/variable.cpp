//
//  variable.cpp
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 18/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#include "variable.h"

using namespace std;

float TSimpleVariable::Calculate() const {
    return Value;
}

TSimpleVariable::TSimpleVariable(float value)
    : Value(value)
{}

void TSimpleVariable::SetValue(float value) {
    Value = value;
}

shared_ptr<IVariable> TSimpleVariable::Derivative(shared_ptr<IVariable> x) const {
    if (this == dynamic_cast<TSimpleVariable*>(x.get())) {
        return make_shared<TSimpleVariable>(1);
    } else {
        return make_shared<TSimpleVariable>(0);
    }
}

float TSimpleVariable::GetValue() const {
    return  Value;
}
