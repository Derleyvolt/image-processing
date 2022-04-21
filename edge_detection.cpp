#include <iostream>
#include <math.h>
#include <functional>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_img.h"
#include "stb_img_write.h"

using namespace std;

struct RGB {
    int r, g, b;

    RGB& operator*=(RGB& px) {
        this->r *= px.r;
        this->g *= px.g;
        this->b *= px.b;
        return *this;
    }

    RGB& operator+=(RGB& px) {
        this->r += px.r;
        this->g += px.g;
        this->b += px.b;
        return *this;
    }
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

    // eleva todas as entradas ao quadrado
    void pow2() {
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                mat[i][j] *= mat[i][j];
            }
        }
    }

    // tira a raiz quadrada de todas as entradas
    void sqrt_() {
        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                mat[i][j].r = sqrt(mat[i][j].r);
                mat[i][j].g = sqrt(mat[i][j].g);
                mat[i][j].b = sqrt(mat[i][j].b);
            }
        }
    }

    Matrix<RGB>& operator+=(const Matrix<RGB>& param) {
        if(param.h != this->h or param.w != this->w) {
            return *this;
        }

        for(int i = 0; i < h; i++) {
            for(int j = 0; j < w; j++) {
                this->mat[i][j] += param.mat[i][j];
            }
        }

        return *this;
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

    image(int h, int w) : h(h), w(w), img(h, w) {
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

double gaussian_kernel[3][3];

double kernel_x[3][3] = {
    1, 0, -1,
    2, 0, -2,
    1, 0, -1
};

double kernel_y[3][3] = {
    1, 2, 1,
    0, 0, 0,
    -1, -2, -1
};

double kernel_laplace[3][3] = {
    0, -1, 0,
    -1, 4, -1,
    0, -1, 0
};

void generate_gaussian_kernel() {
    double aux = 2.0, sum = 0.0;

    for(int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            // quando i e j forem iguais a 0 eu estarei no centro
            // do kernel, no ápice do gráfico, ou seja, onde y é o maior possível.
            // por isso preciso variar de -2 até 2.
            gaussian_kernel[i+1][j+1] = exp(-(i*i + j*j) / aux) / (M_PI*aux);
            sum += gaussian_kernel[i+1][j+1];
        }
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            // garantindo que a soma de todas as células do kernel somem 1
            // pra intensidade média da imagem ser mantida.
            gaussian_kernel[i][j] /= sum; 
        }
    }
}

image convolution(image& img, double kernel[3][3]) {
    int h = img.h;
    int w = img.w;

    image out(h, w);
    
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            int r = 0, g = 0, b = 0;

            for(int x = 0; x < 3; x++) {
                for(int y = 0; y < 3; y++) {
                    r += img.get_img()[(i-3/2+x + h) % h][(j-3/2+y + w) % w].r * kernel[x][y];
                    g += img.get_img()[(i-3/2+x + h) % h][(j-3/2+y + w) % w].g * kernel[x][y];
                    b += img.get_img()[(i-3/2+x + h) % h][(j-3/2+y + w) % w].b * kernel[x][y];
                }
            }

            out.get_img()[i][j] = { r, g, b };
        }
    }

    return out;
}

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

void thresholding(image& img) {
	Matrix<RGB>& src = img.get_img();

	#define WHITE RGB{255, 255, 255}
	#define BLACK RGB{0, 0, 0}

	double threshold = 80; // intensity

	for(int i = 0; i < img.h; i++) {
		for(int j = 0; j < img.w; j++) {
			RGB c = src[i][j];
			double intensity = (c.r + c.g + c.b) / 3.f;
			src[i][j] = intensity >= threshold ? WHITE : BLACK;
		}
	}
}

int main() {
    int w, h, chan;

    uint8_t* rgb_image = stbi_load("opt.png", &w, &h, &chan, 3);

    image img(rgb_image, h, w);

    generate_gaussian_kernel();

    grayscale(img);

    image temp = convolution(img, gaussian_kernel);

    image Gx = convolution(temp, kernel_x);
    image Gy = convolution(temp, kernel_y);

    Gx.get_img().pow2();
    Gy.get_img().pow2();

    Gx.get_img() += Gy.get_img();

    Gx.get_img().sqrt_();

    thresholding(Gx);

    image teste = convolution(Gx, kernel_laplace);
    
    teste.raw_img(rgb_image);

    stbi_write_png("out.png", w, h, 3, rgb_image, w * 3);
    stbi_image_free(rgb_image);
    return 0;
}
