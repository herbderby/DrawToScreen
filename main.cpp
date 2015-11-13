
#include <iostream>
#include <mutex>
#include <thread>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

class Screen {
 public:
  Screen(int32_t width, int32_t height)
      : width_(width), height_(height) {
    bits_[0].reset(new uint32_t[width * height]);
    bits_[1].reset(new uint32_t[width * height]);
    glClear(GL_COLOR_BUFFER_BIT);
    glFinish();
  }

  void Clear() {
    Set(0x00000000);
  }

  void Draw() {
    glDrawPixels(width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, bits_[current_].get());
    glFinish();
  }

  void Set(uint32_t c) {
    for(int i = 0; i < width_ * height_; i++) {
      bits_[current_][i] = c;
    }
  }

  template <typename F>
  void DrawLoop(F f) {
    while (true) {
      uint32_t* bits;
      {
        std::lock_guard<std::mutex> l(mu_);
        bits = bits_[1 - current_].get();
      }
      f(width_, height_, bits);
      glutPostRedisplay();
    }
  }

  void Display() {
    std::lock_guard<std::mutex> l(mu_);
    Draw();
    current_ = 1 - current_;
  }

 private:
  int32_t width_;
  int32_t height_;
  std::unique_ptr<uint32_t[]> bits_[2];
  std::mutex mu_;
  int current_{0};
};

void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:             // ESCAPE key
      glutLeaveGameMode();
      exit (0);

      break;
  }
}


int main(int argc, char* argv[])
{
  std::cout << "Hello, World!\n";

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

  glutEnterGameMode();
  int width = glutGameModeGet(GLUT_GAME_MODE_WIDTH);
  int height = glutGameModeGet(GLUT_GAME_MODE_HEIGHT);
  int depth = glutGameModeGet(GLUT_GAME_MODE_PIXEL_DEPTH);

  Screen screen(width, height);
  screen.Clear();

  static Screen& gScreen = screen;
  static std::mutex gMutex;

  auto display = [](){
    gScreen.Display();
  };

  uint32_t color = 0;
  std::thread t(
      [&]() {
        screen.DrawLoop(
            [&](int32_t width, int32_t height, uint32_t* bits) {
              for (int i = 0; i < width * height; i++) {
                bits[i] = color;
              }
              color += 1;
            }
        );
      }
  );

  glutKeyboardFunc(Keyboard);
  glutDisplayFunc(display);

  std::cout << "done. w: " << width << " h: " << height << " d: " << depth << std::endl;

  glutMainLoop();

  // Should never get here.
  return 0;
}

/* std::cout << "Draw" << std::endl;
 char buffer[1024*1024*4];

 glClear(GL_COLOR_BUFFER_BIT);

 for (int i = 0; i < 1024*1024*4; ++i) {

   buffer[i] = 0;
 }

 for (int i = 0; i < 1024*1024*4; ++i) {
   if (i % 4 == 0) {
     buffer[i] = 0xFF;
   }
 }

 glWindowPos2i(10,80);
 glDrawPixels(1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
 std::cout << "End Draw" << std::endl;

 glFinish(); */
