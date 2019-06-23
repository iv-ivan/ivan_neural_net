//
//  layer.cpp
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 22/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#include <stdio.h>
#include "layer.h"
#include "formulas.h"
#include <future>

using namespace std;

TLayer::TLayer(const vector<shared_ptr<IVariable>>& input, size_t outputSize, float weight)
    : Input(input)
    , PrevLayer(nullptr)
{
    ParamsInputToOutput.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        for (size_t j = 0; j < outputSize; ++j) {
            ParamsInputToOutput[i].push_back(make_shared<TSimpleVariable>(weight));
        }
    }
    for (size_t i = 0; i < outputSize; ++i) {
        shared_ptr<IVariable> accumulate = make_shared<TSimpleVariable>(0);
        for (size_t j = 0; j < input.size(); ++j) {
            accumulate = make_shared<TSum>(make_shared<TMult>(Input[j], ParamsInputToOutput[j][i]), accumulate);
        }
        Output.push_back(accumulate);
    }
}

TLayer::TLayer(TLayer& inputLayer, size_t outputSize, float weight)
    : TLayer(inputLayer.GetOutput(), outputSize, weight)
{
    PrevLayer = &inputLayer;
}

const vector<shared_ptr<IVariable>>& TLayer::GetOutput() const {
    return Output;
}

void TLayer::Step(const vector<shared_ptr<IVariable>>& dOutputHeavy, float alpha) {
    vector<shared_ptr<IVariable>> dOutput;
    dOutput.resize(dOutputHeavy.size());
    for (size_t i = 0; i < dOutputHeavy.size(); ++i) {
        dOutput[i] = make_shared<TSimpleVariable>(dOutputHeavy[i]->Calculate());
    }

    vector<vector<shared_ptr<IVariable>>> dFdParams;
    vector<shared_ptr<IVariable>> dFdInputs;
    dFdParams.resize(ParamsInputToOutput.size());
    for (size_t i = 0; i < ParamsInputToOutput.size(); ++i) {
        dFdParams[i].resize(ParamsInputToOutput[0].size());
        for (size_t j = 0; j < ParamsInputToOutput[0].size(); ++j) {
            dFdParams[i][j] = make_shared<TSimpleVariable>(0);
        }
    }
    if (PrevLayer) {
        dFdInputs.resize(Input.size());
        for (size_t i = 0; i < Input.size(); ++i) {
            dFdInputs[i] = make_shared<TSimpleVariable>(0);
        }
    }

    for (size_t i = 0; i < Output.size(); ++i) {
        if (PrevLayer) {
            for (size_t j = 0; j < Input.size(); ++j) {
                dFdInputs[j] = make_shared<TSum>(dFdInputs[j], make_shared<TMult>(dOutput[i], Output[i]->Derivative(Input[j])));
            }
        }
        for (size_t j = 0; j < ParamsInputToOutput.size(); ++j) {
            for (size_t k = 0; k < ParamsInputToOutput[0].size(); ++k) {
                dFdParams[j][k] = make_shared<TSum>(dFdParams[j][k], make_shared<TMult>(dOutput[i], Output[i]->Derivative(ParamsInputToOutput[j][k])));
            }
        }
    }

    for (size_t i = 0; i < ParamsInputToOutput.size(); ++i) {
        for (size_t j = 0; j < ParamsInputToOutput[0].size(); ++j) {
            ParamsInputToOutput[i][j]->SetValue(ParamsInputToOutput[i][j]->GetValue() - alpha * dFdParams[i][j]->Calculate());
        }
    }
    
    if (PrevLayer) {
        PrevLayer->Step(dFdInputs, alpha);
    }
}

int c(int z) {
    return z;
}
void TLayer::StepParallel(const vector<shared_ptr<IVariable>>& dOutputHeavy, float alpha, TThreadPool& pool) {
    vector<shared_ptr<IVariable>> dOutput;
    dOutput.resize(dOutputHeavy.size());
    vector<std::future<size_t>> futures;
    for (size_t i = 0; i < dOutputHeavy.size(); ++i) {
        futures.push_back(pool.Submit([&dOutput, &dOutputHeavy, i] () -> size_t {
            dOutput[i] = make_shared<TSimpleVariable>(dOutputHeavy[i]->Calculate());
            return i;
        }));
    }
    
    vector<vector<shared_ptr<IVariable>>> dFdParams;
    vector<shared_ptr<IVariable>> dFdInputs;
    dFdParams.resize(ParamsInputToOutput.size());
    for (size_t i = 0; i < ParamsInputToOutput.size(); ++i) {
        dFdParams[i].resize(ParamsInputToOutput[0].size());
        for (size_t j = 0; j < ParamsInputToOutput[0].size(); ++j) {
            futures.push_back(pool.Submit([&dFdParams, i, j] () -> size_t {
                dFdParams[i][j] = make_shared<TSimpleVariable>(0);
                return i;
            }));
        }
    }
    if (PrevLayer) {
        dFdInputs.resize(Input.size());
        for (size_t i = 0; i < Input.size(); ++i) {
            futures.push_back(pool.Submit([&dFdInputs, i] () -> size_t {
                dFdInputs[i] = make_shared<TSimpleVariable>(0);
                return i;
            }));
        }
    }

    for (size_t i = 0; i < futures.size(); ++i) {
        futures[i].wait();
    }
    futures.clear();

    for (size_t i = 0; i < Output.size(); ++i) {
        if (PrevLayer) {
            for (size_t j = 0; j < Input.size(); ++j) {
                futures.push_back(pool.Submit([&dFdInputs, &dOutput, this, i, j] () -> size_t {
                    dFdInputs[j] = make_shared<TSum>(dFdInputs[j], make_shared<TMult>(dOutput[i], Output[i]->Derivative(Input[j])));
                    return i;
                }));
            }
        }
        for (size_t j = 0; j < ParamsInputToOutput.size(); ++j) {
            for (size_t k = 0; k < ParamsInputToOutput[0].size(); ++k) {
                futures.push_back(pool.Submit([&dFdParams, &dOutput, this, i, j, k] () -> size_t {
                    dFdParams[j][k] = make_shared<TSum>(dFdParams[j][k], make_shared<TMult>(dOutput[i], Output[i]->Derivative(ParamsInputToOutput[j][k])));
                    return i;
                }));
            }
        }
    }

    for (size_t i = 0; i < futures.size(); ++i) {
        futures[i].wait();
    }
    futures.clear();
    
    for (size_t i = 0; i < ParamsInputToOutput.size(); ++i) {
        for (size_t j = 0; j < ParamsInputToOutput[0].size(); ++j) {
            futures.push_back(pool.Submit([&dFdParams, this, i, j, alpha] () -> size_t {
                ParamsInputToOutput[i][j]->SetValue(ParamsInputToOutput[i][j]->GetValue() - alpha * dFdParams[i][j]->Calculate());
                return i;
            }));
        }
    }
 
    for (size_t i = 0; i < futures.size(); ++i) {
        futures[i].wait();
    }
    futures.clear();

    if (PrevLayer) {
        PrevLayer->StepParallel(dFdInputs, alpha, pool);
    }
    //Step(dOutputHeavy, alpha);
}
