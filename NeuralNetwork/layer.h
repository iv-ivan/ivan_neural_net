//
//  layer.h
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 22/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#ifndef layer_h
#define layer_h

#include "variable.h"
#include <vector>
#include "pool.h"

class TLayer {
public:
    TLayer(const std::vector<std::shared_ptr<IVariable>>& input, size_t outputSize, float weight = 1);
    TLayer(TLayer& inputLayer, size_t outputSize, float weight = 1);
    const std::vector<std::shared_ptr<IVariable>>& GetOutput() const;
    void Step(const std::vector<std::shared_ptr<IVariable>>& dOutput, float alpha);
    void StepParallel(const std::vector<std::shared_ptr<IVariable>>& dOutput, float alpha, TThreadPool& pool);
private:
    const std::vector<std::shared_ptr<IVariable>>& Input;
    std::vector<std::vector<std::shared_ptr<TSimpleVariable>>> ParamsInputToOutput;
    std::vector<std::shared_ptr<IVariable>> Output;
    TLayer* PrevLayer;
};

#endif /* layer_h */
