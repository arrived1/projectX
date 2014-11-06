#pragma once

#include <cmath>
#include <vector>

struct ColorFactory
{
    float4 purple;
    float4 red;
    float4 yellow;
    float4 green;
    float4 blue;
    float4 cyan;
    float4 white;
    float4 orange;
    std::vector<float4> colorFactory;
    
    ColorFactory()
    {
        srand(time(NULL));

        purple = make_float4(1.f, 0.f, 1.f, 0.f);
        red = make_float4(1.f, 0.f, 0.f, 0.f);
        yellow = make_float4(1.f, 1.f, 0.f, 0.f);
        green = make_float4(0.f, 1.f, 0.f, 0.f);
        blue = make_float4(0.f, 0.f, 1.f, 0.f);
        cyan = make_float4(0.f, 1.f, 1.f, 0.f);
        white = make_float4(1.f, 1.f, 1.f, 0.f);
        orange = make_float4(1.f, 0.5f, 0.f, 0.f);

        colorFactory.push_back(purple);
        colorFactory.push_back(red);
        colorFactory.push_back(yellow);
        colorFactory.push_back(green);
        colorFactory.push_back(blue);
        colorFactory.push_back(cyan);
        colorFactory.push_back(white);
        colorFactory.push_back(orange);
    }

    float4 randomColor()
    {
        const unsigned rnd = rand() % colorFactory.size() + 0;
        return colorFactory[rnd];
    } 
};


template <typename W>
class ParticleSystemPhountain
{
    const unsigned size;
    const int box;
    std::vector<float3> r;
    std::vector<float3> next_r;
    std::vector<float3> prev_r;
    std::vector<float3> v;
    std::vector<float3> next_v;
    float mass;
    float radius;
    std::vector<float4> color;
    W& wing;
    float3 phontain;
    ColorFactory colorFactory;

    float lenght(const float3& a)
    {
        return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    }

    float3 normalize(const float3 val)
    {
        float3 tmp;
        if (val.x > 0) tmp.x = 1;
        if (val.x < 0) tmp.x = -1;
        if (val.x == 0) tmp.x = 0;

        if (val.y > 0) tmp.y = 1;
        if (val.y < 0) tmp.y = -1;
        if (val.y == 0) tmp.y = 0;

        if (val.z > 0) tmp.z = 1;
        if (val.z < 0) tmp.z = -1;
        if (val.z == 0) tmp.z = 0;

    }
    

    void Euler(const unsigned& i, const float3& a, const float& dt)
    {
        next_v[i] = v[i] + a * dt;
        next_r[i] = r[i] + next_v[i] * dt;
        //std::cout << i << ") " << next_v[i] << " = " << v[i] << " + " << a << " * " << dt << std::endl;
        //std::cout << i << ") " << next_r[i] << " = " << r[i] << " + " << next_v[i] << " * " << dt << std::endl;
    }

    void initialize()
    {
        r.resize(size, make_float3(0.f, 0.f, 0.f));
        next_r.resize(size, make_float3(0.f, 0.f, 0.f));
        prev_r.resize(size, make_float3(0.f, 0.f, 0.f));
        v.resize(size, make_float3(0.f, 0.f, 0.f));
        next_v.resize(size, make_float3(0.f, 0.f, 0.f));
        color.resize(size, colorFactory.purple);
    }
    
    void putParticleInPhountain(const unsigned i)
    {
        color[i] = colorFactory.white;
        r[i] = phontain;
        const float x = rand() % 1000 + 1;
        const float y = rand() % 40 + 1;
        const float z = rand() % 45 + 0;
        const float sign = (rand() % 10) > 5 ? -1.f : 1.f;
        v[i] = make_float3(x * 0.1f, y * sign * 0.1f, z * sign * 0.1f);
    }

public:
    ParticleSystemPhountain(const unsigned size_, const int box_, W& wing_, const float3 phontain_)
        : size(size_), box(box_), wing(wing_), phontain(phontain_), mass(1.f), radius(0.0016f)
    {
        srand(time(NULL));
	    initialize();
    };

    void initializeParticle()
    {
        for (unsigned i = 0; i < size; i++)
            putParticleInPhountain(i);
    }

    void boxCollision(const unsigned i)
    {
        const float halfBox = box / 2.f;

        if (r[i].x < -halfBox || r[i].x > halfBox)
        {
            putParticleInPhountain(i);
        }
        if (r[i].y  < -halfBox)
        {
            r[i].y = -box - r[i].y;
            v[i].y = -v[i].y;
        }
        if (r[i].y > halfBox)
        {
            r[i].y = box - r[i].y;
            v[i].y = -v[i].y;
        }
        if (r[i].z < -halfBox)
        {
            r[i].z = -box - r[i].z;
            v[i].z = -v[i].z;
        }
        if (r[i].z > halfBox)
        {
            r[i].z = box - r[i].z;
            v[i].z = -v[i].z;
        }
    }
    
    void particleCollision(const unsigned i)
    {
        if (r[i].x < (-box / 2.f) + (box / 4.f))
            return;

        const float radius2 = radius + radius;

        for (unsigned j = 0; j < size; j++)
        //for (unsigned j = i + 1; j < size; j++)
        {
            //if (i == j)
            //   continue;

            const float distance = lenght(r[i] - r[j]);
            if (distance < radius2)
            {
                color[i] = colorFactory.red;
                v[i] = v[i] * (-1);
            }
        }
    }

    void wingCollision(const unsigned i)
    {
        const float3 wingPos = make_float3(wing.getPos().x, wing.getPos().y, r[i].z);
        const float doubleRadius = radius + wing.getRadius();
        const float distance = lenght(r[i] - wingPos);

        if (distance <= doubleRadius)
        {
            const float gamma = radius / 4.f;
            const float moveDist = ((doubleRadius - distance) / 2.f) + gamma;

            const float3 movePos = normalize(v[i]) * (-moveDist);
            r[i] = r[i] + movePos;
            
            if (r[i].y >= 0)
                r[i].y += radius;
            if (r[i].y < 0)
                r[i].y -= radius;

            color[i] = colorFactory.green;

            wing.addForce(abs(v[i].y) * mass);
            //std::cout << "\t" << abs(v[i].y) * mass << " " << wing.getForce() << std::endl;
        }
    }

    void prepareMove(const unsigned& i, const float3& f, const float& dt)
    {
        const float3 a = f * (1.0f / mass);
        Euler(i, a, dt);
    }

    void move()
    {
        prev_r = r;
        r = next_r;
        v = next_v;
    }

    void reset()
    {
        next_r.clear();
        prev_r.clear();
        v.clear();
        next_v.clear();
        color.clear();

        initialize();
        initializeParticle();
    }

    float4 getColor(const unsigned i) const 
    {
        return color[i];
    }

    float3 getPos(const unsigned i) const 
    {
        return r[i];
    }

    float3 getVel(const unsigned i) const 
    {
        return v[i];
    }

    float getEnergy()
    {
        float energy = 0.f;
        for (unsigned i = 0; i < size; i++)
            energy += (lenght(v[i]) * mass / 2.f);

        return energy;
    }

    void printSystem(std::string txt = "") const 
    {
        for (int i = 0; i < size; i++)
            std::cout << txt << " r[i]: " << r[i] << "\tv[i]: " << v[i] 
                      << "\tnext_r[i]: " << next_r[i] << "\tnext_v[i]: " << next_v[i]
                      << std::endl;

        std::cout << std::endl;
    }
};
