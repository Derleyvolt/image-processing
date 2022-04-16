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

void blur(image& img) {
    int h = img.h;
    int w = img.w;

    #define x_ker 3
    #define y_ker 3

    // podemos aumentar o matriz filter
    // e com isso obter a média de mais vizinhos.
    // é importante que a soma de todos os elementos de filter
    // seja 1, pro bilho da imagem se manter.
    double filter[x_ker][y_ker] = {
        0.1, 0.1, 0.1,
        0.1, 0.1, 0.1,
        0.1, 0.1, 0.1
    };

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            int r = 0, g = 0, b = 0;

            for(int xf = 0; xf < x_ker; xf++) {
                for(int yf = 0; yf < y_ker; yf++) {
                    // se o valor for maior que h ou w, ou menor que 0, a gente faz um
                    // wrap-around pra pegar o pixel da outra parte da matriz.
                    // isso é mais fácil que tratar os casos onde (x, y) pertencem à
                    // borda da matriz
                    r += img.get_img()[(i-1+xf + h) % h][(j-1+yf + w) % w].r * filter[xf][yf];
                    g += img.get_img()[(i-1+xf + h) % h][(j-1+yf + w) % w].g * filter[xf][yf];
                    b += img.get_img()[(i-1+xf + h) % h][(j-1+yf + w) % w].b * filter[xf][yf];
                }
            }

            img.get_img()[i][j] = {r, g, b};
        }
    }
}

int main() {
    int w, h, n;

    uint8_t* img_src = stbi_load("gato_amor.png", &w, &h, &n, 3);

    image img(img_src, h, w);

    blur(img);
    img.raw_img(img_src);

    stbi_write_png("out.png", w, h, n, img_src, w * n);
    stbi_image_free(img_src);
    return 0;
}
