#include <iostream>
#include <math.h>

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

double gaussian_kernel[5][5];

void generate_gaussian_kernel() {
    double aux = 2.0, sum = 0.0;

    for(int i = -2; i < 3; i++) {
        for(int j = -2; j < 3; j++) {
            // quando i e j forem iguais a 0 eu estarei no centro
            // do kernel, no ápice do gráfico, ou seja, onde y é o maior possível.
            // por isso preciso variar de -2 até 2.
            gaussian_kernel[i+2][j+2] = exp(-(i*i + j*j) / aux) / (M_PI*aux);
            sum += gaussian_kernel[i+2][j+2];
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            // garantindo que a soma de todas as células do kernel somem 1
            // pra intensidade média da imagem ser mantida.
            gaussian_kernel[i][j] /= sum; 
        }
    }
}

void gaussian_blur(image& img) {
    int h = img.h;
    int w = img.w;

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            int r = 0, g = 0, b = 0;

            for(int x = 0; x < 5; x++) {
                for(int y = 0; y < 5; y++) {
                    r += img.get_img()[(i-5/2+x + h) % h][(j-5/2+y + w) % w].r * gaussian_kernel[x][y];
                    g += img.get_img()[(i-5/2+x + h) % h][(j-5/2+y + w) % w].g * gaussian_kernel[x][y];
                    b += img.get_img()[(i-5/2+x + h) % h][(j-5/2+y + w) % w].b * gaussian_kernel[x][y];
                }
            }

            img.get_img()[i][j] = { r, g, b };
        }
    }
}

int main() {
    int w, h, chan;

    uint8_t* rgb_image = stbi_load("photo3.png", &w, &h, &chan, 3);

    image img(rgb_image, h, w);

    generate_gaussian_kernel();

    gaussian_blur(img);
    img.raw_img(rgb_image);

    stbi_write_png("out.png", w, h, 3, rgb_image, w * 3);
    stbi_image_free(rgb_image);
    return 0;
}
