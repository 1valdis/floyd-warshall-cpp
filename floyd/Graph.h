#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Graph
{
public:
	Graph(int** adjacencyMatrix, int verticesCount);
	//original adjacency matrix of graph
	int** adjacencyMatrix;
	//the algorithm will be divided into iterations.
	//an iteration is every increment of 'k'.
	//on the end of every iteration we'll watch for changed paths.
	//to accomplish this we need distances and predecessors arrays
	//between and after the iteration.

	//distances matrix holds minimum distance from i vertice to j vertice
	int** distancesMatrixBeforeIteration;
	int** distancesMatrixAfterIteration;
	//The predecessors matrix can be read as follows: 
	//if we want to reconstruct the (shortest) path between nodes i and j,
	//then we look at the element at corresponding coordinates.
	//If its value is 0, then there is no path between these nodes,
	//otherwise the value of the element denotes predecessor of j on the path from i to j.
	//So we repeat this procedure, while the preceding node is not equal to i.
	int** predecessorsMatrixBeforeIteration;
	int** predecessorsMatrixAfterIteration;

	int verticesCount;

	//indices of algorithm
	int k;
	int i;
	int j;

	enum FloydStepResult //this is what floyd step function will return
	{
		NoPathFound, // - there's infinity in adjacency matrix;
		PathFoundAndApplied, // - there's path found between 2 vertices, which was infinity before;
		BetterPathFound, // - there's path with less weight found between vertices
		BetterPathApplied, // - ...and applied to distances matrix.
		EndOfAlgorithm // completed
	};

	//checking what is changed between floyd algorithm iterations
	//on path between i and j
	FloydStepResult floydStep();

	//constructs path between start and finish vertices from predecessors array.
	//"Old" set to true means the path will be constructed using predecessors array
	//before iteration. It is useful to show a bad path if there's a better path found
	//during the iteration.
	void getPath(int start, int finish, std::vector<int> &path, bool old=false);

	virtual ~Graph();

private:
	//increments indices one step forward
	void updateIndices();

	//we want to show up both bad and good path
	//for the same indices.
	//this flag will tell to not update indices
	//between calling of floydStep function
	bool isNotifiedAboutBetterPathFound;

	//actual Floyd algorithm, one iteration
	void oneIteration();
};