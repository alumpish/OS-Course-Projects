#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <iostream>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std::chrono;

#pragma pack(1)
#pragma once

const int NUMBER_OF_THREADS = 3;

char *fileBuffer;
int bufferSize;
std::vector<std::vector<unsigned char>> red_pixels;
std::vector<std::vector<unsigned char>> green_pixels;
std::vector<std::vector<unsigned char>> blue_pixels;

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

class threadData
{
public:
  std::vector<std::vector<unsigned char>> img;
  int count;
  std::vector<std::vector<int>> kernel;
  std::vector<double> ratio;

  threadData(std::vector<std::vector<unsigned char>> &img, int count)
      : img(img), count(count) {}
  threadData(std::vector<std::vector<unsigned char>> &img, int count, std::vector<std::vector<int>> kernel)
      : img(img), count(count), kernel(kernel) {}
  threadData(std::vector<std::vector<unsigned char>> &img, int count, std::vector<double> ratio)
      : img(img), count(count), ratio(ratio) {}
};

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

void *get_pixels(void *data1)
{
  threadData *data = (threadData *)data1;

  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    data->count += extra;
    std::vector<unsigned char> row;

    for (int j = cols - 1; j >= 0; j--)
    {
      row.push_back(fileBuffer[bufferSize - data->count]);
      // go to the next position in the buffer
      data->count += 3;
    }
    data->img.push_back(row);
  }
  pthread_exit(NULL);
}

void getPixlesFromBMP24()
{
  threadData *data[3]; // red , green , blue

  pthread_t threads[NUMBER_OF_THREADS];
  int count = 1;
  int return_code;
  data[0] = new threadData(red_pixels, count);
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_create(&threads[tid], NULL, get_pixels, (void *)data[tid]);
    if (return_code)
    {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
    count++;
    if (tid == 0)
      data[1] = new threadData(green_pixels, count);
    else if (tid == 1)
      data[2] = new threadData(blue_pixels, count);
  }

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }
  red_pixels = data[0]->img;
  green_pixels = data[1]->img;
  blue_pixels = data[2]->img;
}

void *write_pixels(void *data1)
{
  threadData *data = (threadData *)data1;

  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    data->count += extra;
    for (int j = 0; j < cols; j++)
    {
      fileBuffer[bufferSize - data->count] = data->img[i][j];
      // go to the next position in the buffer
      data->count += 3;
    }
  }
  pthread_exit(NULL);
}

void writeOutBmp24(const char *nameOfFileToCreate)
{

  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }

  threadData *data[3]; // red , green , blue

  pthread_t threads[NUMBER_OF_THREADS];
  int count = 1;
  int return_code;
  data[0] = new threadData(red_pixels, count);
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_create(&threads[tid], NULL, write_pixels, (void *)data[tid]);
    if (return_code)
    {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
    count++;
    if (tid == 0)
      data[1] = new threadData(green_pixels, count);
    else if (tid == 1)
      data[2] = new threadData(blue_pixels, count);
  }

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }

  write.write(fileBuffer, bufferSize);
}

void h_reverse()
{
  auto temp_red(red_pixels);
  auto temp_green(green_pixels);
  auto temp_blue(blue_pixels);

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      red_pixels[i][j] = temp_red[i][cols - j - 1];
      green_pixels[i][j] = temp_green[i][cols - j - 1];
      blue_pixels[i][j] = temp_blue[i][cols - j - 1];
    }
  }
}

void v_reverse()
{
  auto temp_red(red_pixels);
  auto temp_green(green_pixels);
  auto temp_blue(blue_pixels);

  for (int i = 0; i < rows; i++)
  {
    red_pixels[i] = temp_red[rows - i - 1];
    green_pixels[i] = temp_green[rows - i - 1];
    blue_pixels[i] = temp_blue[rows - i - 1];
  }
}

void *conv2D(void *data1)
{
  threadData *data = (threadData *)data1;
  int out_h = data->img.size() - 1;
  int out_w = data->img[0].size() - 1;
  std::vector<std::vector<unsigned char>> ret(data->img);

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
          if (data->img[i + muli - 1][j + mulj - 1] == 0 || data->kernel[muli][mulj] == 0)
            continue;
          conv += data->img[i + muli - 1][j + mulj - 1] * data->kernel[muli][mulj];
        }
      }
      if (conv > 255)
        conv = 255;

      else if (conv < 0)
        conv = 0;

      ret[i][j] = conv;
    }
  }
  data->img = ret;
  pthread_exit(NULL);
}

void sharpen()
{
  std::vector<std::vector<int>> kernel{{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

  threadData *data[3]; // red , green , blue
  data[0] = new threadData(red_pixels, 0, kernel);
  data[1] = new threadData(green_pixels, 0, kernel);
  data[2] = new threadData(blue_pixels, 0, kernel);

  pthread_t threads[NUMBER_OF_THREADS];
  int return_code;
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_create(&threads[tid], NULL, conv2D, (void *)data[tid]);
    if (return_code)
    {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
  }
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }
  red_pixels = data[0]->img;
  green_pixels = data[1]->img;
  blue_pixels = data[2]->img;
}

void *sepiaRGB(void *data1)
{
  threadData *data = (threadData *)data1;
  auto out(data->img);

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      unsigned int pixel = data->ratio[0] * red_pixels[i][j] + data->ratio[1] * green_pixels[i][j] + data->ratio[2] * blue_pixels[i][j];
      out[i][j] = pixel > 255 ? 255 : pixel;
    }
  }
  data->img = out;
  pthread_exit(NULL);
}

void sepia()
{
  threadData *data[3]; // red , green , blue
  data[0] = new threadData(red_pixels, 0, {0.393, 0.769, 0.189});
  data[1] = new threadData(green_pixels, 0, {0.349, 0.686, 0.168});
  data[2] = new threadData(blue_pixels, 0, {0.272, 0.534, 0.131});

  pthread_t threads[NUMBER_OF_THREADS];
  int return_code;

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_create(&threads[tid], NULL, sepiaRGB, (void *)data[tid]);
    if (return_code)
    {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
  }

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }

  red_pixels = data[0]->img;
  green_pixels = data[1]->img;
  blue_pixels = data[2]->img;
}

void *mulSignRGB(void *data1)
{
  threadData *data = (threadData *)data1;
  std::vector<std::vector<unsigned char>> temp(data->img);

  for (int i = 0; i < cols; i++)
  {
    int j = (rows * i) / cols;
    temp[j][i] = 255;
    temp[rows - j - 1][i] = 255;
  }
  data->img = temp;
  pthread_exit(NULL);
}

void mulSign()
{
  threadData *data[3]; // red , green , blue

  pthread_t threads[NUMBER_OF_THREADS];

  int return_code;
  data[0] = new threadData(red_pixels, 0);
  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_create(&threads[tid], NULL, mulSignRGB, (void *)data[tid]);
    if (return_code)
    {
      printf("ERROR; return code from pthread_create() is %d\n", return_code);
      exit(-1);
    }
    if (tid == 0)
      data[1] = new threadData(green_pixels, 0);
    else if (tid == 1)
      data[2] = new threadData(blue_pixels, 0);
  }

  for (int tid = 0; tid < NUMBER_OF_THREADS; tid++)
  {
    return_code = pthread_join(threads[tid], NULL);
    if (return_code)
    {
      printf("ERROR; return code from pthread_join() is %d\n", return_code);
      exit(-1);
    }
  }
  red_pixels = data[0]->img;
  green_pixels = data[1]->img;
  blue_pixels = data[2]->img;
}

int main(int argc, char *argv[])
{
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }

  // read input file
  auto tick = high_resolution_clock::now();

  getPixlesFromBMP24();

  auto tock1 = high_resolution_clock::now();
  auto read_input_duration = duration_cast<milliseconds>(tock1 - tick);

  // apply filters
  h_reverse();
  v_reverse();
  sharpen();
  sepia();
  mulSign();

  auto tock2 = high_resolution_clock::now();
  auto filters_duration = duration_cast<milliseconds>(tock2 - tock1);

  // write output file
  writeOutBmp24("output.bmp");
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