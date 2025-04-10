#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <tuple>
#include <iomanip>

struct Body
{
    double x, y, z, mass;
    double vx, vy, vz;

    Body(double x, double y, double z, double mass, double vx = 0, double vy = 0, double vz = 0)
        : x(x), y(y), z(z), mass(mass), vx(vx), vy(vy), vz(vz) {}

    std::tuple<double, double, double> computeForceFrom(const Body &other, double G) const
    {
        double dx = other.x - x;
        double dy = other.y - y;
        double dz = other.z - z;

        double distSqr = dx * dx + dy * dy + dz * dz + 1e-10; // avoid division by zero
        double dist = std::sqrt(distSqr);
        double force = G * mass * other.mass / distSqr;

        double fx = force * dx / dist;
        double fy = force * dy / dist;
        double fz = force * dz / dist;

        return {fx, fy, fz};
    }

    void updateVelocity(double fx, double fy, double fz, double dt)
    {
        double ax = fx / mass;
        double ay = fy / mass;
        double az = fz / mass;

        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;
    }

    void updatePosition(double dt)
    {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }

    void print(int i) const
    {
        std::cout << "Body " << i + 1 << " : ("
                  << x << ", " << y << ", " << z
                  << ") | (" << vx << ", " << vy << ", " << vz << ")"
                  << std::endl;
    }
};

struct Quad
{
    double x, y, size;

    Quad(double x, double y, double size) : x(x), y(y), size(size) {}

    bool contains(const Body &body)
    {
        return body.x >= x - size && body.x <= x + size &&
               body.y >= y - size && body.y <= y + size;
    }
};

struct Node
{
    Quad boundary;
    Body *body = nullptr;
    Node *NW = nullptr, *NE = nullptr, *SW = nullptr, *SE = nullptr;

    Node(Quad boundary) : boundary(boundary) {}

    bool isExternal()
    {
        return NW == nullptr && NE == nullptr && SW == nullptr && SE == nullptr;
    }

    void insert(Body *b)
    {
        if (!boundary.contains(*b))
            return;

        if (body == nullptr)
        {
            body = b;
            return;
        }

        if (isExternal())
        {
            subdivide();
            insertIntoChildren(body);
            body = nullptr;
        }

        insertIntoChildren(b);
    }

    void subdivide()
    {
        double x = boundary.x;
        double y = boundary.y;
        double size = boundary.size / 2;

        NW = new Node(Quad(x - size, y + size, size));
        NE = new Node(Quad(x + size, y + size, size));
        SW = new Node(Quad(x - size, y - size, size));
        SE = new Node(Quad(x + size, y - size, size));
    }

    void insertIntoChildren(Body *b)
    {
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

int main()
{
    std::ifstream input("src.txt");
    if (!input)
    {
        std::cerr << "Failed to open src.txt" << std::endl;
        return 1;
    }

    std::ofstream fout("output.csv");
    fout << "time,body,x,y,z,vx,vy,vz\n"; // CSV header

    std::cout << std::fixed << std::setprecision(6);

    double G;
    int numBodies, timeSteps;
    input >> G >> numBodies >> timeSteps;

    std::vector<Body> bodies;
    for (int i = 0; i < numBodies; i++)
    {
        double mass, x, y, z, vx, vy, vz;
        input >> mass;
        input >> x >> y >> z;
        input >> vx >> vy >> vz;
        bodies.emplace_back(x, y, z, mass, vx, vy, vz);
    }

    Quad boundary(0.5, 0.5, 0.5);
    Node tree(boundary);

    for (auto &body : bodies)
    {
        tree.insert(&body);
    }

    std::cout << "Initial State:" << std::endl;
    for (int i = 0; i < bodies.size(); i++)
    {
        bodies[i].print(i);
    }

    double dt = 0.01;

    for (int t = 0; t < timeSteps; t++)
    {
        std::cout << "\nCycle " << t + 1 << std::endl;

        std::vector<std::tuple<double, double, double>> forces(bodies.size(), {0.0, 0.0, 0.0});

        for (int i = 0; i < bodies.size(); i++)
        {
            for (int j = 0; j < bodies.size(); j++)
            {
                if (i == j)
                    continue;
                auto [fx, fy, fz] = bodies[i].computeForceFrom(bodies[j], G);
                std::get<0>(forces[i]) += fx;
                std::get<1>(forces[i]) += fy;
                std::get<2>(forces[i]) += fz;
            }
        }

        for (int i = 0; i < bodies.size(); i++)
        {
            bodies[i].updateVelocity(
                std::get<0>(forces[i]),
                std::get<1>(forces[i]),
                std::get<2>(forces[i]),
                dt);
            bodies[i].updatePosition(dt);
            bodies[i].print(i);

            fout << t + 1 << "," << i + 1 << ","
                 << std::fixed << std::setprecision(6)
                 << bodies[i].x << "," << bodies[i].y << "," << bodies[i].z << ","
                 << bodies[i].vx << "," << bodies[i].vy << "," << bodies[i].vz << "\n";
        }
    }

    return 0;
}