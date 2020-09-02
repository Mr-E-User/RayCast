#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <cstdlib>

const int width = 1450, height = 840;
const int fps = 60;

const int numBound = 10;

const int numRays = 360 * 20;
const float increment = 360.f / numRays;
const float DEGTORAD = 3.1415926435897932 / 180;

int rayScale = width;
int opacity = 65;
int R = 128;
int G = 0;
int B = 128;

using namespace sf;

struct boundary {
	Vector2f s, e;

	boundary() { s = Vector2f(0, 0); e = Vector2f(0, 0); }

	boundary(Vector2f start, Vector2f end) { s = start; e = end; }
};

struct ray {
	Vector2f p, d, pt;

	ray() { p = Vector2f(0, 0); d = Vector2f(0, 0); pt = Vector2f(0, 0); }

	ray(Vector2f pos, Vector2f dir) { p = pos; d = dir; pt = Vector2f(0, 0); }

	bool cast(boundary b)
	{
		float x1 = b.s.x;
		float y1 = b.s.y;
		float x2 = b.e.x;
		float y2 = b.e.y;

		float x3 = p.x;
		float y3 = p.y;
		float x4 = p.x + d.x;
		float y4 = p.y + d.y;

		float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		if (den == 0)
			return false;

		float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
		float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

		if (t > 0 && t < 1 && u > 0)
		{
			pt = Vector2f(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
			return true;
		}
		else
			return false;
	}
};

struct particle {
	Vector2f p;
	ray r[numRays];

	particle()
	{
		p = Vector2f(20, 20);
		const float increment = 360.f / numRays;
		float a = 0;
		for (int i = 0; i < numRays; i++)
		{
			r[i] = ray(Vector2f(p.x, p.y), Vector2f(cos(DEGTORAD * a), sin(DEGTORAD * a)));
			a += increment;
		}
	}

	void tick()
	{
		for (int i = 0; i < numRays; i++)
		{
			r[i].p = p;
		}
	}
};

float dist(Vector2f a, Vector2f b)
{
	//Vector2f d = b - a;
	//return sqrt(d.x * d.x + d.y * d.y);

	return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

int main()
{
	std::srand(std::time(0));

	sf::RenderWindow w(sf::VideoMode(width, height), "cart");
	w.setFramerateLimit(fps);
	Vector2f position(20, 20);

	particle par;

	boundary bound[numBound + 4];

	for (int i = 0; i < numBound; i++)
	{
		bound[i] = boundary(Vector2f(std::rand() % width, std::rand() % height), Vector2f(std::rand() % width, std::rand() % height));
	}

	bound[numBound] = boundary(Vector2f(0, 0), Vector2f(0, height));
	bound[numBound + 1] = boundary(Vector2f(0, height), Vector2f(width, height));
	bound[numBound + 2] = boundary(Vector2f(width, height), Vector2f(width, 0));
	bound[numBound + 3] = boundary(Vector2f(width, 0), Vector2f(0, 0));

	while (w.isOpen())
	{
		sf::Event event;
		while (w.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				w.close();
		}

		if (Mouse::isButtonPressed(Mouse::Left)) { if (opacity < 255) { opacity += 5; } }
		if (Mouse::isButtonPressed(Mouse::Right)) { if (opacity > 0) { opacity -= 5; } }
		if (Keyboard::isKeyPressed(Keyboard::W)) { if (rayScale < width) {rayScale += 25; } }
		if (Keyboard::isKeyPressed(Keyboard::S)) { if (rayScale > 0) { rayScale -= 25; } }
		if (Keyboard::isKeyPressed(Keyboard::R)) { R += 1; R %= 255; }
		if (Keyboard::isKeyPressed(Keyboard::G)) { G += 1; G %= 255; }
		if (Keyboard::isKeyPressed(Keyboard::B)) { B += 1; B %= 255; }

		position.x = Mouse::getPosition(w).x;
		position.y = Mouse::getPosition(w).y;

		w.clear();

		par.p = position;
		par.tick();

		for (int i = 0; i < numRays; i++)
		{
			VertexArray line(Lines, 2);
			float recorddist = 100000;
			Vector2f record(0, 0);
			for (int j = 0; j < numBound + 4; j++)
			{				
				if (par.r[i].cast(bound[j]))
				{
					Vector2f tmp = par.r[i].pt;
					float tmpdist = dist(tmp, par.r[i].p);
					//std::cout << i << " " << j << " " << tmpdist << " " << recorddist << std::endl;
					if (tmpdist < recorddist)
					{
						//std::cout << "true" << std::endl;
						recorddist = tmpdist;
						record = tmp;
					}
				}	
			}

			if (dist(record, par.r[i].p) < rayScale)
			{
				line[0].position = par.p;
				line[1].position = record;
			}
			else
			{
				line[0].position = par.p;
				line[1].position = par.p + Vector2f(par.r[i].d.x * rayScale, par.r[i].d.y * rayScale);
			}
			/*else
				{
					line[0].position = par.p;
					line[1].position = par.p + Vector2f(par.r[i].d.x * rayScale, par.r[i].d.y * rayScale);
				}*/

			line[0].color = line[1].color = Color(R, G, B, opacity);

			w.draw(line);
		}

		for (int i = 0; i < numBound + 4; i++)
		{
			VertexArray line(Lines, 2);
			line[0].position = bound[i].s;
			line[1].position = bound[i].e;
			line[0].color = line[1].color = Color::White;

			w.draw(line);
		}

		w.display();
	}

	return 0;
}