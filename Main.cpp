#include <iostream>
#include <math.h>
#include "NeuralNetwork.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include "windows.h"
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD events;
	INPUT_RECORD buffer;
	#define WINDOWS
#elif __linux__

#elif __unix__

#elif defined(_POSIX_VERSION)

#else
#error "Unknown compiler"
#endif

#define NetworkCount 200

/*
* Ruleset
* 
* player
*	+1 ... X (The new Network)
*	-1 ... O (The "best" Network)
*	 0 ...   nothing empty cell
* 
* return:
*	0 ... lost / wrong move
*	1 ... next
*	2 ... won
*/

int play(Vectorx &field,Vectorx *input,float player) {

	int n = -1;

	for (int i = 0; i < input->count; i++)
		if (input->data[i] >= 0.0f)
			n = i;

	if (n == -1)
		return 0;

	if (!(n < field.count && n >= 0))
		return 0;

	if(field.data[n]!=0)
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
				field.data[i + (j + 1) * length] == player) return 2;		//vertical

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i+1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i-1  + (j + 1) * length] == player) return 2;	//accross left right

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i-1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i+1 + (j + 1) * length] == player) return 2;		//accross right left
		}

	return 1;
}

int play(Vectorx& field,int n, float player) {

	if (!(n < field.count && n >= 0))
		return 0;

	if (field.data[n] != 0)
		return 0;

	field.data[n] = player;

	int length = sqrt(field.count);

	for (int i = 0; i < length; i++)
		for (int j = 0; j < length; j++)
		{

			if (i >= 1 && i <= length - 2 &&
				field.data[i - 1 + j * length] == player &&
				field.data[i + j * length] == player &&
				field.data[i + 1 + j * length] == player) return 2;			// horizontal

			if (j >= 1 && j <= length - 2 &&
				field.data[(i + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i + (j + 1) * length] == player) return 2;		//vertical

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i + 1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i - 1 + (j + 1) * length] == player) return 2;	//accross left right

			if (j >= 1 && j <= length - 2 && i >= 1 && i <= length - 2 &&
				field.data[(i - 1 + (j - 1) * length)] == player &&
				field.data[i + j * length] == player &&
				field.data[i + 1 + (j + 1) * length] == player) return 2;		//accross right left
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

int main(int args, char* arg[]) {

	//Example: TikTakToe 3x3
	int length = 3,
		size = length * length,
		max = -10,
		max_this = -10,
		n = 0,
		wins = 0,
		wins_opponent=0,
		adj=0,
		drawC=0;


	float learn = 0.01f, avg = 0;

	//Initialise Network and Fields
	Network* networkM = new Network(size, new Vectorx("dd", 9,9), size);

	Network** networks = new Network * [NetworkCount];
	Vectorx* playfield = new Vectorx(size);
	Vectorx* display = new Vectorx(size);

	Vectorx* score = new Vectorx(NetworkCount);
	Weights* w;

	for (int i = 0; i < NetworkCount; i++)
	{
		networks[i] = new Network(size, new Vectorx("dd", 9,9), size);
		networks[i]->updateWeights(1.0f);
	}

	// learning loop
	while(true) {

		max_this = -10;
		wins = 0;
		wins_opponent = 0;
		avg = 0;
		adj = 0;
		drawC = 0;

		for (int i = 0; i < NetworkCount; i++) {

			// clearing the playfield
			for (int j = 0; j < size; j++)
				playfield->data[j] = 0.0f;

			// seting the first move
			playfield->data[rand()%size] = -1.0f;

			score->data[i] = 1;

			for (int j = 0; j < size / 2 + 1 ; j++, score->data[i] *= 3)
			{
				//Rules what is good and what is bad
				int status = play(*playfield, networks[i]->evaluate(playfield), 1.0f);

				// win
				if (status == 0) {
					score->data[i] -= 20;
					break;
				}
				if (status == 2) {
					wins++;
					score->data[i] += 20;
					break;
				}

				//learning the opponent to place without error (staying in the field)
				do {
					status = play(*playfield, networkM->evaluate(playfield), -1.0f);

					if (status == 0)
						networkM->updateWeights(learn);

				} while (status==0);

				if (status == 2) {
					wins_opponent++;
					break;
				}

				// draw 
				if (j == size / 2-1)
				{
					score->data[i] += 5;
					drawC++;
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

			// finding the best of allsetWeights
			if (max < score->data[i]) {
				max = score->data[i];
				networkM->setWeights(networks[i]->getWeights());
			}

		}

		avg /= float(NetworkCount);

		w = networkM->getWeights();

		//seting the best one for the next round, in order to have in every round a "good one"
		networks[0]->setWeights(w);

		//replacing the bad networks "bad" => everything below or equal avg
		for (int i = 1; i < NetworkCount; i++) {

			if (score->data[i] <= avg) {
				networks[i]->setWeights(w);
				networks[i]->updateWeights(learn);
				adj++;
			}

		}

		// Displaying data
		std::cout << "No. " << n << " learnfactor: " << learn <<  " avg score : " << avg << std::endl;
		std::cout << "Weights adjusted: " << adj << std::endl;
		std::cout << "Round Max score: " << max_this << " Total Max score: " << max << std::endl;
		std::cout << "losses: " << NetworkCount-wins << " / wins: " << wins << " / Draws:" << drawC <<std::endl;
		std::cout << "wins-opponent: " << wins_opponent << std::endl;
		std::cout << "Best play of this iteration" << std::endl;

		draw(display);

		n++;

//playing against the KI if a key is pressed !!only Windows implementation!!
#ifdef WINDOWS
		PeekConsoleInput(handle, &buffer, 1, &events);
		if (events > 0) {

			int input, state;
			for (int j = 0; j < size; j++)
				playfield->data[j] = 0.0f;

			do {
				std::cout << "input:";
				std::cin >> input;

				state = play(*playfield, input, -1);

				std::cout << "state: " << state << std::endl;
				draw(playfield);

				state = play(*playfield, networkM->evaluate(playfield), 1);
				std::cout << "state: " << state << std::endl;
				draw(playfield);
			} while (state != 0);
		}
#endif
	}
}