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

sf::Font montserrat;

class TextInput {
    private:
        sf::RectangleShape rect;
        sf::Text input;
        bool active = false;
    public:
        TextInput(int width, int height, int x, int y) {
            rect.setSize(sf::Vector2f(width, height));
            rect.setOutlineColor(sf::Color(0xD7D6D6));
            rect.setPosition(x, y);

            input.setFont(montserrat);
            input.setFillColor(sf::Color::Black);
            input.setCharacterSize(height-8);
            input.setPosition(x+5, y);
        }

        void draw(sf::RenderWindow& window) {
            window.draw(rect);
            window.draw(input);
        }

        void checkClicked(sf::RenderWindow& window) {
            if (rect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                rect.setOutlineThickness(10);
                active = true;
            } else {
                rect.setOutlineThickness(0);
                active = false;
            }
        }

        void enterText(sf::Keyboard::Key k) {
            if (!active) {
                return;
            }

            std::string old_text = input.getString();
            if (k == sf::Keyboard::Delete || k == sf::Keyboard::Backspace) {
                input.setString(old_text.substr(0, old_text.length() - 1));
            }
            else if (k >= sf::Keyboard::Num0 && k <= sf::Keyboard::Num9) {
                char real_num = k + 22;
                input.setString(old_text + real_num);
            } 
            else if (k >= sf::Keyboard::Numpad0 && k <= sf::Keyboard::Numpad9) {
                char real_num = k - 27;
                input.setString(old_text + real_num);
            }
            else if (k == sf::Keyboard::Period) {
                input.setString(old_text + '.');
            }
            
        }
};

sf::Text title;
sf::Text ipbox_label;
TextInput* ipbox;
std::string current_screen = "menu";

void setupItems() {
    montserrat.loadFromFile("montserrat.ttf");

    title.setString("Pong");
    title.setFont(montserrat);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(100);
    title.setPosition(WIDTH / 2 - title.getLocalBounds().width / 2, 0);

    ipbox = new TextInput(320, 50, WIDTH / 2 - 320 / 2, 300);

    ipbox_label.setString("Enter IP");
    ipbox_label.setFont(montserrat);
    ipbox_label.setFillColor(sf::Color::White);
    ipbox_label.setCharacterSize(50);
    ipbox_label.setPosition(WIDTH / 2 - 320 / 2, 225);
}

void updateMenu(sf::RenderWindow& window) {
    window.clear();

    window.draw(title);
    ipbox->draw(window);
    window.draw(ipbox_label);
}

int main() {
    setupItems();

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pong");
    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                ipbox->checkClicked(window);
            }

            if (event.type == sf::Event::KeyPressed) {
                ipbox->enterText(event.key.code);
            }
        }

        if (current_screen == "menu") {
            updateMenu(window);
        }

        window.display();
    }

    return 0;
}