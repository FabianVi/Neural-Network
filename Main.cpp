#include <iostream>
#include "NeuralNetwork.h"

#define NetworkCount 1000

int main(char** arg, int args) {

	float input[3][9] = { 
		{ 
		  1.0f , 0.0f , 0.0f , 
		  0.0f , 0.0f , 0.0f ,
		  0.0f , 0.0f , 0.0f 
		},
		{
		  0.0f , 0.0f , 0.0f ,
		  0.0f , 1.0f , 0.0f ,
		  0.0f , 0.0f , 0.0f
		},
		{
		  1.0f , 0.0f , 1.0f ,
		  0.0f , 0.0f , 0.0f ,
		  0.0f , 0.0f , 1.0f
		} 
	};

	float output[3] = {
		5.0f,
		1.0f,
		3.0f,
	};

	Network** networks = new Network * [NetworkCount];

	for (int i = 0; i < NetworkCount; i++) {
		networks[i] = new Network(9, 10, 1);
		networks[i]->updateConnections(0.1f);
	}

	float diff[NetworkCount] = { 0 };
	float min = 99999999999.0f;
	int nextNetwork = 0;
	float*** connections;

	while (min>0.001f) 
	{
		for (int i = 0; i < NetworkCount; i++) {

			diff[i] = 0;

			for(int n=0; n<3;n++)
				diff[i] += sqrt(pow(networks[i]->evaluate(input[n % 3])[0] - output[n % 3],2));

			if (min > diff[i]) {
				min = diff[i];

				nextNetwork = i;
			}
		}

		connections = networks[nextNetwork]->getConnections();

		for (int i = 0; i < NetworkCount; i++) {

			networks[i]->setConnections(connections);
			networks[i]->updateConnections(min/100);

		}

		std::cout << " min: " << min << std::endl;

	}

	for (int i = 0; i < 3; i++)
		std::cout << "Test" << i << " error of " <<  sqrt(pow(networks[nextNetwork]->evaluate(input[i % 3])[0] - output[i % 3], 2)) << std::endl;

}