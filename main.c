#include <stdio.h>
#include "ray.h"
#include "rtx_material.h"
#include "sphere.h"
#include <math.h>


rtx_material light = {
    {0.0, 0.0, 0.0}, 
    {0.99, 0.95, 0.78},
    1.5,
    0.0,
    0.0,
    {0.0, 0.0, 0.0},
};


rtx_material spec = {
    {0.2, 0.2, 1.0}, //
    {1.0, 1.0, 1.0},
    0.0,
    1.0,
    0.2,
    {1.0, 1.0, 1.0},
};


rtx_material diffuse = {
    {1.0, 0.5, 1.0}, 
    {0.0, 0.0, 0.0},
    0.0,
    0.0,
    0.0,
    {0.0, 0.0, 0.0}
};

rtx_material ground = {
    {1.0, 0.9, 0.9},
    {0.0, 0.0, 0.0},
    0.0,
    0.0,
    0.0,
    {0.0, 0.0, 0.0}
};

rtx_material metal = {
    {0.9, 0.9, 0.1},
    {0.0, 0.0, 0.0},
    0.0,
    0.99,
    0.91,
    {1.0, 1.0, 1.0}
};    


int main() {

    //vec3 check = {1.0, 1.0, 1.0};
    //vec3 origin = {0.0, 0.0, 0.0};
    //ray testing = {&origin, &check};
    //printf("%f %f %f\n", testing.dir->x, testing.dir->y, testing.dir->z);
    //vec3 norm = at(&testing, 4.0f);
    //printf("%f %f %f\n", norm.x, norm.y, norm.z);

    //exit(0);
    sphere spheres[NUM_SPHERES];

    sphere l = {
        {100.0f, -15.0f, 100.0f},
        100.0f,
        &light
    };

    sphere g = {
        {0.0f, 40.0f, -45000.0f},
        45000.0f,
        &ground
    };

    sphere one = {
        {0.0f, 80.0f, 10.0f},
        20.0f,
        &diffuse
    };

    sphere two = {
        {-20.0f, 80.0f, 10.0f},
        20.0f,
        &ground 
    };

    spheres[0] = l;
    spheres[1] = g;
    spheres[2] = one;
    spheres[3] = two;

    int width = 200;
    int height = 150;

    int maxBounces = 5;
    int samples_per_pixel = 100;

    vec3 camera_pos = {0.0, -4.0, 1.0};


    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen("ppmout.ppm", "w");




    // Write some text to the file
    fprintf(fptr, "P3\n%d %d\n255\n", width, height);
    // printf("P3\n%d %d\n255\n", width, height);
    
    int total_samples = width * height;
    for(int j = height - 1; j >= 0; j--){
        for(int i = 0; i < width; i++){

            vec3 attenuated_color = {0.0f, 0.0f, 0.0f};


            // printf("Checking for hit on sphere, direction %.2f %.2f %.2f\n", r.dir.x, r.dir.y, r.dir.z);
            for(int sample = 0; sample < samples_per_pixel; sample++){
                vec3 direction = {(float)(i + 0.5 - width/2), (float)width, (float)(j + 0.5 - height/2)};
                direction = normalize(&direction);
                ray r = {&camera_pos, &direction};

                vec3 pixel_color = trace(&r, maxBounces, spheres);
                attenuated_color = add(&attenuated_color, &pixel_color);
            }

            int current_pixel = (height - j - 1) * width + i;

            attenuated_color = scale(&attenuated_color, 1.0f / (float)samples_per_pixel);

             // Could implement filter here to make output look better

            fprintf(fptr, "%d %d %d\n", (int)(attenuated_color.x*255), (int)(attenuated_color.y*255), (int)(attenuated_color.z*255));
        }
    }

    // Close the file
    fclose(fptr); 
    return 0;
}