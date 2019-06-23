//
//  variable.h
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 18/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#ifndef variable_h
#define variable_h

#include <utility>
#include <memory>

class IVariable {
public:
    virtual float Calculate() const = 0;
    virtual std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const = 0;
    virtual ~IVariable() = default;
};

class TSimpleVariable : public IVariable {
public:
    TSimpleVariable() = default;
    TSimpleVariable(float value);
    void SetValue(float value);
    float GetValue() const;
    float Calculate() const override;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override;
    virtual ~TSimpleVariable() = default;
private:
    float Value;
};

class IFormula : public IVariable {
public:
    float Calculate() const override = 0;
    std::shared_ptr<IVariable> Derivative(std::shared_ptr<IVariable> x) const override = 0;
    virtual ~IFormula() = default;
};

#endif /* variable_h */
