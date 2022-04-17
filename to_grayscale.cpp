#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_img.h"
#include "stb_img_write.h"

using namespace std;

struct RGB {
    int r, g, b;
};

template<typename T>
class Matrix {
public:
    Matrix(int h, int w) : h(h), w(w) {
        mat = new T*[h];
        for(int i = 0; i < h; i++) {
            mat[i] = new T[w];
        }
    }

    ~Matrix() {
        for(int i = 0; i < h; i++) {
            delete mat[i];
        }

        delete mat;
    }

	T*& operator[](int x) {
		return mat[x];
	}

	friend ostream& operator<<(ostream& ost, const Matrix<T>& obj) {
		for(int i = 0; i < obj.h; i++) {
			for(int j = 0; j < obj.w; j++) {
				ost << obj.mat[i][j] << " ";
			}
			ost << endl;
		}

		return ost;
	}

	int h, w;
private:
    T** mat;
};

// RGB
class image {
public:
    #define channel 3

    image(uint8_t* src, int h, int w) : h(h), w(w), img(h, w) {
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                img[i][j] = { src[channel*i*w+(channel*j)], 
                              src[channel*i*w+(channel*j+1)], 
                              src[channel*i*w+(channel*j+2)] };
            }
        }
    }

    void raw_img(uint8_t* dest) {
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                dest[(channel*j+0)+i*w*channel] = img[i][j].r;
                dest[(channel*j+1)+i*w*channel] = img[i][j].g;
                dest[(channel*j+2)+i*w*channel] = img[i][j].b;
            }
        }
    }

    Matrix<RGB>& get_img() {
        return img;
    }

    int h, w;
private:
    Matrix<RGB> img;
};

// as 3 componentes do pixel agora terão os mesmos valores.. dessa forma podemos considerar que a imagem
// possui apenas 1 canal (e que pode ser diminuída em 3x sem perca de informação).

void grayscale(image& img) {
	Matrix<RGB>& aux = img.get_img();

	for(int i = 0; i < img.h; i++) {
		for(int j = 0; j < img.w; j++) {
			RGB c = aux[i][j];
			int px = (c.r + c.g + c.b) / 3.f; // uma imagem em grayscale basicamente é uma imagem composta apenas pela intensidade dos pixels
			aux[i][j] = { px, px, px };
		}
	}
}

int main() {
    int w, h, chan;

    uint8_t* rgb_image = stbi_load("lena.png", &w, &h, &chan, 3);

    image img(rgb_image, h, w);

    grayscale(img);
    img.raw_img(rgb_image);

    stbi_write_png("out.png", w, h, 3, rgb_image, w * 3);
    stbi_image_free(rgb_image);
    return 0;
}
