#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std::chrono;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer, std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    std::vector<unsigned char> red_row;
    std::vector<unsigned char> green_row;
    std::vector<unsigned char> blue_row;
    for (int j = cols - 1; j >= 0; j--)
    {
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          red_row.push_back(fileReadBuffer[end - count]);
          break;
        case 1:
          green_row.push_back(fileReadBuffer[end - count]);
          break;
        case 2:
          blue_row.push_back(fileReadBuffer[end - count]);
          break;
        }
        count++;
      }
    }
    red.push_back(red_row);
    green.push_back(green_row);
    blue.push_back(blue_row);
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = red[i][j];
          break;
        case 1:
          fileBuffer[bufferSize - count] = green[i][j];
          break;
        case 2:
          fileBuffer[bufferSize - count] = blue[i][j];
          break;
        }
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

void h_reverse(std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  auto temp_red(red);
  auto temp_green(green);
  auto temp_blue(blue);

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      red[i][j] = temp_red[i][cols - j - 1];
      green[i][j] = temp_green[i][cols - j - 1];
      blue[i][j] = temp_blue[i][cols - j - 1];
    }
  }
}

void v_reverse(std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  auto temp_red(red);
  auto temp_green(green);
  auto temp_blue(blue);

  for (int i = 0; i < rows; i++)
  {
    red[i] = temp_red[rows - i - 1];
    green[i] = temp_green[rows - i - 1];
    blue[i] = temp_blue[rows - i - 1];
  }
}

std::vector<std::vector<unsigned char>> conv2D(std::vector<std::vector<unsigned char>> &img, std::vector<std::vector<int>> &kernel)
{
  int out_h = img.size() - 1;
  int out_w = img[0].size() - 1;
  std::vector<std::vector<unsigned char>> ret(img);

  for (int i = 1; i < out_h; i++)
  {
    for (int j = 1; j < out_w; j++)
    {
      int conv = 0;
      // compute ret[i][j]
      for (int muli = 0; muli < 3; muli++)
      {
        for (int mulj = 0; mulj < 3; mulj++)
        {
          /* skipping 0 multiplcations */
          if (img[i + muli - 1][j + mulj - 1] == 0 || kernel[muli][mulj] == 0)
            continue;
          conv += img[i + muli - 1][j + mulj - 1] * kernel[muli][mulj];
        }
      }
      if (conv > 255)
        conv = 255;

      else if (conv < 0)
        conv = 0;

      ret[i][j] = conv;
    }
  }

  return ret;
}

void sharpen(std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  std::vector<std::vector<int>> kernel{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
  red = conv2D(red, kernel);
  green = conv2D(green, kernel);
  blue = conv2D(blue, kernel);
}

void sepia(std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  auto temp_red(red);
  auto temp_green(green);
  auto temp_blue(blue);

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      unsigned int red_pixel = 0.393 * temp_red[i][j] + 0.769 * temp_green[i][j] + 0.189 * temp_blue[i][j];
      unsigned int green_pixel = 0.349 * temp_red[i][j] + 0.686 * temp_green[i][j] + 0.168 * temp_blue[i][j];
      unsigned int blue_pixel = 0.272 * temp_red[i][j] + 0.534 * temp_green[i][j] + 0.131 * temp_blue[i][j];
      red[i][j] = red_pixel > 255 ? 255 : red_pixel;
      green[i][j] = green_pixel > 255 ? 255 : green_pixel;
      blue[i][j] = blue_pixel > 255 ? 255 : blue_pixel;
    }
  }
}

void mulSign(std::vector<std::vector<unsigned char>> &red, std::vector<std::vector<unsigned char>> &green, std::vector<std::vector<unsigned char>> &blue)
{
  for (int i = 0; i < cols; i++)
  {
    int j = (rows * i) / cols;
    red[j][i] = 255;
    green[j][i] = 255;
    blue[j][i] = 255;

    red[rows - j - 1][i] = 255;
    green[rows - j - 1][i] = 255;
    blue[rows - j - 1][i] = 255;
  }
}

int main(int argc, char *argv[])
{
  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  std::vector<std::vector<unsigned char>> red_pixels;
  std::vector<std::vector<unsigned char>> green_pixels;
  std::vector<std::vector<unsigned char>> blue_pixels;

  // read input file
  auto tick = high_resolution_clock::now();

  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer, red_pixels, green_pixels, blue_pixels);

  auto tock1 = high_resolution_clock::now();
  auto read_input_duration = duration_cast<milliseconds>(tock1 - tick);

  // apply filters
  h_reverse(red_pixels, green_pixels, blue_pixels);
  v_reverse(red_pixels, green_pixels, blue_pixels);
  sharpen(red_pixels, green_pixels, blue_pixels);
  sepia(red_pixels, green_pixels, blue_pixels);
  mulSign(red_pixels, green_pixels, blue_pixels);

  auto tock2 = high_resolution_clock::now();
  auto filters_duration = duration_cast<milliseconds>(tock2 - tock1);

  // write output file
  writeOutBmp24(fileBuffer, "output.bmp", bufferSize, red_pixels, green_pixels, blue_pixels);
  auto tock3 = high_resolution_clock::now();
  auto write_output_duration = duration_cast<milliseconds>(tock3 - tock2);

  auto tock = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(tock - tick);

  std::cout << "Read Input Execution Time: " << read_input_duration.count() << endl
            << "Filters Duration Time: " << filters_duration.count() << endl
            << "Write Output Execution Time: " << write_output_duration.count() << endl;

  std::cout << "Execution Time: " << duration.count() << " miliseconds" << endl;

  return 0;
}