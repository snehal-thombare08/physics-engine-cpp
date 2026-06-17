#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm>

const int WIDTH = 1200, HEIGHT = 800;
const float GRAVITY = 900.f;
const float RESTITUTION = 0.78f;   // bounciness (energy kept after bounce)
const float FRICTION = 0.995f;     // air drag
const int SUB_STEPS = 4;           // collision resolution iterations per frame

struct Ball {
    float x, y;
    float vx, vy;
    float radius;
    sf::Color color;
    float mass;

    Ball(float x, float y, float r)
        : x(x), y(y), vx(0), vy(0), radius(r), mass(r*r) {
        color = sf::Color(100,180,255);
    }
};

std::vector<Ball> balls;

float randF(float lo, float hi) {
    return lo + (hi-lo)*(rand()/(float)RAND_MAX);
}

sf::Color speedColor(float speed, float maxSpeed) {
    float t = std::min(1.f, speed / maxSpeed);
    uint8_t r = (uint8_t)(60 + t*195);
    uint8_t g = (uint8_t)(150 - t*100);
    uint8_t b = (uint8_t)(255 - t*150);
    return sf::Color(r, g, b);
}

void spawnBall(float x, float y) {
    float r = randF(12.f, 28.f);
    Ball b(x, y, r);
    b.vx = randF(-150.f, 150.f);
    b.vy = randF(-100.f, 50.f);
    balls.push_back(b);
}

void resolveCollision(Ball& a, Ball& b) {
    float dx = b.x - a.x, dy = b.y - a.y;
    float dist = std::sqrt(dx*dx + dy*dy);
    float minDist = a.radius + b.radius;

    if(dist < minDist && dist > 0.0001f) {
        // Push apart (positional correction) - weighted by inverse mass
        float overlap = minDist - dist;
        float nx = dx/dist, ny = dy/dist;
        float totalMass = a.mass + b.mass;
        float pushA = overlap * (b.mass/totalMass);
        float pushB = overlap * (a.mass/totalMass);
        a.x -= nx * pushA; a.y -= ny * pushA;
        b.x += nx * pushB; b.y += ny * pushB;

        // Elastic collision response along the normal
        float relVx = b.vx - a.vx, relVy = b.vy - a.vy;
        float velAlongNormal = relVx*nx + relVy*ny;

        if(velAlongNormal < 0) { // moving toward each other
            float e = RESTITUTION;
            float j = -(1+e) * velAlongNormal / (1.f/a.mass + 1.f/b.mass);
            float jx = j*nx, jy = j*ny;
            a.vx -= jx/a.mass; a.vy -= jy/a.mass;
            b.vx += jx/b.mass; b.vy += jy/b.mass;
        }
    }
}

void update(float dt, bool gravityOn, float gravDirX, float gravDirY) {
    // Integrate motion
    for(auto& b : balls) {
        if(gravityOn) {
            b.vx += GRAVITY * gravDirX * dt;
            b.vy += GRAVITY * gravDirY * dt;
        }
        b.vx *= FRICTION;
        b.vy *= FRICTION;
        b.x += b.vx * dt;
        b.y += b.vy * dt;

        // Wall collisions
        if(b.x - b.radius < 0) { b.x = b.radius; b.vx = -b.vx * RESTITUTION; }
        if(b.x + b.radius > WIDTH) { b.x = WIDTH - b.radius; b.vx = -b.vx * RESTITUTION; }
        if(b.y - b.radius < 0) { b.y = b.radius; b.vy = -b.vy * RESTITUTION; }
        if(b.y + b.radius > HEIGHT) { b.y = HEIGHT - b.radius; b.vy = -b.vy * RESTITUTION; }
    }

    // Ball-ball collisions, multiple sub-steps for stability with many balls
    for(int iter=0; iter<SUB_STEPS; iter++) {
        for(size_t i=0;i<balls.size();i++)
            for(size_t j=i+1;j<balls.size();j++)
                resolveCollision(balls[i], balls[j]);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH,(unsigned)HEIGHT}),
        "2D Physics Engine | LMB: Spawn | RMB Drag: Throw | G: Gravity | Arrows: Gravity Dir | C: Clear");
    window.setFramerateLimit(60);

    srand(42);
    for(int i=0;i<15;i++) spawnBall(randF(100,WIDTH-100), randF(50,300));

    bool gravityOn = true;
    float gravDirX = 0.f, gravDirY = 1.f;

    bool draggingBall = false;
    int dragIndex = -1;
    sf::Vector2f dragStart, dragCurrent;
    std::vector<sf::Vector2f> dragHistory;

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text hud(font);
    hud.setCharacterSize(15);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(1.5f);
    hud.setPosition({10.f, 10.f});

    sf::Clock clock;

    while(window.isOpen()) {
        float dt = std::min(clock.restart().asSeconds(), 0.02f);
        auto mpos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse((float)mpos.x, (float)mpos.y);

        while(auto ev = window.pollEvent()) {
            if(ev->is<sf::Event::Closed>()) window.close();
            if(auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if(k->code == sf::Keyboard::Key::Escape) window.close();
                if(k->code == sf::Keyboard::Key::G) gravityOn = !gravityOn;
                if(k->code == sf::Keyboard::Key::C) balls.clear();
                if(k->code == sf::Keyboard::Key::Up) { gravDirX=0; gravDirY=-1; }
                if(k->code == sf::Keyboard::Key::Down) { gravDirX=0; gravDirY=1; }
                if(k->code == sf::Keyboard::Key::Left) { gravDirX=-1; gravDirY=0; }
                if(k->code == sf::Keyboard::Key::Right) { gravDirX=1; gravDirY=0; }
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if(mb->button == sf::Mouse::Button::Left) {
                    spawnBall(mouse.x, mouse.y);
                }
                if(mb->button == sf::Mouse::Button::Right) {
                    // Find ball under cursor to drag
                    for(int i=(int)balls.size()-1; i>=0; i--) {
                        float dx = balls[i].x - mouse.x, dy = balls[i].y - mouse.y;
                        if(std::sqrt(dx*dx+dy*dy) < balls[i].radius) {
                            draggingBall = true;
                            dragIndex = i;
                            dragHistory.clear();
                            break;
                        }
                    }
                }
            }
            if(auto* mb = ev->getIf<sf::Event::MouseButtonReleased>()) {
                if(mb->button == sf::Mouse::Button::Right && draggingBall && dragIndex >= 0) {
                    // Throw: velocity from recent drag history
                    if(dragHistory.size() >= 2) {
                        sf::Vector2f vel = (dragHistory.back() - dragHistory.front()) * (1.f/dragHistory.size()) * 60.f;
                        balls[dragIndex].vx = vel.x;
                        balls[dragIndex].vy = vel.y;
                    }
                    draggingBall = false;
                    dragIndex = -1;
                }
            }
        }

        if(draggingBall && dragIndex >= 0 && dragIndex < (int)balls.size()) {
            balls[dragIndex].x = mouse.x;
            balls[dragIndex].y = mouse.y;
            balls[dragIndex].vx = 0; balls[dragIndex].vy = 0;
            dragHistory.push_back(mouse);
            if(dragHistory.size() > 6) dragHistory.erase(dragHistory.begin());
        }

        update(dt, gravityOn, gravDirX, gravDirY);

        window.clear(sf::Color(15, 16, 24));

        // Draw balls
        float maxSpeed = 600.f;
        for(auto& b : balls) {
            float speed = std::sqrt(b.vx*b.vx + b.vy*b.vy);
            sf::CircleShape shape(b.radius, 24);
            shape.setOrigin({b.radius, b.radius});
            shape.setPosition({b.x, b.y});
            shape.setFillColor(speedColor(speed, maxSpeed));
            shape.setOutlineColor(sf::Color(255,255,255,80));
            shape.setOutlineThickness(1.5f);
            window.draw(shape);
        }

        // Drag indicator
        if(draggingBall && dragIndex >= 0 && dragIndex < (int)balls.size()) {
            sf::CircleShape ring(balls[dragIndex].radius + 6.f, 24);
            ring.setOrigin({balls[dragIndex].radius+6.f, balls[dragIndex].radius+6.f});
            ring.setPosition({balls[dragIndex].x, balls[dragIndex].y});
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255,220,80,200));
            ring.setOutlineThickness(2.f);
            window.draw(ring);
        }

        if(hasFont) {
            hud.setString(
                "LMB: Spawn Ball | RMB Drag: Grab & Throw | G: Gravity " +
                std::string(gravityOn?"(ON) ":"(OFF)") +
                " | Arrows: Gravity Direction | C: Clear All | Esc: Quit\n"
                "Balls: " + std::to_string(balls.size())
            );
            window.draw(hud);
        }

        window.display();
    }
    return 0;
}
