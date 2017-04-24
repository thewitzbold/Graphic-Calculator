// Home

#include <OpenGL/gl.h>    //MAC
#include <OpenGL/glu.h>   //
#include <GLUT/glut.h>    //

//#include <GL/glut.h>    //CSIF

#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>
#include <cstdlib>
using namespace std;

//in terminal
// g++ p2.cpp -framework OpenGL -framework GLUT

class RGB
{
public:
  float r,g,b;
  RGB(float r, float g, float b):r(r),g(g),b(b){}
};

int width; //window
int height; //window
float *PixelBuffer;
int low_x, low_y;
int high_x, high_y;

float x_min;
float x_max;
float y_min;
float y_max;

RGB white(1,1,1),
    black(0,0,0),
    gray(0.9,0.9,0.9);

RGB colors[6] = {   RGB(1,0,0),
                    RGB(0,1,0),
                    RGB(0,0,1),
                    RGB(1,1,0),
                    RGB(0,1,1),
                    RGB(1,0,1)
                                };

void display();
void set_pixel(int, int, RGB);
void menu();
float fun(string, float);
void Bresenham(int, int, int, int, RGB);
void legend();

int main(int argc, char *argv[])
{
  width = 500;
  height = 500;

  high_x = (width/2);
  high_y = (height/2);

  low_x = (-1)*((width-1)/2);
  low_y = (-1)*((height-1)/2);

  //allocate new pixel buffer, need initialization!!
  PixelBuffer = new float[width * height * 3];

  x_min = -10;
  x_max = 10;
  y_min = -10;
  y_max = 10;

  legend();

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE);

  //set window size to 200*200
  glutInitWindowSize(width, height);

  //set window position
  glutInitWindowPosition(1, 1);

  //create and set main window title
  //int MainWindow = glutCreateWindow("Hello Graphics!!");
  glutCreateWindow("Graphing Calculator - The Witzbold");
  glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL

  //sets display function
  glutDisplayFunc(display);
  glutIdleFunc(menu);

  glutMainLoop(); //main display loop, will display until terminate

  return 0;
}

void legend()
{
  for(int i=0; i<width * height * 3; i++)
    PixelBuffer[i] = 1.0;

  float a =(width - 1)/(x_max - x_min);
  float b = low_x - (x_min * a);
  float x;
  for(int i=x_min; i <= x_max; i++)
  {
    x = i * a + b;
    for(int j=low_y; j <= high_y; j++)
    {
      if(i)
        set_pixel(x, j, gray);
      else
        set_pixel(0, j, black);
    }
  }

  a =(height - 1)/(y_max - y_min);
  b = low_y - (y_min * a);
  float y;

  for(int j=y_min; j <= y_max; j++)
  {
    set_pixel(x, 0, black);
    y = j * a + b;
    for(int i=low_x; i <= high_x; i++)
    {
      if(j)
      {
        if(i)
          set_pixel(i, y, gray);
      }
      else
        set_pixel(i,0,black);
    }
  }
}

void menu()
{
  static int color = -1;
  color++;

  string equation;
  cout << "Enter your function: f(x) = ";
  getline(cin, equation);

  float y, y_window;
  float x_delta = (x_max-x_min)/(width-1);
  float y_delta = (y_max-y_min)/(width-1);
  int tempy, tempy_window;

  tempy = 90000;

  for(int x=low_x, d=0 ; x <= high_x; x++, d++)
  {
    y = fun(equation, x_min + d*x_delta);

    y_window = round((y - y_min)/y_delta + low_y);

    if(y >= y_min && y <= y_max)
    {
      if(x==low_x)
        set_pixel(x, y_window, colors[color%5]);
      else
        Bresenham(x-1,tempy_window,x,y_window,colors[color%5]);
    }
    else
    {
      if(tempy >= y_min && tempy <= y_max)
        Bresenham(x-1,tempy_window,x,y_window,colors[color%5]);
    }

    tempy_window = y_window;
    tempy = y;
  }

  glutPostRedisplay();
}

float fun(string eq, float x)
{
  //Check if there is a + -
  int i;
  string strA, strB;
  for(i=0; i < eq.length() ; i++)
  {
    if(eq[i] == '+' || eq[i] == '-' )
    {
      //StringA = 0 to i-1
      strA = eq.substr(0,i);
      //StringB = i+1 til end
      strB = eq.substr(i+1);
      break;
    }
  }

  if(i == eq.length() )
    strA = eq;

  strA.erase( remove( strA.begin(), strA.end(), ' ' ), strA.end() );


  float value;
  float power;
  float constant;
  int j;
  for(j=0; j < strA.length() ; j++)
  {
    if(strA[j] == 'x' || strA[j] == 'X'  )
    {
      if(j == 0)
        constant = 1;
      else
        constant = atoi( ( strA.substr(0,j) ).c_str() );

      if(strA[j+1] == '^')
        power = atoi( (strA.substr(j+2)).c_str() );
      else
        power = 1;

      value = constant * pow(x,power);
      break;
    }
  }

  if(j == strA.length() ) //If no x found, then its a constant
    value = atoi(strA.c_str());

  if(i == eq.length() )
    return value;
  else
  {
    if(eq[i] == '+')
      return value + fun(strB, x);
    else
      return value - fun(strB, x);
  }
}

void display()
{
  //Misc.
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  //draws pixel on screen, width and height must match pixel buffer dimension
  glDrawPixels(width, height, GL_RGB, GL_FLOAT, PixelBuffer);

  //window refresh
  glFlush();
}

void set_pixel(int x, int y, RGB color)
{
  int center_width = (width-1)/2;
  int center_height =(height-1)/2;

  int point = 3 * (center_width + x + width * (center_height + y));

  PixelBuffer[point] = color.r;
  PixelBuffer[point+1] = color.g;
  PixelBuffer[point+2] = color.b;
}

void Bresenham(int xi, int yi, int xf, int yf, RGB color)
{
  //it is always x++ or y++, in case of x-- or y--, the final and initial swap.
  int x0, y0;

  if(xi == xf && yi == yf)
  {
    set_pixel(xi,yf, color);
    return;
  }

  int dx = abs(xf-xi);
  int dy = abs(yf-yi);

  if(dx >= dy) //x++ or x--
  {
    if (xi < xf)
    {
      x0 = xi;
      y0 = yi;
    }
    else
    {
      x0 = xf;
      y0 = yf;
      xf = xi;
      yf = yi;
      xi = x0;
      yi = y0;
    }

      if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);

    int P = 2*(dy) - dx;

    if(P >= 0)
    {
      if(yf > yi)
        y0++;
      else
        y0--;
    }

    x0++;

    if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);

    while( y0 != yf || x0 != xf)
    {
      if(P<0)
        P = P + 2*dy;
      else
        P = P + 2*dy - 2 * dx;

      if(P>=0){
        if(yf > yi)
          y0++;
        else //if (xf < xi)
          y0--;
      }

      x0++;

      if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);
    }
  }
  else //y++ or y--
  {
    if (yi < yf)
    {
      x0 = xi;
      y0 = yi;
    }
    else
    {
      x0 = xf;
      y0 = yf;
      xf = xi;
      yf = yi;
      xi = x0;
      yi = y0;
    }

    if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);

    int P = 2*(dx) - dy;

    if(P >= 0)
    {
      if(xf > xi)
        x0++;
      else //if (xf < xi)
        x0--;
    }

    y0++;

    if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);

    while( y0 != yf || x0 != xf)
    {
      if(P<0)
        P = P + 2*dx;
      else
        P = P + 2*dx - 2 * dy;

      if(P>=0){
        if(xf > xi)
          x0++;
        else //if (xf < xi)
          x0--;
      }

      y0++;

      if(x0 >= low_x && x0 <= high_x)
        if(y0 >= low_y && y0 <= high_y)
          set_pixel(x0,y0, color);
    }
  }
}
