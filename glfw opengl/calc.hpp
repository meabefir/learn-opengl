#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

inline glm::vec3 getLinePlaneIntersection(glm::vec3& p1, glm::vec3& p2, float a = 0, float b = 1, float c = 0, float d = 0) {
    float dir_x = p2.x - p1.x;
    float dir_y = p2.y - p1.y;
    float dir_z = p2.z - p1.z;

    float t = -(a * p1.x + b * p1.y + c * p1.z + d) / (a * dir_x + b * dir_y + c * dir_z);

    float int_x = p1.x + t * dir_x;
    float int_y = p1.y + t * dir_y;
    float int_z = p1.z + t * dir_z;

    return glm::vec3(int_x, int_y, int_z);
}

inline float signed_tetra_volume(glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& d) {
    return glm::sign(glm::dot(glm::cross(b - a, c - a), d - a) / 6.f);
}

inline bool line_intersect_triangle(glm::vec3 q1, glm::vec3 q2, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    float s1 = signed_tetra_volume(q1, p1, p2, p3);
    float s2 = signed_tetra_volume(q2, p1, p2, p3);

    if (s1 != s2) {
        float s3 = signed_tetra_volume(q1, q2, p1, p2);
        float s4 = signed_tetra_volume(q1, q2, p2, p3);
        float s5 = signed_tetra_volume(q1, q2, p3, p1);

        if (s3 == s4 && s4 == s5) {
            return true;
        }
    }
    return false;
}

inline float distance_point_to_line(glm::vec3& p1, glm::vec3& p2, glm::vec3& v) {
    return glm::length(glm::cross(v-p1, v-p2)) / glm::length(p2 - p1);
}