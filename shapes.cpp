//Include Necessary Libs
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>

//Define Namespaces
using namespace cv;
using namespace std;

//Predefine Functions
void printHelp();
bool check_4(int shape, vector<Point> poly, int height, int width);

//Global Variables
const string validShapes[] = {"triangle", "square", "rectangle", "rhombus", "parallelogram", "trapezoid", "kite", "quadrilateral", "pentagon", "hexagon"};
const int shapeEdges[] = {3,4,4,4,4,4,4,4,5,6};

int main(int argc, char *argv[]) {
  //Local Variables
  Mat image, gray, canny_output;
  vector<vector<Point> > contours,polys;
  vector<Vec4i> hierarchy;
  int thresh = 50;
  string infile = "";
  string outfile = "";
  int shape = 1;
  Scalar color = Scalar(0,0,255);
  int found = 0;
  
  //Check arguments
  for (int i = 1; i < argc-1; i+=2){
    string arg = argv[i];
    string str = argv[i+1];
    if(arg.compare("-i") == 0){
      if(str.length() > 32){
        cout << "ERROR: Filename is too long" << endl;
        printHelp();
        return -1;
      }
      infile = str;
    }
    else if(arg.compare("-o") == 0){
      if(str.length() > 32){
        cout << "ERROR: Filename is too long" << endl;
        printHelp();
        return -1;
      }
      outfile = str;
    }
    else if(arg.compare("-s") == 0){
      if(str.length() > 24){
        cout << "ERROR: Invalid shape" << endl;
        printHelp();
        return -1;
      }
      transform(str.begin(), str.end(),str.begin(), ::tolower);
      for(int j=0; j<(sizeof(validShapes)/sizeof(*validShapes)); j++){
        if(validShapes[j].compare(str) == 0){
          shape = j;
          break;
        }
        else if(j+1 == (sizeof(validShapes)/sizeof(*validShapes))){
          cout << "ERROR: Invalid shape" << endl;
          printHelp();
          return -1;
        }
      }
    }
    else if(arg.compare("-c") == 0){
      if(str.length() > 6){
        cout << "ERROR: Invalid color" << endl;
        printHelp();
        return -1;
      }
      transform(str.begin(), str.end(),str.begin(), ::tolower);
      int r = 0;
      int g = 0;
      int b = 0;
      for(int j=0; j<str.length(); j++){
        if(str[j] > 'f'){
          cout << "ERROR: Invalid color" << endl;
          printHelp();
          return -1;
        }
      }
      if(str[0] < 'a'){
        r += (str[0]-48)*16;
      }
      else{
        r += (str[0]-87)*16;
      }
      if(str[1] < 'a'){
        r += (str[1]-48);
      }
      else{
        r += (str[1]-87);
      }
      if(str[2] < 'a'){
        g += (str[2]-48)*16;
      }
      else{
        g += (str[2]-87)*16;
      }
      if(str[3] < 'a'){
        g += (str[3]-48);
      }
      else{
        g += (str[3]-87);
      }
      if(str[4] < 'a'){
        b += (str[4]-48)*16;
      }
      else{
        b += (str[4]-87)*16;
      }
      if(str[5] < 'a'){
        b += (str[5]-48);
      }
      else{
        b += (str[5]-87);
      }
      color = Scalar(b,g,r);
    }
  }
  
  //Echo back parameters for debugging
  cout << "Input File: " << infile << endl;
  cout << "Output File: " << outfile << endl;
  cout << "Shape: " << validShapes[shape] <<endl;
  cout << "Color: " << color << endl;
  
  //Load in image
  image = imread(infile);
  if(! image.data ){ 
    cout <<  "ERROR: Could not open or find the image" << endl ;
    printHelp();
    return -1;
  }
  
  //Convert to grayscale for processing
  cvtColor(image,gray,CV_BGR2GRAY);
  
  //Get all contours
  Canny( gray, canny_output, thresh, thresh*2, 3 );
  findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  cout << "Found " << contours.size() << " contours" << endl;
  for( int i = 0; i< contours.size(); i++ ){
    vector<Point> poly;
    approxPolyDP(contours[i],poly,0.05*arcLength(contours[i],true),true);
    polys.push_back(poly);
    cout << "edges: " << poly.size() << endl;
    if(poly.size() == shapeEdges[shape]){
      if(poly.size() == 4){
        if(shape == 7 || check_4(shape,poly,image.rows,image.cols)){
          drawContours( image, contours, i, color, 2, 8, 0, 0, Point() );
          found++;
        }
      }
      else{
        drawContours( image, contours, i, color, 2, 8, 0, 0, Point() );
      }
    }
  }
  cout << "Found " << found << " " << validShapes[shape] << "s" << endl;
  imwrite(outfile,image);
  return 0;
} //main

void printHelp(){
  cout << "Usage:" <<endl;
  cout << "./shapes [options]     Outputs image with specified shape highlighted" << endl;
  cout << "Options:" <<endl;
  cout << "-i <filename>          Specify input filename (32 character max)" << endl;
  cout << "-o <filename>          Specify output filename (32 character max)" << endl;
  cout << "-c <string>            Specify color for detected shape outline in hex (no hash sign), \'FF0000\' is default" << endl;
  cout << "-s <string>            Specify shape to look for, \'square\' is default" << endl;
  cout << "                       Valid Shapes: triangle, square, rectangle, rhombus, parallelogram, trapezoid, kite, quadrilateral pentagon, hexagon" << endl;
  return;
} //printHelp

bool check_4(int shape, vector<Point> poly, int height, int width){
  for(int j=0; j<poly.size(); j++){
    cout << poly[j].x << " " << poly[j].y << endl;
  }
  float slope0_1 = 1000000.0;
  float slope2_3 = 1000000.0;
  float slope0_3 = 1000000.0;
  float slope1_2 = 1000000.0;
  float slope_error = 0.03; //3% error
  float ang_error = 0.02; //2% error
  float dist_error = 0.02; //2% error
  if((poly[0].x-poly[1].x) != 0 ){
    slope0_1 = (float)(poly[0].y-poly[1].y)/(float)(poly[0].x-poly[1].x);
    if (abs(slope0_1) >= 29){
      slope0_1 = 1000000.0;
    }
  }
  if((poly[2].x-poly[3].x) != 0){
    slope2_3 = (float)(poly[2].y-poly[3].y)/(float)(poly[2].x-poly[3].x);
    if (abs(slope2_3) >= 29){
      slope2_3 = 1000000.0;
    }
  }
  if((poly[0].x-poly[3].x) != 0){
    slope0_3 = (float)(poly[0].y-poly[3].y)/(float)(poly[0].x-poly[3].x);
    if (abs(slope0_3) >= 29){
      slope0_3 = 1000000.0;
    }
  }
  if((poly[1].x-poly[2].x) != 0){
    slope1_2 = (float)(poly[1].y-poly[2].y)/(float)(poly[1].x-poly[2].x);
    if (abs(slope1_2) >= 29){
      slope1_2 = 1000000.0;
    }
  }
  slope0_1 = round(slope0_1*10)/10.0;
  slope2_3 = round(slope2_3*10)/10.0;
  slope0_3 = round(slope0_3*10)/10.0;
  slope1_2 = round(slope1_2*10)/10.0;
  float ang0 = atan(abs(slope0_1-slope0_3)/(1+slope0_1*slope0_3))*(180/M_PI);
  float ang2 = atan(abs(slope1_2-slope2_3)/(1+slope1_2*slope2_3))*(180/M_PI);
  float dist0_1 = sqrt(pow(poly[0].x-poly[1].x,2)+pow(poly[0].y-poly[1].y,2));
  float dist2_3 = sqrt(pow(poly[2].x-poly[3].x,2)+pow(poly[2].y-poly[3].y,2));
  float dist0_3 = sqrt(pow(poly[0].x-poly[3].x,2)+pow(poly[0].y-poly[3].y,2));
  float dist1_2 = sqrt(pow(poly[1].x-poly[2].x,2)+pow(poly[1].y-poly[2].y,2));
  cout << "slope0_1: " << slope0_1 << endl;
  cout << "slope2_3: " << slope2_3 << endl;
  cout << "slope0_3: " << slope0_3 << endl;
  cout << "slope1_2: " << slope1_2 << endl;
  cout << "dist0_1: " << dist0_1 << endl;
  cout << "dist2_3: " << dist2_3 << endl;
  cout << "dist0_3: " << dist0_3 << endl;
  cout << "dist1_2: " << dist1_2 << endl;
  cout << "ang0: " << ang0 << endl;
  cout << "ang2: " << ang2 << endl;
  if( abs(abs(slope0_1-slope2_3)/((slope0_1+slope2_3+0.000001)/2.0)) < slope_error){ //compare opposite slope pair 1
    cout << "true 1" << endl;
    if( abs(abs(slope0_3-slope1_2)/((slope0_3+slope1_2+0.000001)/2.0) )< slope_error){ //compare oppposite slope pair 2
      cout << "true 2" << endl;
      if(abs(abs(ang0-90)/((ang0+90)/2.0)) < ang_error ) { //compare angle between 1 pair of lines to 90
        cout << "true 3" << endl;
        if(abs(abs(ang2-90)/((ang2+90)/2.0)) < ang_error ){ //compare angle between other pair of lines to 90
          cout << "true 4" << endl;
          if(abs(abs(dist0_1-dist0_3)/((dist0_1+dist0_3)/2.0)) < dist_error){
            cout << "true 5" << endl;
            if(abs(abs(dist1_2-dist2_3)/((dist1_2+dist2_3)/2.0))< dist_error){
              cout << "true 6" << endl;
              cout << "square" << endl;
              if(shape == 1)
                return true;
              else
                return false;
            }
          }
          cout << "rectangle" << endl;
          if(shape == 2)
            return true;
          else
            return false;
            
        }
      }
      if(abs(abs(dist0_1-dist0_3)/((dist0_1+dist0_3)/2.0)) < dist_error){
        cout << "true 7" << endl;
        if(abs(abs(dist1_2-dist2_3)/((dist1_2+dist2_3)/2.0)) < dist_error){
          cout << "rhombus" << endl;
          if(shape == 3)
            return true;
          else
            return false;
        }
      }
      cout << "parallelogram" << endl;
      if(shape == 4)
        return true;
      else
        return false;
    }
  }
  if(abs(abs(dist0_1-dist0_3)/((dist0_1+dist0_3)/2.0)) < dist_error){
    cout << "true 8" << endl;
    if(abs(abs(dist1_2-dist2_3)/((dist1_2+dist2_3)/2.0)) < dist_error){
      cout << "true 9" << endl;
      if(abs(abs(dist0_1-dist0_3)/((dist0_1+dist0_3)/2.0)) < dist_error){
        cout << "true 7" << endl;
        if(abs(abs(dist1_2-dist2_3)/((dist1_2+dist2_3)/2.0)) < dist_error){
          cout << "rhombus" << endl;
          if(shape == 3)
            return true;
          else
            return false;
        }
      }
      cout << "kite" << endl;
      if(shape == 6)
        return true;
      else
        return false;
    }
  }
  else if(abs(abs(dist0_1-dist1_2)/((dist0_1+dist1_2)/2.0)) < dist_error){
    cout << "true 10" << endl;
    if(abs(abs(dist0_3-dist2_3)/((dist0_3+dist2_3)/2.0)) < dist_error){
      cout << "true 11" << endl;
      if(abs(abs(dist0_1-dist0_3)/((dist0_1+dist0_3)/2.0)) < dist_error){
        cout << "true 7" << endl;
        if(abs(abs(dist1_2-dist2_3)/((dist1_2+dist2_3)/2.0)) < dist_error){
          cout << "rhombus" << endl;
          if(shape == 3)
            return true;
          else
            return false;
        }
      }
      cout << "kite" << endl;
      if(shape == 6)
        return true;
      else
        return false;
    }
  }
  if( abs(abs(slope0_1-slope2_3)/((slope0_1+slope2_3+0.000001)/2.0)) < slope_error){ //compare opposite slope pair 1
    cout << "trapezoid" << endl;
    if(shape == 5)
      return true;
  }
  else if( abs(abs(slope0_3-slope1_2)/((slope0_3+slope1_2+0.000001)/2.0)) < slope_error){ //compare oppposite slope pair 2
    cout << "trapezoid" << endl;
    if(shape == 5)
      return true;
  }
  return false;
}
