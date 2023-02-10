#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

vector<vector<int>> findColor(Mat img);
void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues);
void CreateTrackbar();
Point getContours(Mat imgDil);

Mat imgHSV, mask, imgColor;
int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;

VideoCapture cap(0);
Mat img;
vector<vector<int>> newPoints;

// newPoints -> x,y,color
// pink -> {151,75,255,170,255,255}
// yellow -> {29, 57, 255, 38, 255, 255}

vector<vector<int>> myColors{
    {151, 75, 255, 170, 255, 255}, {29, 57, 255, 38, 255, 255}};

vector<Scalar> myColorValues{{147, 20, 255}, {0, 255, 255}};

// pink -> {147,20,255}
// yellow -> {0,255,255}

int main(int argc, char **argv)
{

  // CreateTrackbar();

  while (true)
  {
    cap.read(img);
    newPoints = findColor(img);
    drawOnCanvas(newPoints, myColorValues);

    imshow("Image", img);
    waitKey(1);
  }

  return 0;
}

vector<vector<int>> findColor(Mat img)
{
  cvtColor(img, imgHSV, COLOR_BGR2HSV);

  for (int i = 0; i < myColors.size(); i++)
  {
    Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
    Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
    Mat mask;
    inRange(imgHSV, lower, upper, mask);
    // imshow(to_string(i), mask);
    Point myPoint = getContours(mask);

    if (myPoint.x != 0 && myPoint.y != 0)
    {
      newPoints.push_back({myPoint.x, myPoint.y, i});
    }
  }

  return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
  for (int i = 0; i < newPoints.size(); i++)
  {
    circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
  }
}

void CreateTrackbar() // Creating Trackbar
{
  namedWindow("Trackbars", (640, 200));
  createTrackbar("Hue Min", "Trackbars", &hmin, 179);
  createTrackbar("Hue Max", "Trackbars", &hmax, 179);
  createTrackbar("Sat Min", "Trackbars", &smin, 255);
  createTrackbar("Sat Max", "Trackbars", &smax, 255);
  createTrackbar("Val Min", "Trackbars", &vmin, 255);
  createTrackbar("Val Max", "Trackbars", &vmax, 255);
}

void DisplayTrackbar(Mat img) // To Display Trackbar Values
{
  cvtColor(img, imgHSV, COLOR_BGR2HSV);

  Scalar lower(hmin, smin, vmin);
  Scalar upper(hmax, smax, vmax);
  inRange(imgHSV, lower, upper, mask);

  imshow("Image", img);
  imshow("Image HSV", imgHSV);
  imshow("Image Mask", mask);
}

Point getContours(Mat imgDil)
{
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());
  Point myPoint(0, 0);

  for (int i = 0; i < contours.size(); i++)
  {
    int area = contourArea(contours[i]);
    if (area > 1000)
    {
      float peri = arcLength(contours[i], true);
      approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
      boundRect[i] = boundingRect(conPoly[i]);
      myPoint.x = boundRect[i].x + boundRect[i].width / 2;
      myPoint.y = boundRect[i].y;

      // drawContours(img, conPoly, i, Scalar(0, 0, 0), 3);
      // rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
    }
  }

  return myPoint;
}