#pragma once

float addFunction(float states[], int size);
float avgFunction(float states[], int size);
float sineFunction(float states[], int size);

class Hidden;
class Input;

class Neuron {
public:
	Neuron(float (*fireFunc)(float[],int), int con, Hidden** nextNeurons);
	Neuron(float (*fireFunc)(float[], int), int con, Input** inputNeurons);

	float fire();
	float getState();
	float* getConnections();
	void setConnections(float* connections);
	void updateConnections(float bias=0.1f);

	~Neuron();

protected:
	float value = 0;
	float (*fireFunc)(float[],int);
	float* connections;
	int connectionCount;
	Hidden** nextNeurons;
	Input** inputNeurons;
};

class Input {
public:
	Input(float value);
	void setState(float value);
	float fire();

protected:
	float value = 0;
};

class Hidden : public Neuron {

public:
	Hidden(float (*fireFunc)(float[], int), int con, Hidden** nextNeurons);
	Hidden(float (*fireFunc)(float[], int), int con, Input** inputNeurons);
};

class Output : public Neuron {
public:
	Output(float (*fireFunc)(float[], int), int con, Hidden** nextNeurons);
};

class Network {

public:
	Network(int in, int hidden, int out);

	float* evaluate(float* input);
	float*** getConnections();
	void setConnections(float*** connections);
	void updateConnections(float bias = 0.1f);

	~Network();

private:
	Input** InputNeuron;
	Hidden** HiddenNeuron;
	Output** OutputNeuron;

	int hidden, input, output;
	float* outputVals;
	float*** connections;
};