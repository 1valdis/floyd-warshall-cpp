#include "Graph.h"
#include <limits.h>
#include <iostream>

#define infinity INT_MAX

Graph::Graph(int** adjacencyMatrix, int verticesCount)
{
	this->verticesCount = verticesCount;
	this->adjacencyMatrix = adjacencyMatrix;
	
	//let's allocate memory needed for those arrays described in .h file
	this->distancesMatrixBeforeIteration = new int*[verticesCount];
	this->distancesMatrixAfterIteration = new int*[verticesCount];
	this->predecessorsMatrixBeforeIteration = new int*[verticesCount];
	this->predecessorsMatrixAfterIteration = new int*[verticesCount];
	for (int i = 0; i<verticesCount; i++)
	{
		this->distancesMatrixAfterIteration[i] = new int[verticesCount];
		this->distancesMatrixBeforeIteration[i] = new int[verticesCount];
		this->predecessorsMatrixBeforeIteration[i] = new int[verticesCount];
		this->predecessorsMatrixAfterIteration[i] = new int[verticesCount];
		for (int j = 0; j < verticesCount; j++)
		{
			//by the way we'll fill up initial distances matrix with adjacency matrix values
			distancesMatrixAfterIteration[i][j] = adjacencyMatrix[i][j];
			//and fill up initial predecessors matrix
			if (distancesMatrixAfterIteration[i][j] != infinity)
			{
				predecessorsMatrixAfterIteration[i][j] = i;
			}
			else
			{
				predecessorsMatrixAfterIteration[i][j] = -1;
			}
		}
	}

	//prepare indices for first iteration
	k = -1;
	i = verticesCount - 1;
	j = 0;

	//setting default flag value
	isNotifiedAboutBetterPathFound = false;
}

Graph::~Graph()
{
	//we'll always clean up this mess after us...
	for (int i = 0; i < verticesCount; i++)
	{
		delete[] this->distancesMatrixBeforeIteration[i];
		delete[] this->distancesMatrixAfterIteration[i];
		delete[] this->predecessorsMatrixBeforeIteration[i];
		delete[] this->predecessorsMatrixAfterIteration[i];
	}
	delete[] this->distancesMatrixBeforeIteration;
	delete[] this->distancesMatrixAfterIteration;
	delete[] this->predecessorsMatrixBeforeIteration;
	delete[] this->predecessorsMatrixAfterIteration;
}

Graph::FloydStepResult Graph::floydStep()
{
	//if k is equal to count of vertices it means that we're done
	if (k==verticesCount)
		return FloydStepResult::EndOfAlgorithm;

	//remember, we don't want to update indices if we've notified
	//about better path. The calling code will show better path for the
	//same indices on the next call.
	if (!isNotifiedAboutBetterPathFound)
	{
		updateIndices();
		//if indices are same, we can safely update them again (we don't need
		//path from some vertice to itself)
		if (i == j)
			updateIndices();
	}

	//if distance in the new iteration is less than that in old
	//it means we've either found a new or a better path
	if (this->distancesMatrixAfterIteration[i][j] < this->distancesMatrixBeforeIteration[i][j])
	{
		//if new path is found
		if (this->distancesMatrixBeforeIteration[i][j] == infinity)
		{
			return FloydStepResult::PathFoundAndApplied;
		}
		//if better path is found
		else
		{
			//we'll notify about new path so calling code will be able to draw bad path
			//before drawing new
			if (!isNotifiedAboutBetterPathFound)
			{
				isNotifiedAboutBetterPathFound = true;
				return FloydStepResult::BetterPathFound;
			}
			else
			{
				isNotifiedAboutBetterPathFound = false;
				return FloydStepResult::BetterPathApplied;
			}
		}
	}
	return FloydStepResult::NoPathFound; //or the path just staying same as before. But we don't want to show same paths every time, do we
}

void Graph::oneIteration()
{
	//increment iteration number
	k++;

	//if it equals vertices count then we're done
	if (k == verticesCount)
		return;
	//at first we need to copy distances and predecessors from new to old values arrays
	for (int i = 0; i < verticesCount; i++) {
		for (int j = 0; j < verticesCount; j++) {
			distancesMatrixBeforeIteration[i][j] = distancesMatrixAfterIteration[i][j];
			predecessorsMatrixBeforeIteration[i][j] = predecessorsMatrixAfterIteration[i][j];
		}
	}
	//here goes the actual Floyd algorithm (only one iteration)
	for (int i = 0; i < verticesCount; i++) {
		for (int j = 0; j < verticesCount; j++) {
			if (this->distancesMatrixAfterIteration[i][k] == infinity
				|| this->distancesMatrixAfterIteration[k][j] == infinity)
			{
				continue;
			}
			if (this->distancesMatrixAfterIteration[i][k] + this->distancesMatrixAfterIteration[k][j] < this->distancesMatrixAfterIteration[i][j])
			{
				this->distancesMatrixAfterIteration[i][j] = this->distancesMatrixAfterIteration[i][k] + this->distancesMatrixAfterIteration[k][j];
				this->predecessorsMatrixAfterIteration[i][j] = this->predecessorsMatrixAfterIteration[k][j];
			}
		}
	}
}

void Graph::updateIndices()
{
	//we'll decrement j because we need shorter paths shown first
	//(that's more logical I think)
	j--;
	//if j goes less than 0 we're gonna reset it and incremet i
	if (j == -1) {
		j = verticesCount-1;
		i++;
	}
	//if i goes to it's max then we'll perform one iteration of Floyd algrorithm.
	if (i == verticesCount) {
		i = 0;
		oneIteration();
	}
}

//see description in .h
void Graph::getPath(int start, int finish, std::vector<int> &path, bool old)
{
	//choosing between old and new predecessors matrix
	int** predecessors = old ? predecessorsMatrixBeforeIteration : predecessorsMatrixAfterIteration;
	//if it's the same node we'll put it to vector and that's it
	if (start == finish)
	{
		path.push_back(start);
	}
	//if there's no path known we'll clear the vector and return
	else if (predecessors[start][finish] == -1)
	{
		path.clear();
		return;
	}
	//if there is predecessor we'll call this function again but with predecessor of 'finish' vertice as a finish.
	else
	{
		getPath(start, predecessors[start][finish], path);
		path.push_back(finish);
	}
}
