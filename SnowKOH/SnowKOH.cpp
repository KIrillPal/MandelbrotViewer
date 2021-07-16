#pragma once
#include <chrono>
#include <set>
#include <iostream>
#include <vector>
#include "SFML/Graphics.hpp"
#include "Complex.h"
#include <stack>

double SCROLL_OFFSET = 0.9f;

const char* PROGRAM_NAME = "SFML";
uint16_t WINDOW_W = 820, WINDOW_H = 600;
const uint16_t LEFT_W = 265, MAX_PLAYERS = 10, FIELD_W = 500;
const double USS = 2, ProgramFrameTime = 1000 / 240; // 1000ms / 60frames
auto gameFrameTime = std::chrono::steady_clock::now();
int windowMode = 1, length = 200, doubleClickTimer = -1;
double mouseX = 0, mouseY = 0;
uint64_t frameTime = 0;
sf::Font bF, tF;
sf::Sprite result_sprite;
sf::Texture graphTexture;

sf::Uint8* graphPixels;

struct Frame
{
	double x, y;
};

std::stack<std::pair<Frame, Frame> > zooms;

struct color
{
	double r, g, b;
	color(double r = 0, double g = 0, double b = 0) : r(r), g(g), b(b) {}
};

std::vector<color> mapping;

typedef std::pair<double, color> table_row;

int fractalSize = 0;

sf::RenderWindow* window;

sf::FloatRect mindFrame(-6, -6, 12, 12), frame(0, 0, 228, 228);


double double_to_grey(double r) {
	return std::min(std::max(std::round(r), double(0.0)), double(255.0));
}

color double_to_grey(color c) {
	return { double_to_grey(c.r), double_to_grey(c.g), double_to_grey(c.b) };
}

double lerp(double a, double b, double t) {
	return std::min(std::max(t * (b - a) + a, double(0.0)), double(255.0));
}

color lerp(color a, color b, double t) {
	return { lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t) };
}

double invlerp(double value, double min, double max) {
	return (value - min) / (max - min);
}

color lerp(table_row a, table_row b, double index) {
	return lerp(a.second, b.second, invlerp(index, a.first, b.first));
}

color mandelbrot_color(Complex zn, int alpha, int iters) {
	if (alpha < iters && alpha > 0) {

		return mapping[alpha];
	}
	else return color(0, 0, 0);
}

void setpixel(int x, int y, color c)
{
	int k = (y * WINDOW_W + x) * 4;
	graphPixels[k] = round(c.r);
	graphPixels[k + 1] = round(c.g);
	graphPixels[k + 2] = round(c.b);
	graphPixels[k + 3] = 255;
}

double w_width = 2, w_height = 1.41176470588, w_x = -0.6, w_y = -0.4;
int w_depth = 350;

color HSVtoRGB(double H, double S, double V) {
	double s = S / 100;
	double v = V / 100;
	double C = s * v;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = v - C;
	double r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	double R = (r + m) * 255;
	double G = (g + m) * 255;
	double B = (b + m) * 255;
	return color(R, G, B);
}

void drawfield(int left_bound = 0, int right_bound = WINDOW_W)
{
	#pragma omp parallel
	for (int i = left_bound; i < right_bound; ++i)
	{
		#pragma omp for
		for (int j = 0; j < WINDOW_H; ++j)
		{
			Complex c(i * w_width / WINDOW_W - w_width / 2 + w_x, j * w_height / WINDOW_H - w_height / 2 + w_y), z = 0;
			int alpha = 0;
			for (int iter = 0; iter < w_depth; ++iter)
			{
				auto z2 = z;
				z = z * z + c;
				if (abs2(z) >= 4)
				{
					alpha = iter;
					break;
				}
			}
			double nsmooth = alpha - std::log2(std::log2(abs2(z))) + 4.0;
			double smoothcolor = alpha * 1.0 / w_depth;
			color clr = HSVtoRGB(380.0 * nsmooth / w_depth, 100, std::min(100.0, 1000.0 * alpha / w_depth));//mandelbrot_color(z, alpha, w_depth);
			setpixel(i, j, clr);
			/*sf::Vertex point(sf::Vector2f(i, j), sf::Color(clr.r, clr.g, clr.b));
			window->draw(&point, 1, sf::Points);*/
		}
	}
	graphTexture.create(WINDOW_W, WINDOW_H);
	graphTexture.update(graphPixels, WINDOW_W, WINDOW_H, 0, 0);
	result_sprite.setTexture(graphTexture);
	/*sf::RectangleShape drawline;
	drawline.setPosition(left_bound + 50, 0);
	drawline.setSize(sf::Vector2f(2, WINDOW_H));
	drawline.setFillColor(sf::Color::Red);
	window->draw(drawline);*/
}

void prepare_mapping(color a, color b, int iters)
{
	mapping.resize(iters);
	double adding = 10;
	for (int i = 0; i < std::min(iters, 600); ++i)
	{
		mapping[i].r = a.r + (b.r - a.r) * i / iters + adding;
		mapping[i].g = a.g + (b.g - a.g) * i / iters + adding;
		mapping[i].b = a.b + (b.b - a.b) * i / iters + adding;
		adding *= 0.8;
	}
	for (int i = 600; i < iters; ++i)
	{
			mapping[i].r = mapping[i - 1].r;
			mapping[i].g = mapping[i - 1].g;
			mapping[i].b = mapping[i - 1].b;
	}
}

bool isframe = false;

Frame framePos, frameSize, realPos, realPos2;

void setView(Frame pos, Frame width)
{
	w_width = width.x, w_height = width.y;
	w_x = pos.x + width.x / 2, w_y = pos.y + width.y / 2;
}

void drawFrame()
{
	sf::Vertex line[] =
	{
	 sf::Vertex(sf::Vector2f(realPos.x, realPos.y)),
	 sf::Vertex(sf::Vector2f(realPos2.x, realPos.y)),
	 sf::Vertex(sf::Vector2f(realPos2.x, realPos.y)),
	 sf::Vertex(sf::Vector2f(realPos2.x, realPos2.y)),
	 sf::Vertex(sf::Vector2f(realPos2.x, realPos2.y)),
	 sf::Vertex(sf::Vector2f(realPos.x, realPos2.y)),
	 sf::Vertex(sf::Vector2f(realPos.x, realPos2.y)),
	 sf::Vertex(sf::Vector2f(realPos.x, realPos.y))
	};

	window->draw(line, 8, sf::Lines);
}

Frame tomind(Frame f)
{
	return { f.x * w_width / WINDOW_W - w_width / 2 + w_x, f.y * w_height / WINDOW_H - w_height / 2 + w_y };
}

int main()
{
	std::cout << "TIP: press ENTER to increase precision, BACKSPACE to decrease.\n";
    window = new sf::RenderWindow();
    window->create(sf::VideoMode(WINDOW_W, WINDOW_H), PROGRAM_NAME);
    window->setFramerateLimit(60);
    //bF.loadFromFile("fonts/Nunito-Regular.ttf");

	graphPixels = new sf::Uint8[WINDOW_W * WINDOW_H * 4];
	zooms.push({ { 2, 1.41176470588 }, { -0.6, -0.4 } });

	prepare_mapping(color(27, 2, 0), color(255, 205, 0), w_depth);
	int lb = 0, lb_step = 50;
    while (window->isOpen())
    {
        sf::Event e;
        while (window->pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window->close();
				delete graphPixels;
                delete window;
                return 0;
            }
            int X = e.mouseButton.x, Y = e.mouseButton.y;
            if (windowMode == 1)
            {
                if (e.type == sf::Event::MouseMoved) {
					if (isframe)
					{
						realPos2 = { (double)e.mouseMove.x, realPos.y + ((double)e.mouseMove.x - realPos.x) * WINDOW_H / WINDOW_W };
						window->draw(result_sprite);
						drawFrame();
						window->display();
					}
                }
                else if (e.type == sf::Event::MouseWheelScrolled) {
                }
                else if (e.type == sf::Event::MouseButtonPressed) {
					if (e.mouseButton.button == sf::Mouse::Left)
					{
						isframe = true;
						realPos = { (double)e.mouseButton.x, (double)e.mouseButton.y };
					}
                }
                else if (e.type == sf::Event::MouseButtonReleased) {
					
					/*if (e.mouseButton.button == sf::Mouse::Left)
						w_width /= 2, w_height /= 2;
					else w_width *= 2,w_height *= 2;
					if (w_depth < 256) w_depth = 256;*/
					if (e.mouseButton.button == sf::Mouse::Left && isframe)
					{
						framePos = tomind({ (double)realPos.x, (double)realPos.y });
						Frame frameEnd = tomind({ (double)e.mouseButton.x, (double)e.mouseButton.y });
						frameSize.x = frameEnd.x - framePos.x;
						frameSize.y = frameSize.x * WINDOW_H / WINDOW_W;
						if (frameSize.x > 0 && frameSize.y > 0)
						{
							setView(framePos, frameSize);
							zooms.push({ { w_width, w_height }, { w_x, w_y } });
						}
						lb = 0; 
						isframe = false;
					}
					else if (e.mouseButton.button == sf::Mouse::Right && !isframe)
					{
						if (zooms.size() > 1)
							zooms.pop();
						auto p = zooms.top();
						w_width = p.first.x;
						w_height = p.first.y;
						w_x = p.second.x;
						w_y = p.second.y;
						lb = 0;
					}
                }
                else if (e.type == sf::Event::KeyPressed) {
					if (e.mouseButton.button == sf::Keyboard::Enter)
						w_depth *= 2;
					else if (e.mouseButton.button == sf::Keyboard::BackSpace)
						w_depth /= 2;
					if (w_depth < 3) w_depth = 3;
					else if (w_depth > 100000) w_depth = 100000;
					else if (e.mouseButton.button == sf::Keyboard::Num1)
					{
						framePos.x = -3.843793584379;
						framePos.y = -2;
						frameSize.x = 2 * 3.843793584379;
						frameSize.y = frameSize.x * WINDOW_H / WINDOW_W;
						setView(framePos, frameSize);
					}
					else if (e.mouseButton.button == sf::Keyboard::Num2)
					{
						framePos.x = -0.346465782054;
						framePos.y = -0.631480942820;
						frameSize.x = -0.345217787473 - framePos.x;
						frameSize.y = frameSize.x * WINDOW_H / WINDOW_W;
						setView(framePos, frameSize);
					}
					//prepare_mapping(color(27, 2, 0), color(255, 255, 0), w_depth);
					printf("set precision: %d\n", w_depth);
					lb = 0;
                }
            }
        }
        --doubleClickTimer;
		if (lb != -1)
		{
		//clock_t start = clock();
			drawfield(lb, std::min(lb + lb_step, (int)WINDOW_W));
			lb += lb_step;
			if (lb >= WINDOW_W) lb = -1;
			if (w_depth >= 700 || lb == -1)
			{
				window->draw(result_sprite);
				window->display();
			}
		/*clock_t end = clock();
		double seconds = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Currect time: %lf\n", seconds);*/
		}
    }
}