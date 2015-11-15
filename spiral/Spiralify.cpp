/*
 * Author: Frank Erens
 */

#include "Spiralify.hpp"

#include <QtGui/QColor>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>

#if 0
class ColorList {
public:
    struct ColorRef {
        uint8_t* ref;
        uint8_t bit;

        operator bool() const {
            return *ref & (1 << bit);
        }

        ColorRef& operator=(bool b) {
            if(b) {
                *ref |= (b << bit);
            } else {
                *ref &= ~(b << bit);
            }
            return *this;
        }
    };
public:
    explicit ColorList(uint8_t d) :
            mColors{new uint8_t[(1<<(3*d))/8]}, mDepth{d} {
        std::memset(mColors, 0xFF, numBytes());
    }

    ~ColorList() {
        delete[] mColors;
    }

    size_t numColors() const {
        return 1<<(3*mDepth);
    }

    size_t numBytes() {
        return numColors() / 8;
    }

    ColorRef operator[](QColor c) {
        size_t red = c.red(), green = c.green(), blue = c.blue();
        red >>= (8 - mDepth);
        green >>= (8 - mDepth);
        blue >>= (8 - mDepth);
        size_t bitoffset = red << (mDepth*2) | green << mDepth | blue;
        return ColorRef{mColors + bitoffset/8, uint8_t(bitoffset%8)};
    }

private:
    uint8_t* mColors;
    uint8_t mDepth;
};

#endif

float distance(QColor c1, QColor c2) {
    float r = c2.redF() - c1.redF(),
          g = c2.greenF() - c1.greenF(),
          b = c2.blueF() - c1.blueF();
    return std::sqrt(r*r + b*b + g*g);
}

struct ColorSearch {
public:
    ColorSearch(uint8_t d) :
            mNumColors{1u<<(d*3u)},
            mDepth{d},
            mList{new bool[mNumColors]} {
        std::memset(mList, true, mNumColors);
    }

    ~ColorSearch() {
        delete mList;
    }

    QColor findNearestColor(QColor c) {
        const int cpc = 1 << mDepth;
        const int sd = (8 - mDepth);

        // TODO: class for this
        QColor q = {c.red() >> sd, c.green() >> sd, c.blue() >> sd};

        QColor best;
        float bestdist = std::numeric_limits<float>::infinity();

        for(int i = 0; i < cpc; ++i) {
            for(int j = 0; j < cpc; ++j) {
                for(int k = 0; k < cpc; ++k) {
                    QColor compare{i, j, k};
                    if(!isColor(compare)) continue;
                    float dist = distance(c, compare);
                    if(dist < bestdist) {
                        bestdist = dist;
                        best = compare;
                    }
                }
            }
        }

        removeColor(best);

        return QColor{best.red() << sd, best.green() << sd, best.blue() << sd};
    }

    size_t colorOffset(const QColor& c) const {
        return c.red() << (mDepth*2) | c.green() << mDepth | c.blue();
    }

    bool isColor(const QColor& c) const {
        return mList[colorOffset(c)];
    }


    void removeColor(const QColor& c) {
        mList[colorOffset(c)] = 0;
    }
private:
    size_t mNumColors;
    uint8_t mDepth;
    bool* mList;
};

QImage spiralify(const QImage& in, uint8_t colordepth) {
    // DO STUFF HERE
    QImage out{in.size(), QImage::Format_RGB32};

    ColorSearch search{colordepth};

    for(int i = 0; i < in.width(); ++i) {
        for(int j = 0; j < in.height(); ++j) {
            QColor c = in.pixel(i, j);
            QColor outc = search.findNearestColor(c);
            // std::fprintf(stderr, "## %d %d %d\n",
            //         outc.red(), outc.green(), outc.blue());
            out.setPixel(i, j, outc.rgb());
        }
    }

    return out;
}
