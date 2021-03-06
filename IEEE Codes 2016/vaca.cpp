#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
Mat cap, frame;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
Point pe_vaca_possivel;

Size size(640, 480);


void thresh_callback(int, void* );

int main( int argc, char** argv )
{

  int key =   8;
  VideoCapture video;
  double width, height;

  // video.open("/home/barbosa/Documents/arena.mp4");
  video.open(1);

  if (!video.isOpened())
    return -1;
  namedWindow(  "Banana" , CV_WINDOW_AUTOSIZE );
  createTrackbar( " Threshold:", "Banana", &thresh, max_thresh, thresh_callback );

  width = video.get(CV_CAP_PROP_FRAME_WIDTH);
  height = video.get(CV_CAP_PROP_FRAME_HEIGHT);
  std::cout << "largura=" << width << "\n";;
  std::cout << "altura =" << height << "\n";;

  while (key != 27) {
    key = waitKey(1);
    video >> cap;
    cvtColor(cap, frame, CV_BGR2GRAY);
    flip(frame, frame, 1);
    resize(frame, frame, size);
    cap = frame.clone();
    blur( frame, frame, Size(3, 3) );

    thresh_callback( 0, 0 );

  }
  return 0;
}


void thresh_callback(int, void* ) {

  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  vector<RotatedRect> pe_pequeno;
  /// Detect edges using Threshold
  threshold( frame, threshold_output, thresh, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  /// Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect( contours.size() );
  vector<RotatedRect> minEllipse( contours.size() );

  for ( int i = 0; i < contours.size(); i++ )
  { minRect[i] = minAreaRect( Mat(contours[i]) );
    if ( contours[i].size() > 5 )
    {
      minEllipse[i] = fitEllipse( Mat(contours[i]) );
    }
  }
  /// Draw contours + rotated rects + ellipses
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  for ( int i = 0; i < contours.size(); i++ )
  {
    Point centro = {(int)minEllipse[i].center.x, (int)minEllipse[i].center.y};
    pe_vaca_possivel = centro;
    Point size = {(int)minEllipse[i].size.width, (int)minEllipse[i].size.height};
    stringstream temp;
    int ang = (int)minEllipse[i].angle;
    int pe  = size.y - size.x ;

    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
    temp << "( pe" << pe << "," << ang << ",w y= " << size.y << ",h x =" << size.x << ")"; // concat p/converter p/ string

    if (size.x > 20  && size.x  <  640 / 4 && size.y > 35  && size.y < 480 / 4)
    {
      putText(cap, temp.str(), centro , FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) ), 1, 8, false ); // mostrar texto
      ellipse( cap, minEllipse[i], color, 2, 8 );


    }

    if ((size.x > 25  && size.x  < 60  && size.y > 25  && size.y < 70 ) || (size.x > 75  && size.x < 90 &&  size.y > 75  && size.y < 105) )
    {
      temp << "( retangulo " << pe << "," << ang << ",w y= " << size.y << ",h x =" << size.x << ")";
      int tol = 15;
      if ((ang > 0 && ang < tol) ||(ang >180 && ang <170 ) {
        if (size.y > size.x ) {
          ellipse( frame, minEllipse[i], color, 2, 8 );
          putText(frame, temp.str(), centro , FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) ), 1, 8, false ); // mostrar texto
        }
      }


    }
  }

  /// Show in a window
  namedWindow( "sem filtro", CV_WINDOW_AUTOSIZE );
  imshow( "sem filtro", cap );
  namedWindow( "pe da vaca", CV_WINDOW_AUTOSIZE );
  imshow( "pe da vaca", frame );
}
