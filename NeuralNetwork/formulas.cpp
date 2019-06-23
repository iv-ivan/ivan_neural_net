//
//  formulas.cpp
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 18/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#include "formulas.h"

using namespace std;

TSum::TSum(shared_ptr<IVariable> a, shared_ptr<IVariable> b)
    : A(a)
    , B(b)
{}

float TSum::Calculate() const {
    return A->Calculate() + B->Calculate();
}

shared_ptr<IVariable> TSum::Derivative(shared_ptr<IVariable> x) const {
    if (this == dynamic_cast<TSum*>(x.get())) {
        return make_shared<TSimpleVariable>(1);
    }
    return make_shared<TSum>(A->Derivative(x), B->Derivative(x));
}

TSub::TSub(shared_ptr<IVariable> a, shared_ptr<IVariable> b)
    : A(a)
    , B(b)
{}

float TSub::Calculate() const {
    return A->Calculate() - B->Calculate();
}

shared_ptr<IVariable> TSub::Derivative(shared_ptr<IVariable> x) const {
    if (this == dynamic_cast<TSub*>(x.get())) {
        return make_shared<TSimpleVariable>(1);
    }
    return make_shared<TSub>(A->Derivative(x), B->Derivative(x));
}

TDiv::TDiv(shared_ptr<IVariable> a, shared_ptr<IVariable> b)
    : A(a)
    , B(b)
{}

float TDiv::Calculate() const {
    return A->Calculate() / B->Calculate();
}

shared_ptr<IVariable> TDiv::Derivative(shared_ptr<IVariable> x) const {
    if (this == dynamic_cast<TDiv*>(x.get())) {
        return make_shared<TSimpleVariable>(1);
    }
    shared_ptr<IVariable> dA = A->Derivative(x);
    shared_ptr<IVariable> dB = B->Derivative(x);
    return make_shared<TDiv>(make_shared<TSub>(make_shared<TMult>(dA, B), make_shared<TMult>(A, dB)), make_shared<TMult>(B, B));
}

TMult::TMult(shared_ptr<IVariable> a, shared_ptr<IVariable> b)
    : A(a)
    , B(b)
{}

float TMult::Calculate() const {
    return A->Calculate() * B->Calculate();
}

shared_ptr<IVariable> TMult::Derivative(shared_ptr<IVariable> x) const {
    if (this == dynamic_cast<TMult*>(x.get())) {
        return make_shared<TSimpleVariable>(1);
    }
    shared_ptr<IVariable> dA = A->Derivative(x);
    shared_ptr<IVariable> dB = B->Derivative(x);
    return make_shared<TSum>(make_shared<TMult>(dA, B), make_shared<TMult>(A, dB));
}
