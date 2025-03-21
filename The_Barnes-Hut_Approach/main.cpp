
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

struct Body {
    double x, y, z, mass;
    double vx, vy, vz;

    Body(double x, double y, double z, double mass, double vx = 0, double vy = 0, double vz = 0)
        : x(x), y(y), z(z), mass(mass), vx(vx), vy(vy), vz(vz) {}
};

struct Quad {
    double x, y, size;

    Quad(double x, double y, double size) : x(x), y(y), size(size) {}

    bool contains(const Body& body) {
        return body.x >= x - size && body.x <= x + size &&
               body.y >= y - size && body.y <= y + size;
    }
};

struct Node {
    Quad boundary;
    Body *body = nullptr;
    Node *NW = nullptr, *NE = nullptr, *SW = nullptr, *SE = nullptr;

    Node(Quad boundary) : boundary(boundary) {}

    bool isExternal() {
        return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr;
    }

    void insert(Body *b) {
        if (!boundary.contains(*b)) return;

        if (body == nullptr) {
            body = b;
            return;
        }

        if (isExternal()) {
            subdivide();
            insertIntoChildren(body);
            body = nullptr;
        }

        insertIntoChildren(b);
    }

    void subdivide() {
        double x = boundary.x;
        double y = boundary.y;
        double size = boundary.size / 2;

        NW = new Node(Quad(x - size, y + size, size));
        NE = new Node(Quad(x + size, y + size, size));
        SW = new Node(Quad(x - size, y - size, size));
        SE = new Node(Quad(x + size, y - size, size));
    }

    void insertIntoChildren(Body *b) {
        if (NW->boundary.contains(*b))
            NW->insert(b);
        else if (NE->boundary.contains(*b))
            NE->insert(b);
        else if (SW->boundary.contains(*b))
            SW->insert(b);
        else if (SE->boundary.contains(*b))
            SE->insert(b);
    }
};

int main() {
    std::ifstream input("src.txt");
    if (!input) {
        std::cerr << "Failed to open src.txt" << std::endl;
        return 1;
    }

    double G;
    int numBodies, timeSteps;
    input >> G >> numBodies >> timeSteps;

    std::vector<Body> bodies;
    for (int i = 0; i < numBodies; i++) {
        double mass, x, y, z, vx, vy, vz;
        input >> mass;
        input >> x >> y >> z;
        input >> vx >> vy >> vz;
        bodies.emplace_back(x, y, z, mass, vx, vy, vz);
    }

    Quad boundary(0.5, 0.5, 0.5);
    Node tree(boundary);

    for (auto &body : bodies) {
        tree.insert(&body);
    }

    std::cout << "Initial State:" << std::endl;
    for (int i = 0; i < bodies.size(); i++) {
        std::cout << "Body " << i + 1 << " : (" 
                  << bodies[i].x << ", " << bodies[i].y << ", " << bodies[i].z 
                  << ") | (" << bodies[i].vx << ", " << bodies[i].vy << ", " << bodies[i].vz << ")" 
                  << std::endl;
    }

    // Example for time steps (simplified)
    for (int t = 0; t < timeSteps; t++) {
        std::cout << "\nCycle " << t + 1 << std::endl;
        for (int i = 0; i < bodies.size(); i++) {
            // Example of updating positions (real simulation requires force calculation)
            bodies[i].x += bodies[i].vx * 0.01;
            bodies[i].y += bodies[i].vy * 0.01;
            bodies[i].z += bodies[i].vz * 0.01;

            std::cout << "Body " << i + 1 << " : (" 
                      << bodies[i].x << ", " << bodies[i].y << ", " << bodies[i].z 
                      << ") | (" << bodies[i].vx << ", " << bodies[i].vy << ", " << bodies[i].vz << ")" 
                      << std::endl;
        }
    }

    return 0;
}
