#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Constants for the game
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_ROWS = 8; // Number of rows of pegs
const int NUM_SLOTS = 9; // Number of slots at the bottom
const float PEG_RADIUS = 10.0f;
const float BALL_RADIUS = 15.0f;

// Function to generate random float between min and max
float getRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

int main() {
    // Initialize random seed
    srand(static_cast<unsigned>(time(0)));

    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Plinko Game");

    // Define the board dimensions
    const float boardWidth = WINDOW_WIDTH * 0.8f;
    const float boardHeight = WINDOW_HEIGHT * 0.7f;
    const float boardX = (WINDOW_WIDTH - boardWidth) / 2;
    const float boardY = 50;

    // Create pegs
    std::vector<std::vector<sf::CircleShape>> pegs(NUM_ROWS);
    for (int row = 0; row < NUM_ROWS; ++row) {
        int numPegsInRow = row + 1;
        for (int col = 0; col < numPegsInRow; ++col) {
            sf::CircleShape peg(PEG_RADIUS);
            float x = boardX + (boardWidth / (numPegsInRow + 1)) * (col + 1);
            float y = boardY + (boardHeight / (NUM_ROWS + 1)) * (row + 1);
            peg.setPosition(x - PEG_RADIUS, y - PEG_RADIUS);
            peg.setFillColor(sf::Color::White);
            pegs[row].push_back(peg);
        }
    }

    // Create slots
    std::vector<sf::RectangleShape> slots(NUM_SLOTS);
    std::vector<int> slotValues = {100, 50, 20, 10, 5, 10, 20, 50, 100}; // Payout values
    float slotWidth = boardWidth / NUM_SLOTS;
    for (int i = 0; i < NUM_SLOTS; ++i) {
        sf::RectangleShape slot(sf::Vector2f(slotWidth, 30));
        slot.setPosition(boardX + i * slotWidth, boardY + boardHeight);
        slot.setFillColor(sf::Color::Green);
        slots[i] = slot;
    }

    // Ball setup
    sf::CircleShape ball(BALL_RADIUS);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition(WINDOW_WIDTH / 2 - BALL_RADIUS, boardY - BALL_RADIUS * 2);

    // Betting system
    int playerMoney = 1000; // Initial money
    int betAmount = 0;
    bool isBetting = true;

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle betting input
            if (isBetting && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && betAmount < playerMoney) {
                    betAmount += 10;
                } else if (event.key.code == sf::Keyboard::Down && betAmount > 0) {
                    betAmount -= 10;
                } else if (event.key.code == sf::Keyboard::Enter && betAmount > 0) {
                    isBetting = false;
                    playerMoney -= betAmount;
                }
            }
        }

        // Update ball position if not betting
        if (!isBetting) {
            float dx = getRandomFloat(-2.0f, 2.0f); // Random horizontal movement
            float dy = 5.0f; // Fixed downward movement
            ball.move(dx, dy);

            // Check collision with pegs
            for (auto& row : pegs) {
                for (auto& peg : row) {
                    if (ball.getGlobalBounds().intersects(peg.getGlobalBounds())) {
                        ball.move(-dx, -dy); // Reverse direction
                        break;
                    }
                }
            }

            // Check if ball lands in a slot
            for (int i = 0; i < NUM_SLOTS; ++i) {
                if (ball.getGlobalBounds().intersects(slots[i].getGlobalBounds())) {
                    playerMoney += betAmount * slotValues[i];
                    ball.setPosition(WINDOW_WIDTH / 2 - BALL_RADIUS, boardY - BALL_RADIUS * 2);
                    isBetting = true;
                    betAmount = 0;
                    break;
                }
            }
        }

        // Draw everything
        window.clear();
        for (auto& row : pegs) {
            for (auto& peg : row) {
                window.draw(peg);
            }
        }
        for (auto& slot : slots) {
            window.draw(slot);
        }
        window.draw(ball);

        // Display betting information
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font!" << std::endl;
            return -1;
        }
        sf::Text infoText;
        infoText.setFont(font);
        infoText.setCharacterSize(20);
        infoText.setFillColor(sf::Color::White);
        if (isBetting) {
            infoText.setString("Press UP/DOWN to adjust bet (" + std::to_string(betAmount) + 
                               "), ENTER to start. Money: " + std::to_string(playerMoney));
        } else {
            infoText.setString("Money: " + std::to_string(playerMoney));
        }
        infoText.setPosition(10, WINDOW_HEIGHT - 40);
        window.draw(infoText);

        window.display();
    }

    return 0;
}
