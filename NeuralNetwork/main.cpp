//
//  main.cpp
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 17/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#include "variable.h"
#include "formulas.h"
#include "layer.h"
#include <iostream>
#include <vector>
#include <chrono>
#include "queue.h"
#include "pool.h"

using namespace std;
using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                                 std::chrono::high_resolution_clock,
                                 std::chrono::steady_clock>;

int main(int argc, const char * argv[]) {
    // test formulas and dx/dy
    /*auto x = make_shared<TSimpleVariable>(1), y = make_shared<TSimpleVariable>(2), z = make_shared<TSimpleVariable>(4);
    auto s = make_shared<TSum>(x, y);
    auto w = make_shared<TMult>(z, s);
    std::cout << w->Calculate() << endl;
    std::cout << w->Derivative(w)->Calculate() << endl;
    std::cout << w->Derivative(z)->Calculate() << endl;
    std::cout << w->Derivative(s)->Calculate() << endl;
    std::cout << w->Derivative(x)->Calculate() << endl;
    std::cout << w->Derivative(y)->Calculate() << endl;
    std::cout << z->Derivative(s)->Calculate() << endl;
    std::cout << z->Derivative(y)->Calculate() << endl;
    std::cout << y->Derivative(y)->Calculate() << endl;*/
    
    // test layers
    std::vector<std::shared_ptr<IVariable>> x;
    for (size_t i = 0; i < 40; ++i) {
        x.push_back(make_shared<TSimpleVariable>(1));
    }
    shared_ptr<TSimpleVariable> y = make_shared<TSimpleVariable>(-2);
    
    TLayer firstLayer(x, 40);
    TLayer secondLayer(firstLayer, 1);
    shared_ptr<IVariable> output = secondLayer.GetOutput()[0];
    cout << "Step 0:" << output->Calculate() << endl;
    
    shared_ptr<IVariable> subloss = make_shared<TSub>(output, y);
    shared_ptr<IVariable> loss = make_shared<TMult>(subloss, subloss);
    vector<shared_ptr<IVariable>> dLossdOutput = {loss->Derivative(output)};
    
    auto pool = TThreadPool();
    const auto begin = Clock::now();
    for (size_t i = 0; i < 10; ++i) {
        //secondLayer.StepParallel(dLossdOutput, 0.00001, pool);
        secondLayer.Step(dLossdOutput, 0.00001);
        cout << "Step " << i + 1 << ": " << output->Calculate() << endl;
    }
    const auto end = Clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;
    //20x20, 10 steps 5597 not parallel, 4338 parallel
    //40x40, 10 steps 117024 not parallel, 62753 parallel
    //test thread-safe queue
    /*TThreadSafeQueue<int> k;
    k.TryPop();
    k.Push(1);
    k.Push(2);
    cout << *k.TryPop() << endl;
    cout << *k.TryPop() << endl;*/
    return 0;
}
