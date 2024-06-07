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


vec3 at(ray *r, float t) {
    vec3 t1 = scale(r->dir, t);
    vec3 at = add(r->origin, &t1); 
    return at;
}


float hit_sphere(ray *r, sphere *s) {

    vec3 oc = subtract(r->origin, &s->center);
    float a = dot(r->dir, r->dir);
    float b = 2.0f * dot(&oc, r->dir);
    float c = dot(&oc, &oc) - (s->radius * s->radius);
    float discriminant = b*b - 4.0f*a*c;

    // printf("{%.2f %.2f %.2f}  %.2f\n",r.dir.x, r.dir.y, r.dir.z, a);

    if (discriminant < 0.0f) {
        return -1.0f;
    } else {
        return (-b - sqrtf(discriminant) ) / (a*2.0f);
    }
}

hitInfo calculateRayCollision(ray *r, sphere *spheres){
    hitInfo hitInfo; 
    hitInfo.hit = 0;

    for(int i = 0; i < NUM_SPHERES; i++){
        sphere *sphere = spheres + i;
        float t = hit_sphere(r, sphere);
        if (t > 0.0001f && (t < hitInfo.t || !hitInfo.hit)) {
            hitInfo.hit = 1;
            hitInfo.t = t;
            
            vec3 t1 = at(r, t);
            vec3 t2 = subtract(&t1, &sphere->center);

            *hitInfo.normal = normalize(&t2);
            *hitInfo.hitPoint = at(r, t);
            hitInfo.material = sphere->material;
        }
    }

    return hitInfo;
}


vec3 trace(ray *ray, int maxBounceCount, sphere *spheres){
    vec3 incomingLight = {0.0, 0.0, 0.0};   // Start with no incoming light
    vec3 rayColor = {1.0, 1.0, 1.0};        // Start with white light, lose on hit

    for(int i = 0; i <= maxBounceCount; i++){

        hitInfo hitInfo = calculateRayCollision(ray, spheres);
        rtx_material *material = hitInfo.material;

        if(hitInfo.hit){
            vec3 half = {0.5, 0.5, 0.5};
            vec3 t0 = scale(hitInfo.normal, 0.5f);
            return add(&half, &t0); 

            ray->origin = hitInfo.hitPoint;
            vec3 random_unit_v = random_in_unit_sphere();
            vec3 t1 = add(hitInfo.normal, &random_unit_v);
            vec3 diffuseDir = normalize(&t1);
            vec3 specularDir = reflect(ray->dir, hitInfo.normal);

            int isSpecularBounce = material->specularProbability >= random_f();

            *ray->dir = lerp(&diffuseDir, &specularDir, material->smoothness * isSpecularBounce);

            vec3 emittedLight = scale(&material->emissionColor, material->emissionStrength);
            vec3 t2 = multiply(&emittedLight, &rayColor);
            incomingLight = add(&incomingLight, &t2);

            if (isSpecularBounce) {
                rayColor = multiply(&rayColor, &material->specularColor);
            } else {
                rayColor = multiply(&rayColor, &material->color);
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


/* float hit_tri(ray *r, tri *tr){
    vec3 a = subtract(&tr->v1, &tr->v0);    // edge 0
    vec3 b = subtract(&tr->v2, &tr->v0);    // edge 1
    vec3 n = cross(&a, &b);                 // this is the triangle's normal
    n = normalize(n); 

    double d =-dot(&n, &tr->v0);
    double t = - (dot(&n, &r->origin) + d) / dot(&n, &r->dir);

    if(t < 0){return -1.0;}                 // no hit

    vec3 p = r.origin + t * r.dir;

    vec3 edge0 = subtract(tr.v1, tr.v0);
    vec3 edge1 = subtract(tr.v2, tr.v1);
    vec3 edge2 = subtract(tr.v0, tr.v2);
    vec3 C0 = subtract(p, tr.v0);
    vec3 C1 = subtract(p, tr.v1);
    vec3 C2 = subtract(p, tr.v2);

    if (dot(n, cross(edge0, C0)) > 0 && 
        dot(n, cross(edge1, C1)) > 0 &&
        dot(n, cross(edge2, C2)) > 0) return t; // P is inside the triangle

    return -1.0;
} */






