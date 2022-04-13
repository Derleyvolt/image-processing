#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// compressão e descompressão mt fácil com esses headers..

#include "stb_img.h"
#include "stb_img_write.h"

using namespace std;

int main() {
    int width, height, chan;

    uint8_t* rgb_image = stbi_load("teste.png", &width, &height, &chan, 3);

    // mudando os pixels pra vermelho
    for(int i = 0; i < width * height * 3; i += 3) {
        rgb_image[i]   = 255;
        rgb_image[i+1] = 0;
        rgb_image[i+2] = 0;
    }

    // nome, x, y, canais, source, stride
    stbi_write_png("out.png", width, height, 3, rgb_image, width * 3);
    stbi_image_free(rgb_image);
    return 0;
}
