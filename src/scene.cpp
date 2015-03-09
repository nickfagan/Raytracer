#include <math.h>

#include "scene.hpp"

using namespace std;

/****************************************************************************/
// Scene Feature Values
/****************************************************************************/

#define ANTI_ALIAS 4            // 4x4 = 16 extra rays per pixel
#define SOFT_SHADOW_SAMPLES 3   // 3x3 = 9 extra light sample rays

/****************************************************************************/
// Other variables
/****************************************************************************/

#define BACKGROUND_COLOR Colour(0.4, 0.4, 0.4)
#define REFLECTIVITY_DEPTH 5
#define AIR_REFRACTION_INDICE 1.00029

/****************************************************************************/
// Initialization
/****************************************************************************/

void Scene::init(int width, int height)
{
    viewDir.normalize();
    upDir.normalize();
    rightDir = viewDir.cross(upDir);
    rightDir.normalize();
    tanf = tan((fieldOfView/2)*M_PI/180);
    aspect = (double)width/(double)height;
    
    //TODO: this
    ambient = Colour(0.3, 0.3, 0.3);
}

/****************************************************************************/
// Ray Tracer
/****************************************************************************/

/* Renders part of the scene. Uses pixelStart and pixelEnd to figure out what x/y pixel to start
   and end at */
void Scene::raytrace(Image* image, int pixelStart, int pixelEnd)
{
    int width = image->width();
    int height = image->height();
    
    // Get the x pixel to start at
    int pixelX = pixelStart % width;
    // Get the y pixel to start at
    int pixelY = (pixelStart - pixelStart % width) / width;
    
    // Need to keep track of the number of pixels we rendered in order to stop at the correct spot
    int pixelCount = pixelStart;
    
    // for each y pixel
    for(; pixelY < height && pixelCount < pixelEnd; pixelY++)
    {
        // for each x pixel
        for(; pixelX < width && pixelCount < pixelEnd; pixelX++, pixelCount++)
        {
            Point3D origin = eyePos;
            double r = 0;
            double g = 0;
            double b = 0;
            
            // for each pixel, do a number of samples in order to anti alias the scene
            for(int i = 0; i < ANTI_ALIAS; i++)
            {
                for(int j = 0; j < ANTI_ALIAS; j++)
                {
                    double rayX = (double)pixelX + (double)i / (double)ANTI_ALIAS;
                    double rayY = (double)pixelY + (double)j / (double)ANTI_ALIAS;
                    
                    Vector3D direction = viewDir + (((double)rayX/(double)width)-0.5)*tanf*aspect*rightDir
                    + (((double)rayY/(double)height)-0.5)*tanf*upDir;
                    direction.normalize();
                    
                    Colour pixel_colour = trace(origin, direction, 1);
                    
                    r += pixel_colour.R();
                    g += pixel_colour.G();
                    b += pixel_colour.B();
                }
            }
            
            int pixelSamples = ANTI_ALIAS*ANTI_ALIAS;
            (*image)(pixelX, height-pixelY-1, 0) = r / (double)pixelSamples;
            (*image)(pixelX, height-pixelY-1, 1) = g / (double)pixelSamples;
            (*image)(pixelX, height-pixelY-1, 2) = b / (double)pixelSamples;
        }
        
        // Set the pixel x to 0 in order to render the next "line" of the image
        pixelX = 0;
    }
}

Colour Scene::trace(Point3D origin, Vector3D direction, int depth)
{
    IInfo iInfo;
    if(!intersect(origin, direction, iInfo))
    {
        return BACKGROUND_COLOR;
    }

    /*******************************/
    // Get Material information
    /*******************************/
    
    Colour diffused = iInfo.material->diffused;
    Colour specular = iInfo.material->specular;
    double shininess = iInfo.material->shininess;
    double reflectivity = iInfo.material->reflectivity;
    double trancparency = iInfo.material->transparency;
    double refractionI = iInfo.material->refractionI;
    
    if(reflectivity > 1.0) reflectivity = 1.0;
    else if(reflectivity < 0.0) reflectivity = 0.0;
    
    if(trancparency > 1.0) trancparency = 1.0;
    else if(reflectivity < 0.0) trancparency = 0.0;
    
    if(reflectivity + reflectivity > 1.0)
    {
        cout << "[WARNING] reflectivity + reflectivity > 1.0" << endl;
        double diff = (reflectivity + reflectivity) - 1.0;
        reflectivity -= diff / 2.0;
        trancparency -= diff / 2.0;
    }
    
    /*******************************/
    // Apply lighting and shadows
    /*******************************/
    
    Colour surfaceColour = diffused*ambient;

    for (int i = 0; i < lights.size(); i++)
    {
        Vector3D lightDir = lights[i]->position - iInfo.point;
        double lightDistance = lightDir.length();
        lightDir.normalize();
        
        double lambentCoef = lightDir.dot(iInfo.normal);
        if(lambentCoef < 0)
        {
            continue;	// intersection point not facing light
        }
        
        double totalSamples = SOFT_SHADOW_SAMPLES * SOFT_SHADOW_SAMPLES;
        double lightsamples = totalSamples;
        for(int j = 0; j < totalSamples; j++)
        {
            Vector3D sampleDir = lights[i]->getPosition(j, SOFT_SHADOW_SAMPLES) - iInfo.point;
            double distance = sampleDir.length();
            sampleDir.normalize();
            
            IInfo lightIInfo;
            if(intersect(iInfo.point, sampleDir, lightIInfo) && lightIInfo.t < distance)
            {
                lightsamples--;
            }
        }
        
        if(lightsamples == 0)
        {
            continue;
        }
        
        Vector3D r = lightDir - 2.0 * lightDir.dot(iInfo.normal) * iInfo.normal;
        r.normalize();
        
        double dot = r.dot(direction);
        dot = (dot < 0) ? 0 : dot;
        
        double attenuation = 1 / (lights[i]->falloff[0] + lights[i]->falloff[1]*lightDistance + lights[i]->falloff[2]*lightDistance*lightDistance);
        
        // Adjust pixel colour to account for this lights lambent light + specular reflectivity
        surfaceColour = surfaceColour + (lightsamples/totalSamples)*(diffused * lambentCoef * lights[i]->getColour() * attenuation
        + specular * powf(dot, shininess) * lights[i]->getColour() * attenuation);
    }

    /*******************************/
    // Apply transparency (refaction)
    /*******************************/
    
    Colour transparentColour = Colour(0);
    
    /*
    if(transparency > 0)
    {
        int n1, n2;
        
        // Not inside an object, refracting off outer surface
        if(insideObj == NULL)
        {
            n1 = refractionI;
            n2 = AIR_REFRACTION_INDICE;
        }
        // Inside an object, refracting off inner surface
        else
        {
            n1 = AIR_REFRACTION_INDICE;
            n2 = refractionI;
        }
        
        //TODO: calc refraction
    }
    */
    
    /*******************************/
    // Apply reflectivity (reflection)
    /*******************************/
    
    Colour reflectedColor = Colour(0);
    
    // If the object is reflective, get the color of the reflected object and apply that to the surface color
    if(reflectivity > 0 && depth < REFLECTIVITY_DEPTH) {
        Point3D refOrigin = iInfo.point;
        Vector3D refDir = direction - 2*direction.dot(iInfo.normal)*iInfo.normal;
        refDir.normalize();
        reflectedColor = trace(refOrigin, refDir, depth + 1);
    }
    
    double surfaceColourAmt = 1.0 - (reflectivity + trancparency);
    return surfaceColour * surfaceColourAmt + transparentColour * trancparency + reflectedColor * reflectivity;
}

bool Scene::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{
    root->intersect(rayOrigin, rayDir, iInfo);
    if(iInfo.t != -1)
    {
        iInfo.normal.normalize();
        return true;
    }
    
    return false;
}