#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <limits.h>
#include <windows.h>
#include <conio.h>
#include "Graph.h"
#include "GraphVisualizer.h"

#define infinity INT_MAX

using namespace std;

//void *__gxx_personality_v0;
void displayGraph(int** adjacencyMatrix, int verticesCount);
void showDescription();
void showExample();
void showDevelopers();
void loadFromFile(int*** matrixOutput, int& lengthOutput);

int main(int argc, char* argv[])
{
	//here goes matrix itself and count of vertices
	int** matrix=nullptr, verticesCount=0;
	while (true)
	{
		system("cls");
		cout << "Select what do you wanna do:" << endl
			<< "1 - Load adjacency matrix from file" << endl
			<< "2 - Vizualize loaded matrix" << endl
			<< "3 - Show example visualization" << endl
			<< "4 - Show description of Floyd algorithm" << endl
			<< "5 - Show developers" << endl
			<< "0 - Exit" << endl;
		char choice = _getch();
		system("cls");
		switch (choice)
		{
		case '1':
			loadFromFile(&matrix, verticesCount);
			break;
		case '2':
			if (matrix == nullptr) // gotta check if adjacency matrix is loaded, there's nothing to visualize if it isn't
			{
				cout << "You should load matrix first. Load now?" << endl
					<< "1 - Yes" << endl
					<< "0 - Back to main menu";
				switch (_getch())
				{
				case '1':
					system("cls");
					//loading from file
					loadFromFile(&matrix, verticesCount);
				}
			}
			//visualizing anyway if graph is loaded
			if (matrix!=nullptr)
				displayGraph(matrix, verticesCount);
			break;
		case '3':
			showExample(); //showing predefined example from Wikipedia page "Floyd-Warshall algorithm"
			break;
		case '4':
			showDescription(); //algorithm description
			break;
		case '5':
			showDevelopers(); //list of developers (there's one actually)
			break;
		case '0':
			if (matrix != nullptr) //cleanup before shutdown
			{
				for (int i = 0; i < verticesCount; i++)
				{
					delete[] matrix[i];
				}
				delete[] matrix;
			}
			exit(0);
		}
	}
}

void displayGraph(int** adjacencyMatrix, int verticesCount)
{
	// settings for RenderWindow
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // best quality of edges ;)
	//creating RenderWindow of needed size, with title, style and settings
	sf::RenderWindow window(sf::VideoMode(800, 480), "Floyd visualizer", sf::Style::Default, settings);
	window.setFramerateLimit(60); // thus preventing extensive use of resources
	//creating Graph from matrix
	Graph g(adjacencyMatrix, verticesCount);
	//creating this graph's visualizer on RenderWindow made above and with defined vertice circle radius
	GraphVisualizer visualizer(&g, &window, 200); 

	while (window.isOpen())//event loop
	{
		sf::Event event;
		while (window.pollEvent(event)) //checking for events happened between iterations (like mouse click or window closing)
		{
			//we're gonna close the window if user hits X button or Alt+F4
			if (event.type == sf::Event::Closed
				//or if the user presses the back button in our RenderWindow
				|| event.type == sf::Event::MouseButtonPressed && visualizer.areCoordsInBackButton(event.mouseButton.x, event.mouseButton.y))
				window.close();
		}
		//standard drawing loop - clearing screen, drawing again, and displaying what is drawn
		window.clear(sf::Color::White);
		visualizer.drawGraph();
		window.display();
	}
}

void showDescription()
{
	cout << "In computer science, the Floyd–Warshall algorithm is an algorithm for finding shortest paths in a weighted graph with positive or negative edge weights (but with no negative cycles). A single execution of the algorithm will find the lengths (summed weights) of the shortest paths between all pairs of vertices. Although it does not return details of the paths themselves by default, it is possible to reconstruct the paths with simple modifications to the algorithm (which is done in this implementation). Versions of the algorithm can also be used for finding the transitive closure of a relation R, or (in connection with the Schulze voting system) widest paths between all pairs of vertices in a weighted graph." << endl
		<< "Any key - back" << endl;
	_getch();
}

void showDevelopers()
{
	cout << "Vladislav Gooba - https://github.com/1valdis" <<endl
	<< "Any key - back" << endl;
	_getch();
}

void showExample()
{
	//graph from Wikipedia:
	//https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm#Example
	int** adjacency = new int*[4]
	{
		new int[4]{ 0,  infinity, -2, infinity },
		new int[4]{ 4, 0, 3, infinity },
		new int[4]{ infinity, infinity, 0, 2 },
		new int[4]{ infinity, -1, infinity, 0 }
	};

	displayGraph(adjacency, 4);

	//cleanup (we don't need memory leaks, do we)
	for (int i = 0; i < 4; i++) {
		delete[] adjacency[i];
	}
	delete[] adjacency;
}

void loadFromFile(int*** matrixOutput, int& verticesCount) //three star programming ( ͡~ ͜ʖ ͡°)
{
	cout << "The file should be named as \"input.txt\";" << endl
		<< "have ASCII encoding and contain count of vertices at the first line" << endl
		<< "and anjacency matrix at the next lines;" << endl
		<< "Existent edges should be integer values up to INT_MAX/2;" << endl
		<< "Nonexistent edges should be marked as \"inf\" keyword." << endl
		<< "1 - read file" << endl
		<< "2 - open folder" << endl
		<< "0 - back" << endl;
	switch (_getch())
	{
	case '1':
	{
		//stream for file reading
		ifstream input("input.txt");

		//gonna read only if it's open
		if (!input.is_open())
		{
			system("cls");
			cout << "The file couldn't be loaded" << endl
				<< "Any key - back" << endl;
			_getch();
			break;
		}
		//if there already was some matrix - we should clean up its resources
		if (*matrixOutput != nullptr)
		{
			for (int i = 0; i < verticesCount; i++)
			{
				delete[] (*matrixOutput)[i];
			}
			delete[] *matrixOutput;
		}
		//reading vertices count
		input >> verticesCount;
		*matrixOutput = new int*[verticesCount];
		string element;

		//reading elements
		for (int i = 0; i < verticesCount; i++)
		{
			(*matrixOutput)[i] = new int[verticesCount];
			for (int j = 0; j < verticesCount; j++)
			{
				input >> element;
				//if it's "inf" string we should put infinity there
				if (element == "inf")
				{
					(*matrixOutput)[i][j] = infinity;
				}
				//else the weight itself
				else
				{
					(*matrixOutput)[i][j] = stoi(element);
				}
			}
		}
		break;
	}
	case '2':
		//fancy explorer window in current directory ;)
		system("start .");
	case '0':
		break;
	}
}