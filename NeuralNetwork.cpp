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

Vectorx::Vectorx(const char* args...) {

	this->count = 0;

	const char* copy = args;

	while (*copy != '\0') {
		this->count++;
		copy++;
	}

	data = new float[count];

	va_list arglist;
	va_start(arglist, args);

	for(int i=0; i<this->count;i++, args++) 
		if (*args == 'd') {
			data[i] = float(va_arg(arglist, int));
		} else if (*args == 'f') {
			data[i] = va_arg(arglist, float);
		}

	va_end(arglist);
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
		connections[i] = new Vectorx* [size_t(this->v->data[i+1])];

		for (int j = 0; j < v.data[i+1]; j++)
			connections[i][j] = new Vectorx(int(v.data[i]));
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
		connections[i] = new Vectorx *[size_t(w.v->data[i+1])];
		for (int j = 0; j < v->data[i+1]; j++)
			connections[i][j] = new Vectorx(*w.connections[i][j]);
	}
}

Vectorx*** Weights::getConnections() {
	return connections;
}

Vectorx** Weights::getConnections(int i) {

	if (i >= depth)
		return nullptr;

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
			input->data[i] = inputNeurons[i]->fire() * connections->data[i];

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
Network::Network(int in, Vectorx* hidden, int out) {

	srand((unsigned int)(time(NULL)));

	inputCount = in;
	this->hidden = hidden;
	outputCount = out;

	InputNeuron = new Input*[in];

	for (int i = 0; i < in; i++)
		InputNeuron[i] = new Input(10.0f);


	HiddenNeuron = new Hidden**[this->hidden->count];

	for (int i = 0; i < this->hidden->count; i++) {
		HiddenNeuron[i] = new Hidden * [size_t(this->hidden->data[i])];

		for (int j = 0; j < hidden->data[i]; j++)
			if (i == 0)
				HiddenNeuron[i][j] = new Hidden(&sineFunction, in, InputNeuron);
			else
				HiddenNeuron[i][j] = new Hidden(&sineFunction, int(this->hidden->data[i - 1]), HiddenNeuron[i - 1]);
	}



	OutputNeuron = new Output*[out];

	for (int i = 0; i < out; i++)
		OutputNeuron[i] = new Output(&addFunction, this->hidden->count, HiddenNeuron[this->hidden->count - 1]);

	Vectorx *v = new Vectorx(2 + this->hidden->count);

	v->data[0] = float(inputCount);

	for (int i = 1; i < hidden->count - 1; i++)
		v->data[i] = this->hidden->data[i];

	v->data[this->hidden->count-1] = float(outputCount);

	weights = new Weights(*v);
	output = new Vectorx(outputCount);

	delete v;
}

Vectorx* Network::evaluate(Vectorx* input) {

	for (int i = 0; i < this->inputCount; i++)
		InputNeuron[i]->setState(input->data[i]);

	for (int i = 0; i < this->outputCount; i++)
		output->data[i] = OutputNeuron[i]->fire();

	return output;
}

Weights* Network::getWeights() {

	for (int i = 0; i < hidden->count; i++)
		for (int j = 0; j < hidden->data[i]; j++)
			weights->connections[i][j]=HiddenNeuron[i][j]->getConnections();

	for (int i = 0; i < outputCount; i++)
		weights->connections[hidden->count-1][i] = OutputNeuron[i]->getConnections();

	return weights;
}

void Network::setWeights(Weights* w) {
	weights->setConnections(w);
}

void Network::updateWeights(float bias) {

	for (int i = 0; i < hidden->count; i++)
		for (int j = 0; j < hidden->data[i]; j++)
			HiddenNeuron[i][j]->updateConnections(bias);

	for (int i = 0; i < outputCount; i++)
		OutputNeuron[i]->updateConnections(bias);
}

Network::~Network() {
	delete[] InputNeuron;
	delete[] OutputNeuron;
	delete[] HiddenNeuron;

	delete output;
	delete weights;
}

