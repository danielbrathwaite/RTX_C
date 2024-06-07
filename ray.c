/*
 * ray.c
 *
 *  Created on: Jun 5, 2024
 *      Author: daniel
 */

#include "ray.h"
#include "tri.h"
#include "sphere.h"
#include "hitInfo.h"
#include "math.h"
#include <stdio.h>


vec3 at(ray r, float t) {
    vec3 at = add(r.origin, scale(r.dir, t)); 
    return at;
}


float hit_sphere(ray r, sphere s) {

    vec3 oc = subtract(r.origin, s.center);
    float a = dot(r.dir, r.dir);
    float b = 2.0f * dot(oc, r.dir);
    float c = dot(oc, oc) - (s.radius * s.radius);
    float discriminant = b*b - 4.0f*a*c;

    // printf("{%.2f %.2f %.2f}  %.2f\n",r.dir.x, r.dir.y, r.dir.z, a);

    if (discriminant < 0.0f) {
        return -1.0f;
    } else {
        return (-b - sqrtf(discriminant) ) / (a*2.0f);
    }
}

hitInfo calculateRayCollision(ray r, sphere *spheres){
    hitInfo hitInfo; 
    hitInfo.hit = 0;

    for(int i = 0; i < NUM_SPHERES; i++){
        sphere sphere = spheres[i];
        float t = hit_sphere(r, sphere);
        if (t > 0.0001f && (t < hitInfo.t || !hitInfo.hit)) {
            hitInfo.hit = 1;
            hitInfo.t = t;
            hitInfo.normal = normalize(subtract(at(r, t), sphere.center));
            hitInfo.hitPoint = at(r, t);
            hitInfo.material = sphere.material;
        }
    }

    return hitInfo;
}


vec3 trace(ray ray, int maxBounceCount, sphere *spheres){
    vec3 incomingLight = {0.0, 0.0, 0.0};   // Start with no incoming light
    vec3 rayColor = {1.0, 1.0, 1.0};        // Start with white light, lose on hit

    for(int i = 0; i <= maxBounceCount; i++){

        hitInfo hitInfo = calculateRayCollision(ray, spheres);
        rtx_material material = hitInfo.material;

        if(hitInfo.hit){

            ray.origin = hitInfo.hitPoint;
            vec3 diffuseDir = normalize(add(hitInfo.normal, random_in_unit_sphere()));
            vec3 specularDir = reflect(ray.dir, hitInfo.normal);

            int isSpecularBounce = material.specularProbability >= random_f();

            ray.dir = lerp(diffuseDir, specularDir, material.smoothness * isSpecularBounce);

            vec3 emittedLight = scale(material.emissionColor, material.emissionStrength);
            incomingLight = add(incomingLight, multiply(emittedLight, rayColor));

            if (isSpecularBounce) {
                rayColor = multiply(rayColor, material.specularColor);
            } else {
                rayColor = multiply(rayColor, material.color);
            }
        }else{
            vec3 environmentLight = {0.5, 0.8, 0.9};
            //incomingLight = add(incomingLight, environmentLight); 
            break;
        }

    }

    // printf("Color of current pixel: {%d, %d, %d}\n", incomingLight.x(), incomingLight.y(), incomingLight.z());

    return incomingLight;
}








