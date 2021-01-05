#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include "NeuralNetwork.h"

// Fire Functions
float addFunction(Vectorx *v) {
	float sum = 0;

	for (int i=0; i<v->count;i++)
		sum += v->data[i];

	return sum;
}


float avgFunction(Vectorx *v) {

	float avg = 0;

	for (int i = 0; i < v->count; i++)
		avg += v->data[i];

	return avg / float(v->count);
}

float sineFunction(Vectorx *v) {
	return sin(addFunction(v));
}

//Vectorx
Vectorx::Vectorx(int count) {

	data = new float[count];

	for (int i = 0; i < count; i++)
		data[i] = 1.0f;

	this->count = count;
}

Vectorx::Vectorx(const Vectorx &c) {

	this->count = c.count;
	data = new float[count];

	for (int i = 0; i < count; i++)
		data[i] = c.data[i];
}

Vectorx::~Vectorx() {
	delete[] data;
}

float* Vectorx::getData() {
	return data;
}

void Vectorx::setData(float* connections) {
	for (int i = 0; i < count; i++)
		this->data[i] = connections[i];
}

float* Vectorx::getData(int i) {
	if (i >= count)
		return &data[0];

	return &data[i];
}

//Weights
Weights::Weights(Vectorx v) {

	this->depth = v.count-1;
	this->v = new Vectorx(v);

	connections = new Vectorx**[depth];

	for (int i = 0; i < this->depth; i++) {
		connections[i] = new Vectorx* [this->v->data[i+1]];
		for (int j = 0; j < v.data[i+1]; j++)
			connections[i][j] = new Vectorx(v.data[i]);
	}
}

Weights::~Weights() {

	delete v;
	delete[] connections;

}

Weights::Weights(const Weights &w) {

	this->depth = w.depth;
	this->v = new Vectorx(*w.v);

	connections = new Vectorx** [w.depth];

	for (int i = 0; i < w.depth; i++) {
		connections[i] = new Vectorx *[w.v->data[i+1]];
		for (int j = 0; j < v->data[i+1]; j++)
			connections[i][j] = new Vectorx(*w.connections[i][j]);
	}
}

Vectorx*** Weights::getConnections() {
	return connections;
}

Vectorx** Weights::getConnections(int i) {

	if(i>=depth)

	return connections[i];
}

void Weights::setConnections(Vectorx*** c) {
	for (int i = 0; i < depth; i++)
		for (int j = 0; j < v->data[i+1]; j++)
			for (int k = 0; k < connections[i][j]->count; k++)
				connections[i][j]->data[k] = c[i][j]->data[k];
}

void Weights::setConnections(Weights* w) {
	for (int i = 0; i < depth; i++)
		for (int j = 0; j < v->data[i+1]; j++)
			for (int k = 0; k < connections[i][j]->count; k++)
				this->connections[i][j]->data[k] = w->connections[i][j]->data[k];
}

void Weights::setConnections(int depth, Vectorx** c) {

	if (this->depth >= depth)
		return;

	for (int j = 0; j < v->data[depth+1]; j++)
		for (int k = 0; k < connections[depth][j]->count; k++)
			connections[depth][j]->data[k] = c[depth][j].data[k];
}

//Neurons
Neuron::Neuron(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons) {
	this->fireFunc = fireFunc;

	this->nextNeurons = nextNeurons;
	this->inputNeurons = nullptr;

	connections = new Vectorx(con);
};

Neuron::Neuron(float (*fireFunc)(Vectorx*), int con, Input** inputNeurons) {
	this->fireFunc = fireFunc;

	this->nextNeurons = nullptr;
	this->inputNeurons = inputNeurons;

	connections = new Vectorx(con);
}

float Neuron::fire() {

	Vectorx *input = new Vectorx(connections->count);

	if (inputNeurons) {
		for (int i = 0; i < connections->count; i++)
			input->data[i] = inputNeurons[i]->fire()*connections->data[i];

	} else {
		for (int i = 0; i < connections->count; i++)
			input->data[i] = nextNeurons[i]->fire() * connections->data[i];
	}

	value = (*fireFunc)(input);

	delete input;

	return this->value;
}

float Neuron::getState() {
	return this->value;
}

Neuron::~Neuron() {
	delete connections;
}

Vectorx* Neuron::getConnections() {
	return this->connections;
}

void Neuron::setConnections(Vectorx *c) {
	for (int i = 0; i < connections->count; i++)
		connections->data[i] = c->data[i];
}

void Neuron::updateConnections(float bias) {

	for (int i = 0; i < connections->count ; i++) 
		connections->data[i] += float(rand() % 10 + 1) * bias * ((rand() % 2 == 1) ? -1.0f : 1.0f);
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
Hidden::Hidden(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons) : Neuron(fireFunc,con, nextNeurons) { };
Hidden::Hidden(float (*fireFunc)(Vectorx*), int con, Input** inputNeurons) : Neuron(fireFunc, con, inputNeurons) { };

//Output Neuron
Output::Output(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons) : Neuron(fireFunc, con, nextNeurons) { };


//Network
Network::Network(int in, int hidden, int out) {

	srand(unsigned int(time(NULL)));

	inputCount = in;
	hiddenCount = hidden;
	outputCount = out;

	InputNeuron = new Input*[in];
	HiddenNeuron = new Hidden*[hidden];
	OutputNeuron = new Output*[out];

	for (int i = 0; i < in; i++)
		InputNeuron[i] = new Input(10.0f);

	for (int i = 0; i < hidden; i++)
		HiddenNeuron[i] = new Hidden(&sineFunction,in, InputNeuron);

	for (int i = 0; i < out; i++)
		OutputNeuron[i] = new Output(&addFunction,hidden, HiddenNeuron);

	output = new Vectorx(outputCount);

	Vectorx *v = new Vectorx(3);

	v->data[0] = inputCount;
	v->data[1] = hiddenCount;
	v->data[2] = outputCount;

	weights = new Weights(*v);

	delete v;
}

Vectorx* Network::evaluate(Vectorx* input) {

	for (int i = 0; i < this->inputCount; i++)
		InputNeuron[i]->setState(input->data[i]);

	for (int i = 0; i < this->outputCount; i++)
		output->data[i] = OutputNeuron[i]->fire();

	return output;
}

Weights* Network::getConnections() {

	for (int i = 0; i < hiddenCount; i++)
		weights->connections[0][i]=HiddenNeuron[i]->getConnections();

	for (int i = 0; i < outputCount; i++)
		weights->connections[1][i] = OutputNeuron[i]->getConnections();

	return weights;
}

void Network::setConnections(Weights* w) {
	weights->setConnections(w);
}

void Network::updateConnections(float bias) {
	for (int i = 0; i < hiddenCount; i++)
		HiddenNeuron[i]->updateConnections(bias);

	for (int i = 0; i < outputCount; i++)
		OutputNeuron[i]->updateConnections(bias);
}

Network::~Network() {
	delete[] OutputNeuron;
	delete[] HiddenNeuron;

	delete output;
	delete weights;
}

