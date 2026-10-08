// Standard library includes
#include <algorithm>
#include <cmath>
#include <vector>
#include <limits> 
#include <random>

// Project includes
#include <tira/graphics/camera.h>

extern std::vector<unsigned char> image_buffer;
extern size_t s_exp;

tira::graphics::Camera camera;

float RandomNum() {
    static std::mt19937_64 rng(42); // fixed seed so runs are reproducible
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
/* Sphere struct defines properties of a sphere
p is position in space :: a is the radius :: e is emission property :: d is diffuse property*/
struct sphere {
    glm::vec3 pos;
    double area;
    glm::vec3 emissive;
    glm::vec3 diffuse; 

    sphere(glm::vec3 p, double a, glm::vec3 e, glm::vec3 d) {
        pos = p;
        area = a;
        emissive = e;
        diffuse = d;
    }
};

std::vector<sphere> groupOfSpheres;
std::vector<glm::vec3> accumulation_buffer;
size_t pass_count = 0;
glm::vec3 prevCameraPosition;
float prevFOV;
/* hitSphere calculates how a ray intersects with a sphere returns a boolean true if theres an intersection*/
bool hitSphere(glm::vec3 centerPos, float radius, glm::vec3 direction, glm::vec3 start, float& t) {
    glm::vec3 originToSphere = centerPos - start;

    auto a = dot(direction, direction);
    auto b = -2 * dot(direction, originToSphere);
    auto c = dot(originToSphere, originToSphere) - radius * radius;

    auto discriminant = b * b -4 * a * c;
    
    if (discriminant < 0) {
        return false;
    }

    float sqrtD = std::sqrt(discriminant);

    float t1 = (-b - sqrtD) / (2 * a);
    float t2 = (-b + sqrtD) / (2 * a);

    if (t1 >= 0) {
        t = t1;
        return true;
    }

    if (t2 >= 0) {
        t = t2;
        return true;
    }

    return false;
}


/*
 * This function is called when the program is loaded. Use it to initialize
 * the path tracer as necessary. For example, you can load models (like
 * spheres), initialize data structures, etc.
 */
void InitializePathTracer() {
  //place spheres in space
    groupOfSpheres.clear();
  //place 4 along the axis
    for (int i = 0; i < 4; i++) {
        groupOfSpheres.push_back(
            sphere(
                glm::vec3(-1.5f + i * 1.0f, 0.0f, -3.0f),           //position
                0.4f,                                               //area
                glm::vec3(i % 2, (i + 1) % 2, i % 2 ),              //emissive
                glm::vec3(0, 0, 0)                                  //diffuse
            )
        );

    }

  // place more spheres along some line
    for (int i = 0; i < 3; i++) {
        groupOfSpheres.push_back(
            sphere(
                glm::vec3(-1.5f + i * 1.0f, 3.0f, -3.0f),           //position
                0.4f,                                               //area
                glm::vec3(0.0f),                                    //emissive
                glm::vec3((i + 1) % 2, (i + 1) %2, i % 2)            //diffuse
             )
        );

    }
    groupOfSpheres.push_back(
        sphere(
            glm::vec3(-1.5f, -3.0f, -3.0f),                     //position
            0.4f,                                               //area
            glm::vec3(0.0f),                                    //emissive
            glm::vec3(1.0f,0.0f, 0.0f)                          //diffuse
        )
    );
    groupOfSpheres.push_back(
        sphere(
            glm::vec3(-1.5f +1.0f, -3.0f, -3.0f),               //position
            0.4f,                                               //area
            glm::vec3(0.0f),                                    //emissive
            glm::vec3(0.0f, 1.0f, 0.0f)                         //diffuse
        )
    );
    groupOfSpheres.push_back(
        sphere(
            glm::vec3(-1.5f + 2.0f, -3.0f, -3.0f),           //position
            0.4f,                                               //area
            glm::vec3(0.0f),                                    //emissive
            glm::vec3(0.0f, 0.0f, 1.0f)            //diffuse
        )
    );

  // allocate the image buffer and clear it (set it to zero)
  if (s_exp > 0) {
    image_buffer.resize(pow(2, s_exp) * pow(2, s_exp) * 3);
    std::fill(image_buffer.begin(), image_buffer.end(), 0);
  }

  size_t s = std::pow(2, s_exp);
  accumulation_buffer.resize(s * s, glm::vec3(0.0f));
  prevCameraPosition = camera.Position();
  prevFOV = camera.FieldOfView();
}





/**
 * This function is called every time the user interface is rendered. When
 * this function returns, the value of the image buffer will be displayed.
 * Make sure that it returns in a reasonable time so that you have some
 * visual feedback ever frame.
 */
void UpdatePathTracer() {

  size_t s = std::pow(2, s_exp);
  float dx = 1.0f / s;

  glm::vec3 currentCameraPosition = camera.Position();
  float currentFOV = camera.FieldOfView();

  if (currentCameraPosition != prevCameraPosition ||
      currentFOV != prevFOV) {

      std::fill(
          accumulation_buffer.begin(),
          accumulation_buffer.end(),
          glm::vec3(0.0f)
      );

      pass_count = 0;

      prevCameraPosition = currentCameraPosition;
      prevFOV = currentFOV;
  }

  pass_count++;
  // iterate across all pixels in the image buffer
  for (size_t yi = 0; yi < s; yi++) {
    for (size_t xi = 0; xi < s; xi++) {
      int bounceCount = 0;
      // this gives you a vector from the camera through a pixel
      glm::vec3 direction = camera.Ray(-0.5f + xi * dx, -0.5f + yi * dx);

      // this gives you the position of the camera
      glm::vec3 start = camera.Position();

      glm::vec3 color = glm::vec3(0.0f);
      glm::vec3 throughput = glm::vec3(1.0f);

      // set the associated pixel color based on the ray direction
      // if intersection of rays and sphere, the image buffer will be loaded with white information
      while (glm::length(throughput) >= 0.1f && bounceCount < 10) {


          float closestT = std::numeric_limits<float>::infinity();
          int closestSphere = -1;

          for (int i = 0; i < groupOfSpheres.size(); i++) {

              float t;
              if (hitSphere(groupOfSpheres[i].pos, groupOfSpheres[i].area, direction, start, t)) {
                  if (t < closestT) {
                      closestT = t;
                      closestSphere = i;
                  }
              }
          }
          if (closestSphere == -1) {
              glm::vec3 backgroundColor(
                  std::abs(direction.x),
                  std::abs(direction.y),
                  std::abs(direction.z)
              );

              color = color + throughput * backgroundColor;

              throughput = glm::vec3(0.0f);
              break;
          }

          color = color + throughput * groupOfSpheres[closestSphere].emissive;

          glm::vec3 hitPoint = start + closestT * direction;

          glm::vec3 normal = glm::normalize(hitPoint - groupOfSpheres[closestSphere].pos);

          glm::vec3 newStart = hitPoint + 0.001f * normal;

          float RandomX = RandomNum();
          float RandomY = RandomNum();
          float RandomZ = RandomNum();
          glm::vec3 RandomDirection(RandomX, RandomY, RandomZ);
          RandomDirection = glm::normalize(RandomDirection);

          if (glm::dot(RandomDirection, normal) < 0) {
              RandomDirection = -RandomDirection;
          }
          
          throughput = throughput * groupOfSpheres[closestSphere].diffuse;
          start = newStart;
          direction = RandomDirection;

          bounceCount = bounceCount + 1;
      }

      size_t pixelIndex = yi * s + xi;

      accumulation_buffer[pixelIndex] += color;
      glm::vec3 averageColor =
          accumulation_buffer[pixelIndex] / float(pass_count);

      averageColor = glm::clamp(
          averageColor,
          glm::vec3(0.0f),
          glm::vec3(1.0f)
      );


      image_buffer[yi * s * 3 + xi * 3 + 0] = static_cast<unsigned char>(averageColor[0] * 255);

      image_buffer[yi * s * 3 + xi * 3 + 1] =  static_cast<unsigned char>(averageColor[1] * 255);

      image_buffer[yi * s * 3 + xi * 3 + 2] = static_cast<unsigned char>(averageColor[2] * 255);

    }
  }
}
