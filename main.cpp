#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>

const int HEIGHT = 600;
const int WIDTH = 1000;

const int PLAYER_HEIGHT = 80;
const int PLAYER_WIDTH = 10;

const int BALL_SIZE = 20;

int p1pos = HEIGHT / 2;
int p2pos = HEIGHT / 2;

float ballx = WIDTH / 2;
float bally = HEIGHT / 2;

float ballxvel = 0.2;
float ballyvel = 0.1;

std::string current_screen = "menu";
sf::Font montserrat;

class TextInput {
    private:
        sf::RectangleShape rect;
        sf::Text input;
    public:
        TextInput(int width, int height, int x, int y) {
            rect.setSize(sf::Vector2f(width, height));
            rect.setOutlineColor(sf::Color(0xD7D6D6));
            rect.setPosition(x, y);

            input.setFont(montserrat);
            input.setFillColor(sf::Color::Black);
            input.setCharacterSize(height-8);
            input.setPosition(x+5, y);
            input.setString("Hello");
        }

        void draw(sf::RenderWindow& window) {
            window.draw(rect);
            window.draw(input);
        }

        void checkClicked(sf::RenderWindow& window) {
            if (rect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                std::cout << "Hello\n";
                rect.setOutlineThickness(10);
            } else {
                rect.setOutlineThickness(0);
            }
        }
};

TextInput ipbox(300, 50, 50, 50);

void update_menu(sf::RenderWindow& window) {
    window.clear();

    sf::Text title;

    title.setString("Pong");
    title.setFont(montserrat);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(100);
    title.setPosition(WIDTH / 2 - title.getLocalBounds().width / 2, 0);
    window.draw(title);

    ipbox.draw(window);
}

int main() {
    montserrat.loadFromFile("montserrat.ttf");
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pong");

    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                ipbox.checkClicked(window);
            }
        }

        if (current_screen == "menu") {
            update_menu(window);
        }

        window.display();
    }

    return 0;
}