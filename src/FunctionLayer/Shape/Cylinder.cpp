#include "Cylinder.h"
#include "ResourceLayer/Factory.h"
bool Cylinder::rayIntersectShape(Ray &ray, int *primID, float *u, float *v) const {
    //* todo 完成光线与圆柱的相交 填充primId,u,v.如果相交，更新光线的tFar
    //* 1.光线变换到局部空间
    //* 2.联立方程求解
    //* 3.检验交点是否在圆柱范围内
    //* 4.更新ray的tFar,减少光线和其他物体的相交计算次数
    //* Write your code here.

    float tNear = ray.tNear, tFar = ray.tFar;
    Ray trans_ray = transform.inverseRay(ray);

    float ox = trans_ray.origin[0], dx = trans_ray.direction[0];
    float oy = trans_ray.origin[1], dy = trans_ray.direction[1];

    float A = dx * dx + dy * dy;
    float B = 2 * (dx * ox + dy * oy);
    float C = ox * ox + oy * oy - radius * radius;

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

void Cylinder::fillIntersection(float distance, int primID, float u, float v, Intersection *intersection) const {
    /// ----------------------------------------------------
    //* todo 填充圆柱相交信息中的法线以及相交位置信息
    //* 1.法线可以先计算出局部空间的法线，然后变换到世界空间
    //* 2.位置信息可以根据uv计算出，同样需要变换
    //* Write your code here.
    /// ----------------------------------------------------

    float phi = u * phiMax;
    float pz = v * height;

    Point3f localSpacePosition{
        radius * std::cos(phi), radius * std::sin(phi), pz
    };
    Vector3f localSpaceNormal{
        radius * std::cos(phi), radius * std::sin(phi), 0.f
    };
    intersection->position = transform.toWorld(localSpacePosition);
    intersection->normal = transform.toWorld(localSpaceNormal);

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

void Cylinder::uniformSampleOnSurface(Vector2f sample, Intersection *result, float *pdf) const {

}

Cylinder::Cylinder(const Json &json) : Shape(json) {
    radius = fetchOptional(json,"radius",1.f);
    height = fetchOptional(json,"height",1.f);
    phiMax = fetchOptional(json,"phi_max",2 * PI);
    AABB localAABB = AABB(Point3f(-radius,-radius,0),Point3f(radius,radius,height));
    boundingBox = transform.toWorld(localAABB);
}

REGISTER_CLASS(Cylinder,"cylinder")
