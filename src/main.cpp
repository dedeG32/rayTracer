#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "shaders.h"


using namespace std;

const int IMAGE_WIDTH = 512;
const int IMAGE_HEIGHT = 512;

class Vec3 {
    public:
    double x, y, z;

    Vec3(double x=0, double y=0, double z=0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3 operator + (const Vec3& right) const {
        return Vec3(this->x + right.x, this->y + right.y, this->z + right.z);
    }

    Vec3 operator - (const Vec3& right) const {
        return Vec3(this->x - right.x, this->y - right.y, this->z - right.z);
    }

    Vec3 operator * (const double right) const {
        return Vec3(this->x * right, this->y * right, this->z * right);
    }

    Vec3 operator * (const Vec3& right ) const {
        return Vec3(this->x * right.x, this->y * right.y, this->z * right.z);
    }

    Vec3 operator / (const double right) const {
        return Vec3(this->x / right, this->y / right, this->z / right);
        
    }

    double dot(const Vec3& right) const { 
        return this->x * right.x + this->y * right.y + this->z * right.z;
    }

    double magnitude() const { 
        return sqrt(this->x*this->x + this->y*this->y + this->z*this->z); 
    }

    Vec3 normalize() const { 
        double mag = this->magnitude();
        if (mag == 0) return Vec3(0, 0, 0);
        return Vec3(this->x/mag, this->y/mag, this->z/mag);
    }

    friend istream& operator >> (istream& in, Vec3& vec);
};

istream& operator >> (istream& in, Vec3& vec) { 
    
    in >> vec.x >> vec.y >> vec.z;
    return in; 
}

class Light {
public:
    Vec3 position;
    Vec3 color;
    friend istream& operator >> (istream& in, Light& light);
};

istream& operator >> (istream& in, Light& light) { 
    in >> light.position;
    in >> light.color;
    cout<< "got light"<< endl;
    return in; 
}

class Sphere {
public:
    Vec3 center;
    double radius;
    Vec3 color;
    friend istream& operator >> (istream& in, Sphere& sphere);
};

istream& operator >> (istream& in, Sphere& sphere) { 
    //string str;
    //in >> str;
    //if (str != "sphere") {
    //    cerr << "Error: malformed scene file. "
     //        << "Expected Sphere." 
      //       << endl;
       // cout<<str<<endl;
        //return in; //exit(1);
    //}
    in >> sphere.center;
    in >> sphere.radius;
    in >> sphere.color;
    cout<< "got Sphere"<< endl;
    return in; 

    //in >> sphere.center;
    //in >> sphere.radius;
    //in >> sphere.color;
    //return in; 
}

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray(Vec3 origin, Vec3 direction)
    {
        this->origin = origin ;
        this->direction = direction;
    }

    double intersectsDistance(const Sphere& sphere) { 
        return 0;
    }

    //bool intersesct(const Sphere& sphere)
    //{
    //    return 
    //}
};


// utility methodes
double restrictValue(double value, double low, double high);
int getIntersectedSphere(const vector<Sphere>& spheres, const Ray& ray);
Vec3 LambertianShading(const Sphere& sphere,const Light& light,const Vec3& point);
//bool no_shadow(const vector<Sphere>& spheres, const Light& light, const Vec3& point);

//openGL fctions
GLFWwindow* creatWindow();
void shadersSwtting(GLuint& shaderProgram);
void setVAO_VBO(GLuint& VAO, GLuint& VBO);
void terminate_window(GLFWwindow* window, GLuint& VAO, GLuint& VBO, GLuint& shaderProgram);

int main(int argc, char **argv){
    cout<< "start program"<< endl;
    cout<< "start"<< endl;
    string sceneFilename(argv[1]);
    string ppmFilename(argv[1]);
    ppmFilename.replace(ppmFilename.length()-3, 3, "ppm");

    ifstream in(sceneFilename);
    if(!in) {
        cerr << "Error: unable to open scene file." << endl;
        return 1;
    }
    ofstream out(ppmFilename);
    cout<< "created file "<< ppmFilename<< endl;

    string type;
    Light light;
    vector<Sphere> spheres;

    while(in>>type){
        if(type=="light") 
            in>>light;
        else if(type=="sphere"){
            Sphere inSphere;
            in>>inSphere;
            spheres.push_back(inSphere);
        }
    }

    
    out << "P3" << endl;
    out << IMAGE_WIDTH << " " << IMAGE_HEIGHT << endl;
    out << "255" << endl;


    // start OpenGL init    ///////////////////////////////////////////////////////////////
    cout<< "start OpenGL"<< endl;
    GLFWwindow* window = creatWindow();
    GLuint shaderProgram = glCreateProgram();
    shadersSwtting(shaderProgram);
    float vertices[IMAGE_WIDTH *IMAGE_HEIGHT * 6];  //*6 is because each point is represented by X Y Zindex R G B 
    glPointSize(1.0f);
    cout<< "end OpenGL"<< endl;
    //end openGL init    ///////////////////////////////////////////////////////////
    for(int j=IMAGE_HEIGHT-1;j>=0;j--){
        for(int i=0;i<IMAGE_WIDTH;i++){
            vector<Vec3> all_colors;
            double offsets[5][2] = {{0.25, 0.25}, {0.75, 0.25}, {0.5, 0.5}, {0.25, 0.75}, {0.75, 0.75}};
            for(int nb_ray=0; nb_ray<5; nb_ray++)   // ray is the number of rays to be shot at each pixel
                {
                Vec3 color(0,0,0);
                double x = (i + offsets[nb_ray][0] - IMAGE_WIDTH / 2.0) / (IMAGE_WIDTH / 2 );
                double y = (j + offsets[nb_ray][1]- IMAGE_HEIGHT / 2.0) / (IMAGE_HEIGHT / 2 );
                Ray ray{Vec3(x,y,0), Vec3(0,0,-1)};
                
                int index = getIntersectedSphere(spheres, ray);
                
                if(index>-1){
                    double a = ray.direction.dot(ray.direction); // refresh
                    double b = (ray.origin - spheres[index].center).dot(ray.direction)*2;
                    double c = (ray.origin - spheres[index].center).dot((ray.origin - spheres[index].center))-spheres[index].radius*spheres[index].radius;
                    double delta=b*b-4*a*c;
                    double t;
                    double t1=(-b - sqrt(delta))/(2*a);
                    double t2=(-b + sqrt(delta))/(2*a);
                    if (t1<t2 && t1>=0)
                        t = t1;
                    else 
                        t = t2;

                    Vec3 point = ray.origin + ray.direction*t;
                    //if (no_shadow(spheres, light, point)) 
                    if(getIntersectedSphere(spheres, Ray(point,(light.position - point).normalize()*0.000001)) < 0) // same as no-shadow fction
                        color = LambertianShading(spheres[index], light, point);
                }

                //Vec3 pixelColor = spheres[index].color * color;
                all_colors.push_back(spheres[index].color * color);
            }
            Vec3 final_color = Vec3(0,0,0);
            for(int i2=0;i2<all_colors.size();i2++) final_color = final_color + all_colors[i2];

            final_color = final_color / all_colors.size();
            // start OpenGL
            //vertices[0] = (i - IMAGE_WIDTH / 2.0) / (IMAGE_WIDTH / 2 ); //X
            //vertices[j*IMAGE_HEIGHT*6 + i+1] = (j - IMAGE_HEIGHT / 2.0) / (IMAGE_HEIGHT / 2 );//Y
            //vertices[j*IMAGE_HEIGHT*6 + i+2] = 0.0f; //Zindex
            //vertices[j*IMAGE_HEIGHT*6 + i+3] = restrictValue(final_color.x, 0, 1); //R
            //vertices[j*IMAGE_HEIGHT*6 + i+4] = restrictValue(final_color.y, 0, 1); //G
            //vertices[j*IMAGE_HEIGHT*6 + i+5] = restrictValue(final_color.z, 0, 1); //B 
            // end OpenGL

            int red = 255*restrictValue(final_color.x, 0, 1);
            int green = 255*restrictValue(final_color.y, 0, 1);
            int blue = 255*restrictValue(final_color.z, 0, 1);
            out << red << " " << green << " " << blue << endl;
        }
        
    }
    //for(int i=0; i < IMAGE_WIDTH *IMAGE_HEIGHT * 6; i++ )
    //{
    //    if(i%6 == 0) cout<< endl;
     //   cout<< vertices[i];
    //}

    ///OpenGL/////////////////////////////////////
    GLuint VAO, VBO;
    setVAO_VBO(VAO, VBO);
    /*
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		glUseProgram(shaderProgram);
		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		// Draw the triangle using the GL_TRIANGLES primitive
		glDrawArrays(GL_POINTS, 0, 1);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}*/
    ///////////////////////////////////
    cout<<"program ended successfuly"<< endl;
    cout<< "end program"<< endl;
    terminate_window(window,VAO,VBO, shaderProgram);
    return 0;
}







////////////////////////////////////////////////////////////////////////////////////
double restrictValue(double value, double low, double high) {
    if(value < low)
        return low;
    if(value > high)
        return high;
    return value;
}



int getIntersectedSphere(const vector<Sphere>& spheres, const Ray& ray) {
    int index=-1; 
    double distance=-1;

    for(int i=0;i<spheres.size();i++){
        double a = ray.direction.dot(ray.direction); // refresh
        double b = (ray.origin - spheres[i].center).dot(ray.direction)*2;
        double c = (ray.origin - spheres[i].center).dot((ray.origin - spheres[i].center))-spheres[i].radius*spheres[i].radius;
        double delta = b*b - 4*a*c;
        if(delta>=0){
            double t1=(-b - sqrt(delta))/(2*a);
            double t2=(-b + sqrt(delta))/(2*a);
            double t=(t1<=t2)?t1:t2;
            if( t>=0 && distance>=0 && t<distance)
            {
                distance=t; 
                index=i;
            }
            else if (t>=0 && distance<0)  // assigns the very first value of distance
            {
                distance = t;
                index = i;
            }
        }
    }
    return index;
}

/*
bool no_shadow(const vector<Sphere>& spheres, const Light& light, const Vec3& point){
    Ray ray = Ray(point,(light.position - point).normalize()*0.000001);
    int index = getIntersectedSphere(spheres, ray);
    if (index>-1) return false;
    return true;
}
    */

Vec3 LambertianShading(const Sphere& sphere,const Light& light,const Vec3& point){
    Vec3 surfaceNormal = (point - sphere.center).normalize();
    Vec3 lightNormal = (light.position - point).normalize();
    
    return light.color * max(0.0, surfaceNormal.dot(lightNormal));
}


GLFWwindow* creatWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "Ray tracer", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    return window;
}

void shadersSwtting(GLuint& shaderProgram)
{
    // Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(shaderProgram);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void setVAO_VBO(GLuint& VAO, GLuint& VBO){
    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

}


void terminate_window(GLFWwindow* window, GLuint& VAO, GLuint& VBO, GLuint& shaderProgram)
{
    glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}