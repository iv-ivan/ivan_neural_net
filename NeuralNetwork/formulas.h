//
//  formulas.h
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 18/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#ifndef formulas_h
#define formulas_h

#include "variable.h"

class TSum : public IFormula {
public:
    TSum(std::shared_ptr<IVariable> a, std::shared_ptr<IVariable> b);
    float Calculate() const override;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override;
    virtual ~TSum() = default;
private:
    std::shared_ptr<IVariable> A;
    std::shared_ptr<IVariable> B;
};

class TSub : public IFormula {
public:
    TSub(std::shared_ptr<IVariable> a, std::shared_ptr<IVariable> b);
    float Calculate() const override;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override;
    virtual ~TSub() = default;
private:
    std::shared_ptr<IVariable> A;
    std::shared_ptr<IVariable> B;
};

class TMult : public IFormula {
public:
    TMult(std::shared_ptr<IVariable> a, std::shared_ptr<IVariable> b);
    float Calculate() const override;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override;
    virtual ~TMult() = default;
private:
    std::shared_ptr<IVariable> A;
    std::shared_ptr<IVariable> B;
};

class TDiv : public IFormula {
public:
    TDiv(std::shared_ptr<IVariable> a, std::shared_ptr<IVariable> b);
    float Calculate() const override;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override;
    virtual ~TDiv() = default;
private:
    std::shared_ptr<IVariable> A;
    std::shared_ptr<IVariable> B;
};

#endif /* formulas_h */
