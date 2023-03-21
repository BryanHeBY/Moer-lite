#include "Cone.h"
#include "ResourceLayer/Factory.h"

bool Cone::rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const {
    //* todo 完成光线与圆柱的相交 填充primId,u,v.如果相交，更新光线的tFar
    //* 1.光线变换到局部空间
    //* 2.联立方程求解
    //* 3.检验交点是否在圆锥范围内
    //* 4.更新ray的tFar,减少光线和其他物体的相交计算次数
    //* Write your code here.

    float tNear = ray.tNear, tFar = ray.tFar;
    Ray trans_ray = transform.inverseRay(ray);

    Vector3f CO = trans_ray.origin - Point3f{0.f, 0.f, height};
    float COz = CO[2];
    float Dz = trans_ray.direction[2];

    float sqr_cosTheta = cosTheta * cosTheta;
    float A = Dz * Dz - sqr_cosTheta;
    float B = 2 * (Dz * COz - dot(trans_ray.direction, CO) * sqr_cosTheta);
    float C = COz * COz - dot(CO, CO) * sqr_cosTheta;

    float t0, t1;
    bool solution_exist = Quadratic(A, B, C, &t0, &t1);
    
    if(!solution_exist)
        return false;

    auto check_intersect = [&](float t) {
        if(t < tNear || t > tFar)
            return false;

        Point3f p = trans_ray.at(t);

        if(p[2] < 0.f || p[2] > height)
            return false;

        float phi = std::atan2(p[1], p[0]);
        if(phi < 0.f) phi += 2 * fm::pi_f;
        if(phi > phiMax)
            return false;

        ray.tFar = t;
        *primID = 0;
        *u = phi / phiMax;
        *v = p[2] / height;
        return true;
    };

    if(check_intersect(t0)) return true;
    else return check_intersect(t1);
}

void Cone::fillIntersection(float distance, int primID, float u, float v, Intersection *intersection) const {
    /// ----------------------------------------------------
    //* todo 填充圆锥相交信息中的法线以及相交位置信息
    //* 1.法线可以先计算出局部空间的法线，然后变换到世界空间
    //* 2.位置信息可以根据uv计算出，同样需要变换
    //* Write your code here.
    /// ----------------------------------------------------

    float phi = u * phiMax;
    float pz = v * height;

    Point3f M {
        radius * std::cos(phi), radius * std::sin(phi), pz
    };
    Vector3f CM = M - Point3f{0.f, 0.f, height};
    float CM_N = std::sqrt(dot(CM, CM));
    Point3f K {
        0.f, 0.f, height - CM_N / cosTheta
    };
    Vector3f KM = M - K;

    intersection->position = transform.toWorld(M);
    intersection->normal = transform.toWorld(normalize(KM));

    intersection->shape = this;
    intersection->distance = distance;
    intersection->texCoord = Vector2f{u, v};
    Vector3f tangent{1.f, 0.f, .0f};
    Vector3f bitangent;
    if (std::abs(dot(tangent, intersection->normal)) > .9f) {
        tangent = Vector3f(.0f, 1.f, .0f);
    }
    bitangent = normalize(cross(tangent, intersection->normal));
    tangent = normalize(cross(intersection->normal, bitangent));
    intersection->tangent = tangent;
    intersection->bitangent = bitangent;
}

void Cone::uniformSampleOnSurface(Vector2f sample, Intersection *result, float *pdf) const {

}

Cone::Cone(const Json &json) : Shape(json) {
    radius = fetchOptional(json, "radius", 1.f);
    height = fetchOptional(json, "height", 1.f);
    phiMax = fetchOptional(json, "phi_max", 2 * PI);
    float tanTheta = radius / height;
    cosTheta = sqrt(1/(1+tanTheta * tanTheta));
    //theta = fetchOptional(json,)
    AABB localAABB = AABB(Point3f(-radius,-radius,0),Point3f(radius,radius,height));
    boundingBox = transform.toWorld(localAABB);
    boundingBox = AABB(Point3f(-100,-100,-100),Point3f(100,100,100));
}

REGISTER_CLASS(Cone, "cone")
