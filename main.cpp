#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <ctime>

using namespace std;

//Базовый абстрактный класс
class AbstractGenerator {
protected:
    int width, height;
    vector<vector<bool>> grid;
public:
    AbstractGenerator(int w, int h) : width(w), height(h) {
        grid.resize(height, vector<bool>(width));
    }
    virtual void step() = 0;//Чистая виртуальная функция
    virtual void initialize() = 0;
    virtual ~AbstractGenerator() {}
};

//Генератор пещер
class CaveGenerator : public AbstractGenerator {
    int birthLimit, deathLimit, chanceToStartAlive;

public:
    CaveGenerator(int w, int h, int bLimit, int dLimit, int chance) : AbstractGenerator(w, h), birthLimit(bLimit), deathLimit(dLimit), chanceToStartAlive(chance) {
        srand(time(0));
        initialize();
    }

    //Заполнение поля случайным шумом
    void initialize() override {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                grid[y][x] = (rand() % 100 < chanceToStartAlive);
            }
        }
    }

    //Подсчет соседей
    int countAliveNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                int nx = x + i, ny = y + j;
                if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                    if (grid[ny][nx]) count++;
                } else {
                    count++; //Считаем границы стенами
                }
            }
        }
        return count;
    }

    //Один шаг алгоритма
    void step() override {
        vector<vector<bool>> nextGrid = grid;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int neighbors = countAliveNeighbors(x, y);
                if (grid[y][x]) {
                    if (neighbors < deathLimit) nextGrid[y][x] = false; // Клетка умирает
                } else {
                    if (neighbors > birthLimit) nextGrid[y][x] = true;  // Клетка рождается
                }
            }
        }
        grid = nextGrid;
    }

    //Отрисовка
    void draw(sf::RenderWindow& window, float cellSize) {
        sf::RectangleShape cell(sf::Vector2f(cellSize - 1.0f, cellSize - 1.0f));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x]) {
                    cell.setPosition(x * cellSize, y * cellSize);
                    cell.setFillColor(sf::Color::Black);
                    window.draw(cell);
                }
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    int w, h, bl, dl, cb;
    cout << "Введите ширину и высоту (например, 60 60): "; cin >> w >> h;
    cout << "Предел рождения (рек. 4): "; cin >> bl;
    cout << "Предел смерти (рек. 3): "; cin >> dl;
    cout << "Шанс заполнения % (рек. 45): "; cin >> cb;

    float cellSize = 10.0f;
    sf::RenderWindow window(sf::VideoMode(w * cellSize, h * cellSize), "Пещерки");

    CaveGenerator cave(w, h, bl, dl, cb);

    cout << "\nУправление:\n[Space] - Шаг генерации\n[R] - Пересоздать шум\n";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::   KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) cave.step();
                if (event.key.code == sf::Keyboard::R) cave.initialize();
            }
        }

        window.clear(sf::Color::White);
        cave.draw(window, cellSize);
        window.display();
    }

    return 0;
}
