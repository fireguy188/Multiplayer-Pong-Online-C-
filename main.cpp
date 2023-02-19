#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>

const int HEIGHT = 600;
const int WIDTH = 1000;

const int PLAYER_HEIGHT = 80;
const int PLAYER_WIDTH = 10;
const float PADDLE_SPEED = 1;

const int BALL_SIZE = 20;

double p1pos = HEIGHT / 2;
double p2pos = HEIGHT / 2;

double ballx = WIDTH / 2;
double bally = HEIGHT / 2;

double ballxvel = 0.2;
double ballyvel = 0.1;

sf::Font montserrat;

class TextInput {
    private:
        sf::RectangleShape rect;
        sf::Text input;
        bool active = false;
    public:
        TextInput(float width, float height, float x, float y) {
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

        std::string getInp() {
            return input.getString();
        }
};

class Button {
    private:
        sf::RectangleShape rect;
        sf::Text text;
        void (*callback)(std::string, sf::RenderWindow& window);
    public:
        Button(float width, float height, float x, float y, std::string t, void (*c)(std::string, sf::RenderWindow& window)) {
            callback = c;

            rect.setSize(sf::Vector2f(width, height));
            rect.setOutlineColor(sf::Color(0xD7D6D6));
            rect.setPosition(x, y);

            text.setFont(montserrat);
            text.setFillColor(sf::Color::Black);
            text.setCharacterSize(height - 8);
            text.setString(t);
            text.setPosition(x + width / 2 - text.getGlobalBounds().width / 2, y);
        }

        void checkClicked(sf::RenderWindow& window, std::string inp) {
            if (rect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                rect.setOutlineThickness(10);
                (*callback)(inp, window);
            }
        }

        void checkUnClicked(sf::RenderWindow& window) {
            rect.setOutlineThickness(0);
        }

        void draw(sf::RenderWindow& window) {
            window.draw(rect);
            window.draw(text);
        }
};

std::string current_screen = "menu";
int player;
int p1points = 0;
int p2points = 0;

// Network stuff
sf::TcpSocket client;
sf::TcpListener listener;

// Menu stuff
sf::Text title;
sf::Text ipbox_label;
TextInput* ipbox;
Button* startBtn;

// Game stuff
sf::RectangleShape p1paddle;
sf::RectangleShape p2paddle;
sf::RectangleShape ball;


void start(std::string text, sf::RenderWindow& window) {
    window.clear();

    sf::Text message;
    message.setString("Connecting...");
    message.setFont(montserrat);
    message.setFillColor(sf::Color::White);
    message.setCharacterSize(120);
    message.setPosition(WIDTH / 2 - message.getLocalBounds().width / 2, HEIGHT / 2 - message.getLocalBounds().height / 2);

    window.draw(message);
    window.display();
    if (text == "") {
        // Wants to be host
        if (listener.listen(53000) != sf::Socket::Done) {
            std::cout << "Starting server error\n";
        }
        
        if (listener.accept(client) != sf::Socket::Done) {
            std::cout << "Client joining error\n";
        }

        player = 1;
    }
    else {
        // Wants to be a client
        sf::Socket::Status status = client.connect("127.0.0.1", 53000);
        if (status != sf::Socket::Done) {
            std::cout << "Connection error\n";
        }

        player = 2;
    }

    current_screen = "game";
}

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

    startBtn = new Button(320, 50, WIDTH / 2 - 320 / 2, 400, "Start", start);

    p1paddle.setFillColor(sf::Color::White);
    p1paddle.setSize(sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT));
    p1paddle.setPosition(sf::Vector2f(10, p1pos));

    p2paddle.setFillColor(sf::Color::White);
    p2paddle.setSize(sf::Vector2f(PLAYER_WIDTH, PLAYER_HEIGHT));
    p2paddle.setPosition(sf::Vector2f(WIDTH - 10 - PLAYER_WIDTH, p2pos));

    ball.setFillColor(sf::Color::White);
    ball.setSize(sf::Vector2f(BALL_SIZE, BALL_SIZE));
    ball.setPosition(sf::Vector2f(WIDTH / 2 - BALL_SIZE / 2, HEIGHT / 2 - BALL_SIZE / 2));

    client.setBlocking(false);
}

void updateMenu(sf::RenderWindow& window) {
    window.clear();

    window.draw(title);
    ipbox->draw(window);
    window.draw(ipbox_label);
    startBtn->draw(window);
}

void resetGame() {
    p1pos = HEIGHT / 2;
    p2pos = HEIGHT / 2;

    ballx = WIDTH / 2;
    bally = HEIGHT / 2;

    ballxvel = 0.2;
    ballyvel = 0.1;
}

void updateGame(sf::RenderWindow& window) {
    window.clear();

    ballx += ballxvel;
    bally += ballyvel;

    if (ballx >= WIDTH - 10 - PLAYER_WIDTH - BALL_SIZE && ballx <= WIDTH - 10 && bally <= p2pos + PLAYER_HEIGHT / 2 && bally >= p2pos - PLAYER_HEIGHT / 2) {
        ballxvel *= -1.01;
    }
    if (ballx >= 10 && ballx <= PLAYER_WIDTH + 10 && bally <= p1pos + PLAYER_HEIGHT / 2 && bally >= p1pos - PLAYER_HEIGHT / 2) {
        ballxvel *= -1.01;
    }
    if (bally >= HEIGHT || bally <= 0) {
        ballyvel *= -1;
    }

    if (ballx >= WIDTH) {
        p1points++;
        resetGame();
    } else if (ballx <= 0) {
        p2points++;
        resetGame();
    }

    p1paddle.setPosition(sf::Vector2f(10, p1pos));
    p2paddle.setPosition(sf::Vector2f(WIDTH - 10 - PLAYER_WIDTH, p2pos));
    ball.setPosition(sf::Vector2f(ballx, bally));

    window.draw(p1paddle);
    window.draw(p2paddle);
    window.draw(ball);
}

void sendUpdate() {
    sf::Packet packet;

    if (player == 1) {
        packet << p1pos;
    } else if (player == 2) {
        packet << p2pos;
    }

    client.send(packet);
}

void receiveUpdate() {
    sf::Packet packet;
    if (client.receive(packet) == sf::Socket::NotReady) {
        return;
    }

    if (player == 1) {
        packet >> p2pos;
    } else if (player == 2) {
        packet >> p1pos;
    }
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
                if (current_screen == "menu") {
                    ipbox->checkClicked(window);
                    startBtn->checkClicked(window, ipbox->getInp());
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (current_screen == "menu") {
                    startBtn->checkUnClicked(window);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (current_screen == "menu") {
                    ipbox->enterText(event.key.code);
                }
            }
        }

        if (current_screen == "menu") {
            updateMenu(window);
        } else if (current_screen == "game") {
            receiveUpdate();
            updateGame(window);

            if (window.hasFocus()) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    if (player == 1) {
                        if (p1pos > 0) {
                            p1pos -= PADDLE_SPEED;
                        }
                    }
                    else if (player == 2) {
                        if (p2pos > 0) {
                            p2pos -= PADDLE_SPEED;
                        }
                    }
                    sendUpdate();
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    if (player == 1) {
                        if (p1pos + PLAYER_HEIGHT < HEIGHT) {
                            p1pos += PADDLE_SPEED;
                        }
                    }
                    else if (player == 2) {
                        if (p2pos + PLAYER_HEIGHT < HEIGHT) {
                            p2pos += PADDLE_SPEED;
                        }
                    }
                    sendUpdate();
                }
            }
        }

        window.display();
    }

    return 0;
}