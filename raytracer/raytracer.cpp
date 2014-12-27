#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <string.h>
#include <cmath>

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

char *filename=0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480

//the field of view of the camera in radians
#define fov 1.0471975512

unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

typedef struct _Triangle
{
  struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
} Sphere;

typedef struct _Light
{
  double position[3];
  double color[3];
} Light;

typedef struct _Ray
{
  double position[3];
  double direction[3];
} Ray;

typedef struct _Intersection
{
  double time;
  double position[3];
} Intersection;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

double *cast_ray(unsigned int x, unsigned int y);
double check_spheres(Ray);
double check_triangles(Ray);
bool lookForShadow(double *);

//MODIFY THIS FUNCTION
void draw_scene()
{
  unsigned int x,y;
  //simple output
  for(x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(y=0;y < HEIGHT;y++)
    {
      cast_ray(x,y);
      //plot_pixel(x,y,x%256,y%256,(x+y)%256);
    }
    glEnd();
    glFlush();
  }
  printf("Done!\n"); fflush(stdout);
}

double *cast_ray(unsigned int x, unsigned int y)
{
  //printf("\ncast_ray(%i,%i)\n",x,y);
  double pixDirectionFactor = std::abs(2 * std::tan(fov/2.0) / HEIGHT);
  double rayLength;
  
  Ray primary_ray;

  primary_ray.position[0] = 0.0;
  primary_ray.position[1] = 0.0;
  primary_ray.position[2] = 0.0;
  
  primary_ray.direction[0] = ((double)x - (WIDTH/2.0)) * pixDirectionFactor;
  primary_ray.direction[1] = ((double)y - (HEIGHT/2.0)) * pixDirectionFactor;
  primary_ray.direction[2] = -1.0;

  rayLength = (pow(primary_ray.direction[0], 2) + pow(primary_ray.direction[1], 2) + pow(primary_ray.direction[2], 2));
  
  rayLength = sqrt(rayLength);

  primary_ray.direction[0] /= rayLength;
  primary_ray.direction[1] /= rayLength;
  primary_ray.direction[2] /= rayLength;
  
  if(check_spheres(primary_ray) > 0.0)
  {
    plot_pixel(x,y,255,255,255);
  }
  
  if(check_triangles(primary_ray) > 0.0)
  {
    plot_pixel(x,y,255,255,255);
  }
  
  return primary_ray.position;
}

double check_spheres(Ray ray)
{
  double solution = -1.0;
  
  for(int i = 0; i < num_spheres; i++)
  {
    double a = (pow(ray.direction[0], 2) + pow(ray.direction[1], 2) + pow(ray.direction[2], 2));
    
    double b = 2.0 * ((ray.direction[0] * (ray.position[0] - spheres[i].position[0])) +
                      (ray.direction[1] * (ray.position[1] - spheres[i].position[1])) +
                      (ray.direction[2] * (ray.position[2] - spheres[i].position[2])));
    
    double c = pow((ray.position[0] - spheres[i].position[0]),2) +
              pow((ray.position[1] - spheres[i].position[1]),2) +
              pow((ray.position[2] - spheres[i].position[2]),2) -
              pow(spheres[i].radius,2);
  
    double discriminant = pow(b,2) - (4 * a * c);
    
    if(discriminant >= 0)
    {
      double zero1 = ((-1.0 * b) + sqrt(discriminant)) / (2.0 * a);
      double zero2 = ((-1.0 * b) - sqrt(discriminant)) / (2.0 * a);
      
      if(zero1 < zero2)
      {
        solution = zero1;
      }
      else
      {
        solution = zero2;
      }
      
      return solution;
    }
    else if(discriminant == 0.0)
    {
      solution = (-1.0 * b) / (2.0 * a);

      return solution;
    }
    else
    {
     //return invalid value
    return -1.0;
    }
  }
}

double check_triangles(Ray ray)
{
  double planeNormal[3];
  
  double u[3];
  double v[3];
  double w[3];
  
  double intersectionPoint[3];
  
  for(int i = 0; i< num_triangles; i++)
  {
    //calculate edges of triangle
    //edge 1
    u[0] = triangles[i].v[1].position[0] - triangles[i].v[0].position[0];
    u[1] = triangles[i].v[1].position[1] - triangles[i].v[0].position[1];
    u[2] = triangles[i].v[1].position[2] - triangles[i].v[0].position[2];
    //edge 2
    v[0] = triangles[i].v[2].position[0] - triangles[i].v[0].position[0];
    v[1] = triangles[i].v[2].position[1] - triangles[i].v[0].position[1];
    v[2] = triangles[i].v[2].position[2] - triangles[i].v[0].position[2];
    
    //find normal vector
    planeNormal[0] = (u[1] * v[2]) - (u[2] * v[1]);
    planeNormal[1] = (u[2] * v[0]) - (u[0] * v[2]);
    planeNormal[2] = (u[0] * v[1]) - (u[1] * v[0]);
    
    //normalize it
    double vectorLength = (pow(planeNormal[0],2) + pow(planeNormal[1],2) + pow(planeNormal[2],2));
    vectorLength = sqrt(vectorLength);
    
    double intersectionDenominator =((ray.direction[0] * planeNormal[0]) + (ray.direction[1] * planeNormal[1]) + (ray.direction[2] * planeNormal[2]));
    
    if(intersectionDenominator < -0.0005 || intersectionDenominator > 0.0005)
    {
      double intersectionTime = ((triangles[i].v[0].position[0] - ray.position[0]) * planeNormal[0]) + ((triangles[i].v[0].position[1] - ray.position[1]) * planeNormal[1]) + ((triangles[i].v[0].position[2] - ray.position[2]) * planeNormal[2]);
      intersectionTime /= intersectionDenominator;
      
      intersectionPoint[0] = ray.position[0] + (intersectionTime * ray.direction[0]);
      intersectionPoint[1] = ray.position[1] + (intersectionTime * ray.direction[1]);
      intersectionPoint[2] = ray.position[2] + (intersectionTime * ray.direction[2]);
      
      //now check if it's in the triangle
      w[0] = intersectionPoint[0] - triangles[i].v[0].position[0];
      w[1] = intersectionPoint[1] - triangles[i].v[0].position[1];
      w[2] = intersectionPoint[2] - triangles[i].v[0].position[2];
      
      double uv = (u[0] * v[0]) + (u[1] * v[1]) + (u[2] * v[2]);
      double uself = pow(u[0],2) + pow(u[1],2) + pow(u[2],2);
      double vself = pow(v[0],2) + pow(v[1],2) + pow(v[2],2);
      double uw = (u[0] * w[0]) + (u[1] * w[1]) + (u[2] * w[2]);
      double vw = (v[0] * w[0]) + (v[1] * w[1]) + (v[2] * w[2]);
      
      double s = ((uv * vw) - (vself * uw)) / (pow(uv,2) - (uself * vself));
      double t = ((uv * uw) - (uself * vw)) / (pow(uv,2) - (uself * vself));
      
      if(s > 0.0005 && t > 0.0005 && (s + t) <= 1.000)
      {
        return 1.0;
      }
    }
      
  }
  return -1.0;
}

bool lookForShadow(double *intersectionPoint)
{
  bool hitObject = false;
  
  for(int i = 0; i < num_lights; i ++)
  {
    Ray ray;
    
    ray.position[0] = intersectionPoint[0];
    ray.position[1] = intersectionPoint[1];
    ray.position[2] = intersectionPoint[2];
    
    ray.direction[0] = lights[i].position[0] - intersectionPoint[0];
    ray.direction[1] = lights[i].position[1] - intersectionPoint[1];
    ray.direction[2] = lights[i].position[2] - intersectionPoint[2];
    
    if(check_spheres(ray) >= 0 || check_triangles(ray) >= 0)
    {
      hitObject = true;
    }
  }
}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  buffer[HEIGHT-y-1][x][0]=r;
  buffer[HEIGHT-y-1][x][1]=g;
  buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
      plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
  Pic *in = NULL;

  in = pic_alloc(640, 480, 3, NULL);
  printf("Saving JPEG file: %s\n", filename);

  memcpy(in->pix,buffer,3*WIDTH*HEIGHT);
  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);      

}

void parse_check(char *expected,char *found)
{
  if(strcasecmp(expected,found))
    {
      char error[100];
      printf("Expected '%s ' found '%s '\n",expected,found);
      printf("Parse error, abnormal abortion\n");
      exit(0);
    }

}

void parse_doubles(FILE*file, char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE *file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  int i;
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i",&number_of_objects);

  printf("number of objects: %i\n",number_of_objects);
  char str[200];

  parse_doubles(file,"amb:",ambient_light);

  for(i=0;i < number_of_objects;i++)
    {
      fscanf(file,"%s\n",type);
      printf("%s\n",type);
      if(strcasecmp(type,"triangle")==0)
	{

	  printf("found triangle\n");
	  int j;

	  for(j=0;j < 3;j++)
	    {
	      parse_doubles(file,"pos:",t.v[j].position);
	      parse_doubles(file,"nor:",t.v[j].normal);
	      parse_doubles(file,"dif:",t.v[j].color_diffuse);
	      parse_doubles(file,"spe:",t.v[j].color_specular);
	      parse_shi(file,&t.v[j].shininess);
	    }

	  if(num_triangles == MAX_TRIANGLES)
	    {
	      printf("too many triangles, you should increase MAX_TRIANGLES!\n");
	      exit(0);
	    }
	  triangles[num_triangles++] = t;
	}
      else if(strcasecmp(type,"sphere")==0)
	{
	  printf("found sphere\n");

	  parse_doubles(file,"pos:",s.position);
	  parse_rad(file,&s.radius);
	  parse_doubles(file,"dif:",s.color_diffuse);
	  parse_doubles(file,"spe:",s.color_specular);
	  parse_shi(file,&s.shininess);

	  if(num_spheres == MAX_SPHERES)
	    {
	      printf("too many spheres, you should increase MAX_SPHERES!\n");
	      exit(0);
	    }
	  spheres[num_spheres++] = s;
	}
      else if(strcasecmp(type,"light")==0)
	{
	  printf("found light\n");
	  parse_doubles(file,"pos:",l.position);
	  parse_doubles(file,"col:",l.color);

	  if(num_lights == MAX_LIGHTS)
	    {
	      printf("too many lights, you should increase MAX_LIGHTS!\n");
	      exit(0);
	    }
	  lights[num_lights++] = l;
	}
      else
	{
	  printf("unknown type in scene description:\n%s\n",type);
	  exit(0);
	}
    }
  return 0;
}

void display()
{

}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
      draw_scene();
      if(mode == MODE_JPEG)
	save_jpg();
    }
  once=1;
}

int main (int argc, char ** argv)
{
  if (argc<2 || argc > 3)
  {  
    printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
    {
      mode = MODE_JPEG;
      filename = argv[2];
    }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
}
