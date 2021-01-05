#include <iostream>
#include "NeuralNetwork.h"

#define NetworkCount 1000

/*
* Ruleset
* 
* player
*	1 ... X (The new Network)
*	2 ... = (The "best" Network)
* 
* return:
*	0 ... lost / wrong move
*	1 ... next
*	2 ... won
*/

int play(Vectorx &field,int n,float player) {

	if (n >= field.count)
		return 0;

	if(field.data[n]>0)
		return 0;

	field.data[n] = player;

	int length = sqrt(field.count);

	for(int i=0; i< length;i++)
		for (int j = 0; j < length; j++)
		{

			if (i >= 1 && i <= length - 2 && 
				field.data[i - 1 + j * length] == player &&
				field.data[i + j * length] == player &&
				field.data[i + 1 + j * length] == player) return 2;			// horizontal

			if (j >= 1 && j <= length - 2 &&
				field.data[(i  + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i + (j + 1) * length] == player) return 2;	//vertical

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i+1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i-1  + (j + 1) * length] == player) return 2;	//accross left right

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i-1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i+1 + (j + 1) * length] == player) return 2;	//accross right left
		}

	return 1;
}


//Draw the Field 
void draw(Vectorx* display) {

	int length = sqrt(display->count);
	int size = display->count;

	for (int o = 0; o < length + 2; o++)
		std::cout << "#";
	std::cout << std::endl;

	for (int k = 0; k < size; k++) {

		if (k % length == 0)
			std::cout << "#";

		std::cout << ((display->data[k] == 0) ? " " : display->data[k] == 1 ? "X" : "O");

		if (k % length == length - 1)
			std::cout << "#" << std::endl;
	}

	for (int o = 0; o < length + 2; o++)
		std::cout << "#";
	std::cout << std::endl;
	std::cout << std::endl;
}



int main(char** arg, int args) {

	//Example: TikTakToe 5x5
	int length = 3,
		size = length * length,
		max = -10,
		max_this = -10,
		n = 0,
		losses = 0,
		wins = 0;

	float learn = 0.10f, avg = 0;

	//Initialise Network and Fields
	Network* networkM = new Network(size, 100, 1);

	Network** networks = new Network * [NetworkCount];
	Vectorx* playfield = new Vectorx(size);
	Vectorx* display = new Vectorx(size);

	Vectorx* score = new Vectorx(NetworkCount);
	Weights* w;

	for (int i = 0; i < NetworkCount; i++)
	{
		networks[i] = new Network(size, 100, 1);
		networks[i]->updateConnections(0.1f);

	}


	//Playing / learning

	while (true) {

		max_this = 0;
		losses = 0;
		wins = 0;
		avg = 0;

		for (int i = 0; i < NetworkCount; i++) {

			for (int j = 0; j < size; j++)
				playfield->data[j] = 0.0f;

			playfield->data[rand()%size] = 2.0f;

			score->data[i] = 0;

			for (int j = 0; j < size / 2 + 1; j++)
			{
				int move1 = int(networks[i]->evaluate(playfield)->data[0]);
				int status = play(*playfield, move1, 1.0f);

				//Rules what is good and what is bad

				if (status == 0) {
					score->data[i] += -10;
					losses++;
					break;
				}
				else if (status == 1) {
					score->data[i] += 10;
				}
				else if (status == 2) {
					wins++;
					score->data[i] += 20;
					break;
				}

				int move2 = int(networkM->evaluate(playfield)->data[0]);
				status = play(*playfield, move1, 1.0f);
				if (status == 0) {

					break;
				}
				else if (status == 2) {
					wins++;
					score->data[i] += 5;
					break;
				}
			}

			avg += score->data[i];

			// finding the best of this round
			if (max_this < score->data[i]) {
				max_this = score->data[i];
				for (int p = 0; p < size; p++)
					display->data[p] = playfield->data[p];
			}

			// finding the best of all
			if (max < score->data[i]) {
				max = score->data[i];
				networkM->setConnections(networks[i]->getConnections());
			}

		}

		avg /= float(NetworkCount);

		w = networkM->getConnections();

		//seting the best one for the next round, in order to have in every round a "good one"
		networks[0]->setConnections(w);

		//replacing the bad networks "bad" => everything below or equal avg
		for (int i = 1; i < NetworkCount; i++) {

			if (score->data[i] <= avg) {
				networks[i]->setConnections(w);
				networks[i]->updateConnections(learn);
			}

		}

		n++;

		// Reducing learn value factor
		if (n % 500 == 0)
			learn /= 1.5;

		// Displaying data
		std::cout << "No. " << n << " learnfactor: " << learn <<  " avg score : " << avg << std::endl;
		std::cout << "Round Max score: " << max_this << " Total Max score: " << max << std::endl;
		std::cout << "losses: " << losses << " /  " << wins << std::endl;
		std::cout << "Best play of this iteration" << std::endl;

		draw(display);
	}
}