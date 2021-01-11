# Neural Network C++ Library
This Library should help by creating "simple" KI's and should help, geting started with neural networks,
furthermore it should be a light weight c++ library for simple KI implementations.

# Neural Networ concept (1 Hiddenlayer)
![Neural Networ Concept Image](https://upload.wikimedia.org/wikipedia/commons/thumb/3/3d/Neural_network.svg/1920px-Neural_network.svg.png "Neural Networ Concept Image")

# Library capabilities
* Creating a Simple Neural Network e.g.
```
Network n( i , new Vectorx("dd..." , ...) , o)  
/* i ... input neurons
*  new Vectorx("dd..." , ...) ... hidden neurons Multilayer e.g. ("dd" , 10,10)  2 layers with 10 neurons
*  o ... output neurons 
*/

// Functions
Vectorx* evaluate(Vectorx* input);          // evaluates the network with an Input of Type Vectorx
Weights* getWeights();                      // gets the current weights of the Network
void setWeights(Weights *w);                // copys the Weights and assigns them to the Network
void updateWeights(float bias = 0.1f);      // Updates the Weights randomly with the Factor bias
```

* Creating custom networks with all three kinds of Neurons e.g.
```c++
// n ... an integer number, which the programmer chooses
Input in[n];
Hidden hidden[n]  = {Hidden(&func , sizeof(in)/sizeof(Input), &in) ... }
Hidden hidden2[n] = {Hidden(&func , sizeof(hidden)/sizeof(Hidden), &hidden) ... }
Output output[n]  = {Output(&func , sizeof(hidden2)/sizeof(Hidden), &hidden2) ... }
```

# Comming Soon
* adding new learning technics
  * currently weights get updated by a randome generator and over time the NN improves
  * future e.g. backpropergation etc. p. p.
 * ERROR Handling
* saving Weights (connections) to file
* loading inputs and outputs
* creating a more advanced NN with the Network class (Neural Network)
  * loopback neurons, in order to get a kind of memory
  * random or oszillating neurons, in order to get a better learning behaviour
* adding visualization
* multithreading

# Example
## Main.cpp
This File contains a test programm, which showes the curent progress of the project and the power of the Neural Network, 
furthermore it shoulde be a benchmark for the improvement over time.
It trys to learn Tic Tac Toe, by playing against it self.