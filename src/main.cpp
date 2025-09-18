
#include "KHR/khrplatform.h"
#include "glm/exponential.hpp"
#include "glm/ext/vector_float2.hpp"
#include "mdcpp/input/keycodes.h"
#include <mdcpp/window.hpp>
#include <mdcpp/drawables/line.hpp>
#include <mdcpp/drawables/circle.h>
#include <string>
#include <vector>
#include <mdcpp/input/input.h>
#include <iostream>


std::vector<glm::vec2>generateROU(int n, float radius)
{
	std::vector<glm::vec2> ROU;

	for(int k = 0; k < n; k++)
	{
		float theta = 2 * M_PI * k / n;
		ROU.push_back(glm::vec2(radius * cos(theta), radius * sin(theta)));
	}

	return ROU;
}

std::vector<mdcpp::Circle> getROUs(int n, float radius, std::vector<glm::vec2> ROU)
{
	std::vector<mdcpp::Circle> ROUs;
	for (int k = 0; k < n; k++)
	{
		ROUs.push_back(mdcpp::Circle(5.f, ROU[k], 2.f, glm::vec3(1.f), true));
	}

	return ROUs;

}

std::vector<glm::vec3> colors = {
	// red
	glm::vec3(1.0f, 0.0f, 0.0f),
	// green
	glm::vec3(0.0f, 1.0f, 0.0f),
	// blue
	glm::vec3(0.0f, 0.0f, 1.0f),
	// yellow
	glm::vec3(1.0f, 1.0f, 0.0f),
	// magenta
	glm::vec3(1.0f, 0.0f, 1.0f),
	// cyan
	glm::vec3(0.0f, 1.0f, 1.0f),
	// orange
	glm::vec3(1.0f, 0.5f, 0.0f),
	// purple
	glm::vec3(0.5f, 0.0f, 1.0f),
};


glm::vec2 polarToCart(glm::vec2 polar)
{
	return glm::vec2(polar.x * cos(polar.y), polar.x * sin(polar.y));
}

class snail {
public:
	snail() {}
	snail(glm::vec2 startPos, int n, int N, float u, float radius, glm::vec3 colour) {
		this->startPos = startPos;

		this->u = u;
		float alpha = 2 * M_PI / N;
		this->r_hat = glm::vec2(cos(alpha), sin(alpha));
		this->theta_hat = glm::vec2(-sin(alpha), cos(alpha));

		this->beta = 0.5f * (M_PI - alpha);

		this->theta_0 = (n-1) * 2 * M_PI / N;
		this->radius = radius;

		this->position = glm::vec2(radius, theta_0);

		this->colour = colour;
	}

	float r(float t){
		return radius - ((u * t) * cos(beta));
	}

	float theta(float t){
		return theta_0 + (glm::log(radius / r(t)) * tan(beta));
	}

	void update(float dt){
		t_ += dt;
		this->position = glm::vec2(r(t_), theta(t_));
	}
	void updateFixed(float t){
		prev_t_ = t_;
		t_ = t;
		this->position = glm::vec2(r(t), theta(t));
	}
	
	glm::vec2 getPosition(float t)
	{
		return polarToCart(glm::vec2(r(t), theta(t)));
	}
	glm::vec2 getPosition(){
		return polarToCart(position);
	}

public:
	glm::vec2 startPos;
	glm::vec2 r_hat;
	glm::vec2 theta_hat;
	glm::vec2 position; // polar

	float u;
	float beta;
	float theta_0;
	float radius;
	float t_;
	float prev_t_;

	glm::vec3 colour;

};

class snailManager {
public:
	snailManager() {}
	snailManager(int N, float u, float radius){
		this->N = N;
		this->u = u;
		this->radius = radius;
		std::vector<glm::vec2> ROU = generateROU(N, radius);
		for (int k = 1; k <= N; k++)
		{
			snails.push_back(snail(ROU[k], k, N, u, radius, colors[k-1]));
		}
	}

	void update(float dt){
		for (int k = 0; k < N; k++)
		{
			snails[k].update(dt);
		}
	}

	void updateFixedTime(float t)
	{
		for (int k = 0; k < N; k++)
		{
			snails[k].updateFixed(t);
		}
	}

	void draw(mdcpp::Window& window){
		for (int k = 0; k < N; k++)
		{
			mdcpp::Circle circle(3.f, snails[k].getPosition(), 2.f, snails[k].colour, true);
			window.draw(std::make_shared<mdcpp::Circle>(circle));
	    }
		
	}

	void drawLines(mdcpp::Window& window){
		for (int k = 0; k < N; k++)
		{
			mdcpp::Line line(snails[k].getPosition(), snails[(k + 1) % N].getPosition(), 2.f, glm::vec3(0.f));
			window.draw(std::make_shared<mdcpp::Line>(line));

		}
	}

	void drawLinesBetween(mdcpp::Window& window){
		for (int k = 0; k < N; k++)
		{
			mdcpp::Line line(snails[k].getPosition(snails[k].prev_t_), snails[k].getPosition(snails[k].t_), 2.f, snails[k].colour);
			window.draw(std::make_shared<mdcpp::Line>(line));
		}
	}

	float getTotalTime()
	{
		return radius / (u * cos(snails[0].beta));
	}

public:
	std::vector<snail> snails;
	int N;
	float u;
	float radius;
};


int main()
{

	float radius = 250.f;
	float u = 5.f;
	float N = 5;

	std::string input;

	std::cout << "Enter the n-gon: (" << N << ")" << std::endl;
	std::getline(std::cin, input);
	if(!input.empty()) { N = std::stoi(input); }

	std::cout << "Enter the speed of the snails: (" << u << ")" << std::endl;
	std::getline(std::cin, input);
	if(!input.empty()) { u = std::stof(input); }

	std::cout << "Enter the radius: (" << radius << ")" << std::endl;
	std::getline(std::cin, input);
	if(!input.empty()) { radius = std::stof(input); }

	snailManager manager(N, u, radius);
	float totalTime = manager.getTotalTime();
	std::cout << "Total time: " << totalTime << std::endl;


	mdcpp::Window window(600, 600, "SnailsofPursuit");
	window.setClearColour(glm::vec3(1.f));



	while(!window.shouldClose())
	{
		int i = 0;
		while (float(i) * 2.f < totalTime)
		{
			i++;
			manager.drawLines(window);
			manager.draw(window);
			manager.updateFixedTime(float(i) * 2.f);
			manager.drawLinesBetween(window);
		}

		window.render();
	}

	return 0;
}
