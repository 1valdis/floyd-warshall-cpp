#define _USE_MATH_DEFINES
#include "GraphVisualizer.h"
#include "Graph.h"
#include "LineShape.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <limits.h>
#include <windows.h>
#include <SFML/Graphics.hpp>

#define infinity INT_MAX

//these defines should talk for themselves
#define VERTICE_RADIUS 20 //radius of vertice in pixels (defines font size in it as well)
#define VERTICE_BORDER_WIDTH 3.f
#define EDGE_THICKNESS 3.f
#define EDGE_WEIGHT_FONT_SIZE 20.f
//in milliseconds
#define NEW_PATH_SHOW_TIME 1000
#define BAD_PATH_SHOW_TIME 2000
#define GOOD_PATH_SHOW_TIME 2000
#define NO_PATH_SHOW_TIME 200
//margin of indices from top and left edges of the window
#define INDICES_MARGIN 30.f
//margin of back button from bottom and left edges of the window
#define BACK_BUTTON_MARGIN 30.f

using namespace sf;
using namespace std;

GraphVisualizer::GraphVisualizer(Graph* graph, RenderWindow* window, int graphRadius)
{
	//so we'll set our graph, window and graph radius
	this->window = window;
	this->graph = graph;
	this->graphRadius = graphRadius;
	//now we'll try to load our Arial font file which should be with .exe
	Font* font = new Font();
	if (font->loadFromFile("arial.ttf") == false) //if font can't be loaded
	{
		delete font;
		font = nullptr;
	}
	this->font = font;
	this->clock = Clock();
	this->beforeUpdate = Time();
	this->lastResult = Graph::FloydStepResult::NoPathFound;

	//to be able to detect if mouse is hovering above the border, we need to store it as a member of class
	buttonText.setFillColor(Color::Black);
	buttonText.setFont(*font);
	buttonText.setString("<< Back");

	//calculating dimensions of rectangle around button's text
	Vector2u targetDimensions = window->getSize();
	FloatRect textBounds = buttonText.getGlobalBounds();
	buttonText.setPosition(textBounds.left + BACK_BUTTON_MARGIN, targetDimensions.y - textBounds.height - textBounds.top * 2 - BACK_BUTTON_MARGIN);

	buttonBorder.setPosition(buttonText.getPosition());
	buttonBorder.setSize(Vector2f(textBounds.width + textBounds.left * 2, textBounds.height + textBounds.top * 2));
	buttonBorder.setOutlineColor(Color::Black);
	buttonBorder.setOutlineThickness(2);
}

GraphVisualizer::~GraphVisualizer()
{
	//all we need to clean up is font... if it's loaded of course
	if (this->font != nullptr)
		delete this->font;
}

void GraphVisualizer::drawGraph()
{
	//drawing all edges and vertices of a graph
	drawEdges();
	drawVertices();
	//check the time elapsed since last frame
	Time time = clock.getElapsedTime();
	//if it's time to draw next step, let's do it!
	if (time > beforeUpdate)
	{
		lastResult = graph->floydStep();
		clock.restart();
		updateTime();
	}
	//it will hold the path to draw
	vector<int> path;
	//we'll draw different things depending on what was the result of floydStep
	switch (lastResult)
	{
	case Graph::FloydStepResult::BetterPathApplied:
		//if it was better path, then we'll draw it with green color
		graph->getPath(graph->i, graph->j, path);
		drawEdges(path, Color::Green);
		drawVertices(path, Color::Green);
		break;
	case Graph::FloydStepResult::BetterPathFound:
		//if it was bad path, we'll draw it in red, getting it from old predecessor matrix
		graph->getPath(graph->i, graph->j, path, true);
		drawEdges(path, Color::Red);
		drawVertices(path, Color::Red);
		break;
	case Graph::FloydStepResult::PathFoundAndApplied:
		//if it was new path, we'll draw it with cyan
		graph->getPath(graph->i, graph->j, path);
		drawEdges(path, Color::Cyan);
		drawVertices(path, Color::Cyan);
		break;
	case Graph::FloydStepResult::NoPathFound:
		//if nothing changed we'll just highlight corresponding vertices
		drawVertice(graph->i, Color::Yellow);
		drawVertice(graph->j, Color::Yellow);
		break;
	case Graph::FloydStepResult::EndOfAlgorithm:
		//if we're done, we won't draw something special
		break;
	}
	//now we'll draw indices and back button
	drawIndices();
	drawBackButton();
}

void GraphVisualizer::updateTime()
{
	//update time before next floydStep call dependently of what was returned from current
	switch (lastResult)
	{
	case Graph::FloydStepResult::BetterPathApplied:
		beforeUpdate = milliseconds(GOOD_PATH_SHOW_TIME);
		break;
	case Graph::FloydStepResult::BetterPathFound:
		beforeUpdate = milliseconds(BAD_PATH_SHOW_TIME);
		break;
	case Graph::FloydStepResult::NoPathFound:
		beforeUpdate = milliseconds(NO_PATH_SHOW_TIME);
		break;
	case Graph::FloydStepResult::PathFoundAndApplied:
		beforeUpdate = milliseconds(NEW_PATH_SHOW_TIME);
		break;
	case Graph::FloydStepResult::EndOfAlgorithm:
		beforeUpdate = seconds(0);
		break;
	}
}

void GraphVisualizer::drawVertices()
{
	//drawing every vertice of graph with black color
	for (int i = 0; i < this->graph->verticesCount; i++)
	{
		drawVertice(i, Color::Black);
	}
}

void GraphVisualizer::drawVertices(std::vector<int> path, Color color)
{
	//drawing every vertice of path with given color
	for (unsigned int i = 0; i < path.size(); i++)
	{
		drawVertice(path[i], color);
	}
}

void GraphVisualizer::drawEdges()
{
	//for every cell in adjacency matrix
	for (int i = 0; i < this->graph->verticesCount; i++)
	{
		for (int j = 0; j < this->graph->verticesCount; j++)
		{
			//if it's not infinity then there's edge with given weight, let's draw it in black
			if (this->graph->adjacencyMatrix[i][j] != infinity && i != j)
			{
				drawEdge(i, j, this->graph->adjacencyMatrix[i][j], Color::Black);
			}
		}
	}
}

void GraphVisualizer::drawEdges(std::vector<int> path, sf::Color color)
{
	//draw every edge connecting i and i+1 vertices of path between vertices
	for (unsigned int i = 0; i < path.size() - 1; i++)
	{
		drawEdge(path[i], path[i + 1], this->graph->adjacencyMatrix[path[i]][path[i + 1]], color);
	}
}

void GraphVisualizer::drawIndices()
{
	//if font isn't loaded we have nothing to do
	if (this->font == nullptr)
		return;

	//k index
	Text k;
	k.setFillColor(Color::Black);
	k.setStyle(Text::Bold);
	k.setFont(*font);
	k.setString("k = " + to_string(this->graph->k));
	k.setPosition(INDICES_MARGIN, INDICES_MARGIN);
	this->window->draw(k);

	//getting k coordinates so we'll draw i index relatively to them
	//because we can't draw multi-line text easily
	FloatRect kBoundingRect = k.getGlobalBounds();

	Text i;
	i.setFillColor(Color::Black);
	i.setStyle(Text::Bold);
	i.setFont(*font);
	i.setString("i = " + to_string(this->graph->i));
	//setting position relatively to k
	i.setPosition(kBoundingRect.left, kBoundingRect.height + kBoundingRect.top);
	this->window->draw(i);

	//same for j
	FloatRect iBoundingRect = i.getGlobalBounds();

	Text j;
	j.setFillColor(Color::Black);
	j.setStyle(Text::Bold);
	j.setFont(*font);
	j.setString("j = " + to_string(this->graph->j));
	j.setPosition(iBoundingRect.left, iBoundingRect.height + iBoundingRect.top);
	this->window->draw(j);
}

void GraphVisualizer::drawBackButton()
{
	//getting mouse position
	Vector2i mousePosition = Mouse::getPosition(*window);
	//if it's inside the button then we'll draw it with grey fill
	if (areCoordsInBackButton(mousePosition.x, mousePosition.y))
		buttonBorder.setFillColor(Color(100, 100, 100));
	else
		buttonBorder.setFillColor(Color::White);
	this->window->draw(buttonBorder);
	this->window->draw(buttonText);
}

Vector2f GraphVisualizer::getVerticeCoords(int verticeIndex)
{
	//complicated math is going here ╰( ͡° ͜ʖ ͡° )つ──☆*:・ﾟ
	//in short we're getting the angle of the vertice on the circle
	//then we're calculating coordinates using sine, cosine and graph circle radius
	//then we're moving it at the center of the window because 0 0 coordinates are top left corner
	double angle = (360 / this->graph->verticesCount*verticeIndex)*M_PI / 180;
	return Vector2f((float)(sin(angle)*this->graphRadius + (this->window->getSize().x / 2)), (float)(-cos(angle)*this->graphRadius + (this->window->getSize().y / 2)));
}

void GraphVisualizer::drawVertice(int index, Color color)
{
	//getting its coordinates
	Vector2f coords = getVerticeCoords(index);

	//creating a circle of given radius, setting origin to it's center and moving to coords above
	sf::CircleShape shape(VERTICE_RADIUS);
	shape.setPosition(coords);
	shape.setOrigin(VERTICE_RADIUS, VERTICE_RADIUS);
	shape.setPointCount(VERTICE_RADIUS < 10 ? 10 : VERTICE_RADIUS);
	shape.setOutlineColor(color);
	shape.setOutlineThickness(-VERTICE_BORDER_WIDTH);

	this->window->draw(shape);

	//we have nothing to write if there's no font
	if (font == nullptr)
		return;

	sf::Text text;
	text.setFont(*font);
	//setting text to vertice index
	text.setString(std::to_string(index));
	text.setCharacterSize((unsigned int)VERTICE_RADIUS);
	text.setFillColor(color);
	//getting its size to center it in the circle created above
	sf::FloatRect textRect = text.getLocalBounds();
	//setting its origin to its center
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	//moving to coords
	text.setPosition(coords);

	this->window->draw(text);
}

void GraphVisualizer::drawEdge(int fromIndex, int toIndex, int weight, Color color)
{
	//getting coords of each vertice and calling this function's overloading
	Vector2f fromCoords = getVerticeCoords(fromIndex);
	Vector2f toCoords = getVerticeCoords(toIndex);
	drawEdge(fromCoords, toCoords, weight, color);
}

void GraphVisualizer::drawEdge(Vector2f& fromCoords, Vector2f& toCoords, int weight, Color color)
{
	//we're gotta make some space between edges between same vertices, which are pointing in
	//opposite directions
	//so we need to move them by their normal vectors of defined length
	Vector2f spacingBetweenEdges = getNormalVectorFromPoints(fromCoords, toCoords, VERTICE_RADIUS / 3);
	Vector2f vectorBetweenPoints = toCoords - fromCoords;
	//and also we need to move arrow tips outside vertices
	Vector2f unitVector = vectorBetweenPoints / (float)sqrt(vectorBetweenPoints.x*vectorBetweenPoints.x + vectorBetweenPoints.y*vectorBetweenPoints.y);
	Vector2f outOfVertice = -unitVector * (float)VERTICE_RADIUS;
	//arrow's body
	LineShape arrowBody(fromCoords, toCoords);
	arrowBody.setThickness(EDGE_THICKNESS);
	arrowBody.setFillColor(color);
	arrowBody.move(spacingBetweenEdges);

	Vector2f direction = (fromCoords - toCoords) / arrowBody.getLength()*20.f; //20 is arrow tip length in pixels

	//creating arrow tip - first line then second
	LineShape arrowTip1(Vector2f(0, 0), direction);
	arrowTip1.setThickness(EDGE_THICKNESS);
	arrowTip1.setFillColor(color);
	arrowTip1.rotate(30);
	arrowTip1.move(toCoords + spacingBetweenEdges + outOfVertice);
	LineShape arrowTip2(Vector2f(0, 0), direction);
	arrowTip2.setThickness(EDGE_THICKNESS);
	arrowTip2.setFillColor(color);
	arrowTip2.rotate(-30);
	arrowTip2.move(toCoords + spacingBetweenEdges + outOfVertice);

	this->window->draw(arrowBody);
	this->window->draw(arrowTip1);
	this->window->draw(arrowTip2);

	//nothing to do more if font is not loaded
	if (font == nullptr)
		return;

	//let's draw edge weight
	sf::Text text;
	text.setFont(*font);
	text.setString(std::to_string(weight));
	text.setCharacterSize((unsigned int)EDGE_WEIGHT_FONT_SIZE);
	text.setFillColor(color);
	//again, centering text
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	//and moving to one third of the length of enge from it's start
	text.move(fromCoords + (toCoords - fromCoords) / 3.f);
	text.move(getNormalVectorFromPoints(fromCoords, toCoords, EDGE_WEIGHT_FONT_SIZE));
	this->window->draw(text);
}

//see .h
Vector2f GraphVisualizer::getNormalVectorFromPoints(Vector2f first, Vector2f second, float normalLength)
{
	Vector2f vector = second - first;
	float coeff = sqrt(vector.x*vector.x + vector.y*vector.y) / normalLength;
	vector /= coeff;
	//rotating it 90 degrees using clever coordinates swap
	float swap = -vector.x;
	vector.x = vector.y;
	vector.y = swap;
	return vector;
}

bool GraphVisualizer::areCoordsInBackButton(int x, int y)
{
	FloatRect borderPosition = this->buttonBorder.getGlobalBounds();
	return (x > borderPosition.left && x<borderPosition.left + borderPosition.width && y>borderPosition.top && y < borderPosition.top + borderPosition.height);
}