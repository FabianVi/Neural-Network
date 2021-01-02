#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "NeuralNetwork.h"

// Fire Functions
float addFunction(float states[], int size) {
	float sum = 0;

	for (int i=0; i<size;i++)
		sum += states[i];

	return sum;
}


float avgFunction(float states[], int size) {

	float avg = 0;

	for (int i = 0; i < size; i++)
		avg += states[i];

	return avg / float(size);
}

float sineFunction(float states[], int size) {
	return sin(addFunction(states,size));
}

//Neurons
Neuron::Neuron(float (*fireFunc)(float[], int), int con, Hidden** nextNeurons) {

	this->fireFunc = fireFunc;
	this->nextNeurons = nextNeurons;
	this->inputNeurons = nullptr;

	connections = new float[con];
	connectionCount = con;

	for (int i = 0; i < con; i++) 
		connections[i] = 1.0f;

	this->updateConnections();
};

Neuron::Neuron(float (*fireFunc)(float[], int), int con, Input** inputNeurons) {
	this->fireFunc = fireFunc;

	this->nextNeurons = nullptr;
	this->inputNeurons = inputNeurons;

	connections = new float[con];
	connectionCount = con;

	for (int i = 0; i < con; i++)
		connections[i] = 1.0f;
}

float Neuron::fire() {

	float* input = new float[connectionCount];

	if (inputNeurons) {
		for (int i = 0; i < connectionCount; i++)
			input[i] = inputNeurons[i]->fire();
	} else {
		for (int i = 0; i < connectionCount; i++)
			input[i] = nextNeurons[i]->fire();
	}


	for (int i = 0; i < connectionCount; i++) 
		input[i] *= connections[i];

	value = (*fireFunc)(input,connectionCount);

	delete[] input;

	return this->value;
}

float Neuron::getState() {
	return this->value;
}

Neuron::~Neuron() {
	delete[] connections;
}

float* Neuron::getConnections() {
	return this->connections;
}

void Neuron::setConnections(float* connections) {
	for (int i = 0; i < connectionCount; i++) 
		this->connections[i] = connections[i];
}

void Neuron::updateConnections(float bias) {

	for (int i = 0; i < connectionCount; i++) 
		connections[i] += float(rand() % 10 + 1) * bias * ((rand() % 2 == 1) ? -1.0f : 1.0f);
}

//Input Neuron
Input::Input(float value) : value(value) {};

void Input::setState(float value) {
	this->value = value;
}

float Input::fire() {
	return value;
}

//Hidden Neuron
Hidden::Hidden( float (*fireFunc)(float[],int), int con, Hidden** nextNeurons) : Neuron(fireFunc,con, nextNeurons) { };
Hidden::Hidden(float (*fireFunc)(float[], int), int con, Input** inputNeurons) : Neuron(fireFunc, con, inputNeurons) { };

//Output Neuron
Output::Output(float (*fireFunc)(float[], int), int con, Hidden** nextNeurons) : Neuron(fireFunc, con, nextNeurons) { };


//Network
Network::Network(int in, int hidden,int out) {

	srand(unsigned int(time(NULL)));

	std::cout << "init" << std::endl;

	input = in;
	this->hidden = hidden;
	output = out;

	InputNeuron = new Input*[in];
	HiddenNeuron = new Hidden*[hidden];
	OutputNeuron = new Output*[out];

	for (int i = 0; i < in; i++)
		InputNeuron[i] = new Input(10.0f);

	for (int i = 0; i < hidden; i++)
		HiddenNeuron[i] = new Hidden(&sineFunction,in, InputNeuron);

	for (int i = 0; i < out; i++)
		OutputNeuron[i] = new Output(&addFunction,hidden, HiddenNeuron);

	outputVals = new float[this->output];

	connections = new float** [2];
	connections[0] = new float* [hidden];
	connections[1] = new float* [output];
}

float* Network::evaluate(float* input) {

	for (int i = 0; i < this->input; i++)
		InputNeuron[i]->setState(input[i]);

	for (int i = 0; i < this->output; i++)
		outputVals[i] = OutputNeuron[i]->fire();

	return outputVals;
}

float*** Network::getConnections() {

	for (int i = 0; i < hidden; i++)
		connections[0][i] = HiddenNeuron[i]->getConnections();

	for (int i = 0; i < output; i++)
		connections[1][i] = OutputNeuron[i]->getConnections();

	return connections;
}

void Network::setConnections(float*** connections) {

	for (int i = 0; i < hidden; i++)
		HiddenNeuron[i]->setConnections(connections[0][i]);

	for (int i = 0; i < output; i++)
		OutputNeuron[i]->setConnections(connections[1][i]);
}

void Network::updateConnections(float bias) {
	for (int i = 0; i < hidden; i++)
		HiddenNeuron[i]->updateConnections(bias);

	for (int i = 0; i < output; i++)
		OutputNeuron[i]->updateConnections(bias);
}

Network::~Network() {
	delete[] InputNeuron;
	delete[] OutputNeuron;
	delete[] HiddenNeuron;

	delete[] outputVals;
}