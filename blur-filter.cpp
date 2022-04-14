#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_img.h"
#include "stb_img_write.h"

using namespace std;

struct pixel {
    int r;
    int g;
    int b;
};

// RGB
class image {
public:
    #define channel 3

    image(uint8_t* src, int h, int w) : h(h), w(w) {
        px = new pixel[h * w];
        for(int i = 0; i < h * w; i++) {
            px[i] = { src[i*3], src[i*3+1], src[i*3+2] };
        }
    }

    void raw_img(uint8_t* dest) const {
        for(int i = 0; i < h * w; i++) {
            dest[i*channel+0] = px[i].r;
            dest[i*channel+1] = px[i].g;
            dest[i*channel+2] = px[i].b;
        }
    }

    pixel* get_img() const {
        return px;
    }

    int h, w;
private:
    pixel* px;
};

void blur(image* img) {
    // i iniciando em w           --> evita a primeira linha
    // i terminando em w * h - w  --> evita a última linha
    for(int i = img->w; i < img->w * img->h - img->w; i++) {
        // a minha abordagem não aplica o filtro aos pixels da borda..
        // também é possível fazer um wrap around quando os pixels forem os da borda.
        if((i+1) % img->w > 1 && (i+1) % img->w > 0) {
            pixel img_left         = img->get_img()[i-1];
            pixel img_right        = img->get_img()[i+1];
            pixel img_top          = img->get_img()[i-img->w];
            pixel img_bottom       = img->get_img()[i+img->w];
            pixel img_top_left     = img->get_img()[i-img->w-1];
            pixel img_top_right    = img->get_img()[i-img->w+1];
            pixel img_bottom_left  = img->get_img()[i+img->w-1];
            pixel img_bottom_right = img->get_img()[i+img->w+1];

            int r = img_left.r + img_right.r + img_top.r + img_bottom.r + img_top_left.r + img_top_right.r + img_bottom_left.r + img_bottom_right.r;
            int g = img_left.g + img_right.g + img_top.g + img_bottom.g + img_top_left.g + img_top_right.g + img_bottom_left.g + img_bottom_right.g;
            int b = img_left.b + img_right.b + img_top.b + img_bottom.b + img_top_left.b + img_top_right.b + img_bottom_left.b + img_bottom_right.b;

            // a média da soma do pixel atual com os seus oito vizinhos.
            img->get_img()[i].r = (r + img->get_img()[i].r) / 9;
            img->get_img()[i].g = (g + img->get_img()[i].g) / 9;
            img->get_img()[i].b = (b + img->get_img()[i].b) / 9;
        }
    }
}

int main() {
    int width, height, chan;

    uint8_t* rgb_image = stbi_load("gato_amor.png", &width, &height, &chan, 3);

    image img(rgb_image, height, width);

    blur(&img);

    img.raw_img(rgb_image);

    // nome, x, y, canais, source, stride
    stbi_write_png("out.png", width, height, 3, rgb_image, width * 3);
    stbi_image_free(rgb_image);
    return 0;
}
