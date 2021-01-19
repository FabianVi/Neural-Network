#pragma once

class Hidden;
class Input;
class Vectorx;

float addFunction(Vectorx *v);
float avgFunction(Vectorx* v);
float sineFunction(Vectorx* v);

class Vectorx {

public:
	Vectorx(int count);
	Vectorx(const char* args...);
	Vectorx(const Vectorx& c);

	float* getData();
	float* getData(int i);
	void setData(float* connections);

	~Vectorx();

	float* data;
	int count;
};

class Weights {
public:
	Weights(Vectorx v);
	Weights(const Weights& w);
	~Weights();

	Vectorx*** getConnections();
	Vectorx** getConnections(int i);

	void setConnections(Vectorx*** c);
	void setConnections(int depth, Vectorx** c);
	void setConnections(Weights* w);

	Vectorx* v;
	Vectorx*** connections;

	int depth;

};

class Neuron {
public:
	Neuron(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons);
	Neuron(float (*fireFunc)(Vectorx*), int con, Input** inputNeurons);

	float fire();
	float getState();
	Vectorx* getConnections();
	void setConnections(Vectorx *c);
	void updateConnections(float bias=0.1f);

	~Neuron();

protected:
	float value = 0;
	float (*fireFunc)(Vectorx*);
	Vectorx* connections;
	Hidden** nextNeurons;
	Input** inputNeurons;
};

class Input {
public:
	Input(float value=1.0f);
	void setState(float value);
	float fire();

protected:
	float value = 0;
};

class Hidden : public Neuron {

public:
	Hidden(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons);
	Hidden(float (*fireFunc)(Vectorx*), int con, Input** inputNeurons);
};

class Output : public Neuron {
public:
	Output(float (*fireFunc)(Vectorx*), int con, Hidden** nextNeurons);
};

class Network {

public:
	Network(int in, Vectorx *hidden,int out);

	Vectorx* evaluate(Vectorx* input);
	Weights* getWeights();
	void setWeights(Weights *w);
	void updateWeights(float bias = 0.1f);

	~Network();

private:
	Input** InputNeuron;
	Hidden*** HiddenNeuron;
	Output** OutputNeuron;

	int inputCount, outputCount;
	Vectorx* hidden;
	Vectorx* output;
	Weights* weights;
};

