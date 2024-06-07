#include <stdio.h>
#include "ray.h"
#include "tri.h"
#include "rtx_material.h"
#include "sphere.h"
#include <math.h>

#define WIDTH 80
#define HEIGHT 60

#define CUBE_RADIUS 20

float ACESFilm(float val);

const rtx_material light = {
    {0.0, 0.0, 0.0}, 
    {0.99, 0.95, 0.78},
    1.5,
    0.0,
    0.0,
    {0.0, 0.0, 0.0},
};


const rtx_material specr = {
    {1.0, 0.2, 0.2}, //
    {1.0, 1.0, 1.0},
    0.0,
    1.0,
    0.4,
    {1.0, 1.0, 1.0},
};


const rtx_material specg = {
    {0.2, 1.0, 0.2}, //
    {1.0, 1.0, 1.0},
    0.0,
    1.0,
    0.4,
    {1.0, 1.0, 1.0},
};


const rtx_material diffuse = {
    {1.0, 0.5, 1.0}, 
    {0.0, 0.0, 0.0},
    0.0,
    0.0,
    0.0,
    {0.0, 0.0, 0.0}
};

const rtx_material ground = {
    {1.0, 0.9, 0.9},
    {0.0, 0.0, 0.0},
    0.0,
    0.0,
    0.0,
    {0.0, 0.0, 0.0}
};

const rtx_material metal = {
    {1.0, 1.0, 1.0},
    {0.0, 0.0, 0.0},
    0.0,
    1.0,
    1.0,// 0.91
    {1.0, 1.0, 1.0}
};    

vec3 cube_center = {-25, 35, 0};



vec3 samples[WIDTH * HEIGHT];
// Set to 0 in actual implementation
int total_samples_taken = 1;

int main() {

    sphere spheres[NUM_SPHERES];
    tri tris[NUM_TRIS];

    sphere l = {
        {100.0f, -15.0f, 100.0f},
        100.0f,
        light
    };

    sphere g = {
        {0.0f, 40.0f, -45000.0f},
        45000.0f,
        ground
    };

    sphere one = {
        {0.0f, 80.0f, 25.0f},
        20.0f,
        diffuse
    };

    sphere two = {
        {-50.0f, 80.0f, 25.0f},
        20.0f,
        specr 
    };

    sphere three = {
        {50.0f, 80.0f, 25.0f},
        20.0f,
        metal 
    };

    spheres[0] = l;
    spheres[1] = g;
    spheres[2] = one;
    spheres[3] = two;
    spheres[4] = three;

    vec3 v0 = {-CUBE_RADIUS, 0, CUBE_RADIUS};
    vec3 v1 = {0, -CUBE_RADIUS, CUBE_RADIUS};
    vec3 v2 = {CUBE_RADIUS, 0, CUBE_RADIUS};
    vec3 v3 = {0, CUBE_RADIUS, CUBE_RADIUS};
    vec3 v4 = {-CUBE_RADIUS, 0, 0};
    vec3 v5 = {0, -CUBE_RADIUS, 0};
    vec3 v6 = {CUBE_RADIUS, 0, 0};
    vec3 v7 = {0, CUBE_RADIUS, 0};

    v0 = add(v0, cube_center);
    v1 = add(v1, cube_center);
    v2 = add(v2, cube_center);
    v3 = add(v3, cube_center);
    v4 = add(v4, cube_center);
    v5 = add(v5, cube_center);
    v6 = add(v6, cube_center);
    v7 = add(v7, cube_center);

    rtx_material tri_material = specg;

    tri t0 = {v0, v4, v5, tri_material};
    tri t1 = {v0, v5, v1, tri_material};
    tri t2 = {v1, v5, v6, tri_material};
    tri t3 = {v1, v6, v2, tri_material};
    tri t4 = {v2, v6, v7, tri_material};
    tri t5 = {v2, v7, v3, tri_material};
    tri t6 = {v0, v1, v3, tri_material};
    tri t7 = {v3, v1, v2, tri_material};

    tris[0] = t0;
    tris[1] = t1;
    tris[2] = t2;
    tris[3] = t3;
    tris[4] = t4;
    tris[5] = t5;
    tris[6] = t6;
    tris[7] = t7;

    int maxBounces = 5;
    int samples_per_pixel = 100;

    vec3 camera_pos = {0.0, -40.0, 30.0};

    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen("ppmout.ppm", "w");


    // Write some text to the file
    fprintf(fptr, "P3\n%d %d\n255\n", WIDTH, HEIGHT);
    // printf("P3\n%d %d\n255\n", WIDTH, HEIGHT);
    
    int total_samples = WIDTH * HEIGHT;
    for(int j = HEIGHT - 1; j >= 0; j--){
        for(int i = 0; i < WIDTH; i++){

            vec3 attenuated_color = {0.0f, 0.0f, 0.0f};


            // printf("Checking for hit on sphere, direction %.2f %.2f %.2f\n", r.dir.x, r.dir.y, r.dir.z);
            for(int sample = 0; sample < samples_per_pixel; sample++){
                vec3 direction = {(float)(i + random_f() - WIDTH/2), (float)WIDTH, (float)(j + random_f() - HEIGHT/2)};
                direction = normalize(direction);
                ray r = {camera_pos, direction};

                vec3 pixel_color = trace(r, maxBounces, spheres, tris);
                attenuated_color = add(attenuated_color, pixel_color);
            }

            int current_pixel = (HEIGHT - j - 1) * WIDTH + i;

            attenuated_color = scale(attenuated_color, 1.0f / (float)samples_per_pixel);
            attenuated_color.x = ACESFilm(attenuated_color.x);
            attenuated_color.y = ACESFilm(attenuated_color.y);
            attenuated_color.z = ACESFilm(attenuated_color.z);

            samples[current_pixel] = add(attenuated_color, samples[current_pixel]);

            attenuated_color = scale(attenuated_color, 1.0 / total_samples_taken);

            unsigned char x = i; 
            unsigned char y = HEIGHT - j - 1;
            unsigned char final_color = ((int)(attenuated_color.x*255) & 224) | (((int)(attenuated_color.y*255) & 224) >> 3) | (((int)(attenuated_color.z*255) & 192) >> 6);

            fprintf(fptr, "%d %d %d\n", (int)(attenuated_color.x*255) & 224, (int)(attenuated_color.y*255) & 224, (int)(attenuated_color.z*255) & 192);
            //fprintf(fptr, "%d %d %d\n", (int)(attenuated_color.x*255), (int)(attenuated_color.y*255), (int)(attenuated_color.z*255));
        }
    }

    // Close the file
    fclose(fptr); 
    return 0;
}

float ACESFilm(float val)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float tone_mapped = (val*(a*val+b))/(val*(c*val+d)+e);
    return 0.0 <= tone_mapped ? ((1.0 >= tone_mapped) ? tone_mapped : 1.0) : 0.0;
}