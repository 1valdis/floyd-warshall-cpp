#pragma once
#include "Graph.h"
#include <SFML/Graphics.hpp>

class GraphVisualizer
{
public:

	GraphVisualizer(Graph* graph, sf::RenderWindow* window, int graphRadius);
	virtual ~GraphVisualizer();
	
	void drawGraph();//draws graph to window
	void drawVertice(int index, sf::Color color); //draws vertice by given index and of given color
	void drawVertices(); //draws all graph vertices
	void drawVertices(std::vector<int> path, sf::Color color); //draws vertices on path of given color
	void drawEdge(int fromIndex, int toIndex, int weight, sf::Color color); //draws edge by start and finish vertice, of given weight and color
	void drawEdge(sf::Vector2f& fromCoords, sf::Vector2f& toCoords, int weight, sf::Color color); //same but for points, not vertice indices
	void drawEdges(); //draws all graph edges
	void drawEdges(std::vector<int> path, sf::Color color); //draws edges on given path of given color
	void drawIndices(); //draws current graph indices
	void drawBackButton(); //draws back button

	bool areCoordsInBackButton(int x, int y); //checks if given x and y are inside of back button

	sf::Vector2f getVerticeCoords(int verticeIndex); //gets coordinates of vertice by its index
	//gets normal vector of given length relatively to straight line between two points
	static sf::Vector2f getNormalVectorFromPoints(sf::Vector2f first, sf::Vector2f second, float normalLength);


protected:

private:
	//window to draw on
	sf::RenderWindow* window;
	//graph to draw
	Graph* graph;
	//radius of vertice circle
	int graphRadius;

	//font of all text
	sf::Font* font;

	//clock for counting show time
	sf::Clock clock;
	//how much should pass before next floydStep call
	sf::Time beforeUpdate;
	//update beforeUpdate time to reflect floydStep call result
	void updateTime();
	//last floydStep call result
	//we'll save what has floydStep returned last time so we can draw graph multiple times
	//while clock is ticking without calling floydStep
	Graph::FloydStepResult lastResult;
	//text of backButton
	sf::Text buttonText;
	//rectangle around text of backButton
	sf::RectangleShape buttonBorder;
};