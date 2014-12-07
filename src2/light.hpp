#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>
#include <string>

#include "algebra.hpp"
#include "scene_obj.hpp"

class Light
{
  public:
    std::string name;
    double* falloff;
    double intensity;
    Point3D position;
    Colour colour;
    int size;
    std::string type;
    std::string plain;

    ~Light() 
    {
      delete falloff;
    }

    Colour getColour()
    {
      return colour * intensity;
    }
    
    Point3D getPosition(int sample, int sampleSize)
    {
        if(sampleSize == 1)
        {
            return position;
        }
        
        double widthOffset = sample % sampleSize;
        double heightOffset = (sample - sample % sampleSize) / sampleSize;
        
        widthOffset *= ((double)size / (double)(sampleSize - 1));
        heightOffset *= ((double)size / (double)(sampleSize - 1));
        
        widthOffset -= (double)size/2.0;
        heightOffset -= (double)size/2.0;
        
        Vector3D offset = Vector3D(0.0, 0.0, 0.0);
        
        if(type == "cube")
        {
            if(plain == "xz" || plain == "zx")
            {
                offset = Vector3D(widthOffset, 0, heightOffset);
            }
            else if(plain == "xy" || plain == "yx")
            {
                offset = Vector3D(widthOffset, heightOffset, 0);
            }
            else if(plain == "yz" || plain == "zy")
            {
                offset = Vector3D(0, widthOffset, heightOffset);
            }
        }
        
        return position + offset;
    }
};

#endif
