/*
 * Author: Frank Erens
 */

#include "Spiralify.hpp"

#include <QtGui/QColor>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <future>
#include <iostream>
#include <limits>
#include <thread>

constexpr static size_t kOffRed   = 2*8;
constexpr static size_t kOffGreen = 1*8;
constexpr static size_t kOffBlue  = 0*8;
constexpr static size_t kOffDepth = 3*8;

constexpr static uint32_t kMaskRed   = 0xFF << kOffRed;
constexpr static uint32_t kMaskGreen = 0xFF << kOffGreen;
constexpr static uint32_t kMaskBlue  = 0xFF << kOffBlue;
constexpr static uint32_t kMaskDepth = 0xFF << kOffDepth;

class Color {
public:
    Color() : mData{} {}

    Color(QRgb rgb, uint8_t d) : mData{rgb} {
        this->d(d);
        requantize();
    }

    Color(uint8_t i, uint8_t j, uint8_t k, uint8_t d) {
        this->d(d);
        const size_t sd = (8 - d);
        r(i << sd);
        g(j << sd);
        b(k << sd);
    }

    uint8_t r() const {
        return (mData & kMaskRed) >> kOffRed;
    }

    uint8_t g() const {
        return (mData & kMaskGreen) >> kOffGreen;
    }

    uint8_t b() const {
        return (mData & kMaskBlue) >> kOffBlue;
    }

    uint8_t d() const {
        return (mData & kMaskDepth) >> kOffDepth;
    }


    float rf() const { return float(r()); }
    float gf() const { return float(g()); }
    float bf() const { return float(b()); }


    void r(uint8_t i) {
        mData = (mData & ~kMaskRed) | (i << kOffRed);
    }

    void g(uint8_t i) {
        mData = (mData & ~kMaskGreen) | (i << kOffGreen);
    }

    void b(uint8_t i) {
        mData = (mData & ~kMaskBlue) | (i << kOffBlue);
    }

    void d(uint8_t i) {
        mData = (mData & ~kMaskDepth) | (i << kOffDepth);
    }

    void requantize() {
        const size_t sd = (8 - d());
        r(r() << sd >> sd);
        g(g() << sd >> sd);
        b(b() << sd >> sd);
    }

    size_t index() const {
         const size_t sd = (8 - d());
         size_t rs = r() >> sd, gs = g() >> sd, bs = b() >> sd;
         return rs << (2*d()) | gs << d() | bs;
    }

private:
    uint32_t mData;
};

class Image {
public:
    Image(size_t w, size_t h) :
            mWidth{w},
            mHeight{h},
            mData{new Color[w*h]} {

    }

    Image(const QImage& in, uint8_t d) : Image(in.width(), in.height()) {
        for(unsigned j = 0; j < mHeight; ++j) {
            const unsigned char* line = in.scanLine(j);
            std::memcpy(mData + j*mWidth, line, mWidth * 4);
            for(unsigned i = 0; i < mWidth; ++i) {
                (*this)(i, j).d(d);
                (*this)(i, j).requantize();
            }
        }
    }

    ~Image() {
        delete[] mData;
    }

    size_t width() const { return mWidth; }
    size_t height() const { return mHeight; }

    Color& operator()(size_t i, size_t j) {
        return mData[j*mWidth + i];
    }

    const Color& operator()(size_t i, size_t j) const {
        return mData[j*mWidth + i];
    }

    operator QImage() {
        QImage img(
            reinterpret_cast<const unsigned char*>(mData),
            mWidth, mHeight, QImage::Format_RGB32,
            [](void* data) { delete[] (Color*)data; }, mData);
        mData = nullptr;
        return img;
    }

private:
    size_t mWidth;
    size_t mHeight;
    Color* mData;
};

float distance(Color c1, Color c2) {
    float r = c2.rf() - c1.rf(),
          g = c2.gf() - c1.gf(),
          b = c2.bf() - c1.bf();
    return std::sqrt(r*r + g*g + b*b);
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

    Color findNearestColor(Color c) {
        const uint8_t cpc = 1 << mDepth;

        constexpr const unsigned int kNumThreads = 4;
        using SearchResult = std::pair<float,Color>;
        std::array<SearchResult,kNumThreads> bests;
        std::array<std::thread, kNumThreads> threads;

        const uint8_t slice = cpc / kNumThreads;

        // Search in parallel using kNumThreads threads
        for(unsigned tid = 0; tid < kNumThreads; ++tid) {
            threads[tid] = std::thread([&,tid] {

                Color best;
                float bestdist = std::numeric_limits<float>::infinity();

                const uint8_t begin = slice * tid;
                const uint8_t end = begin + slice;

                for(uint8_t i = begin; i < end; ++i) {
                    for(uint8_t j = 0; j < cpc; ++j) {
                        for(uint8_t k = 0; k < cpc; ++k) {
                            Color compare{i, j, k, mDepth};
                            if(!isColor(compare)) continue;
                            float dist = distance(c, compare);
                            if(dist < bestdist) {
                                bestdist = dist;
                                best = compare;
                            }
                        }
                    }
                }

                bests[tid] = {bestdist, best};

            });
        }

        // Wait for threads to finish
        for(unsigned tid = 0; tid < kNumThreads; ++tid) {
            threads[tid].join();
        }

        // Get best result
        auto it = std::min_element(bests.begin(), bests.end(),
            [&](const SearchResult& s1, const SearchResult& s2) {
                return s1.first < s2.first;
            });


        removeColor(it->second);

        return it->second;

    }

    size_t colorOffset(const Color& c) const {
        return c.index();
    }

    bool isColor(const Color& c) const {
        return mList[colorOffset(c)];
    }


    void removeColor(const Color& c) {
        mList[colorOffset(c)] = 0;
    }
private:
    size_t mNumColors;
    uint8_t mDepth;
    bool* mList;
};

QImage spiralify(const QImage& qin, uint8_t colordepth) {
    // DO STUFF HERE
    Image in{qin, colordepth};
    Image out{in.width(), in.height()};

    ColorSearch search{colordepth};

    std::cerr << in.width() << "x" << in.height() << std::endl;

    // Start position
    int i, j;
    i = j = in.width() / 2;

    // Center pixel
    out(i, j) = search.findNearestColor(in(i, j));

    // Iterate over image in a spiral shape
    int dx = 1, dy = -1;
    unsigned n = 1;

    // I have no idea why this works (shouldn't this be height + 1?)
    while(n < in.height()) {
        for(unsigned k = 0; k < n; ++k) {
            i += dx;
            if(i < 0 || i >= int(in.width())) continue;
            out(i, j) = search.findNearestColor(in(i, j));
        }

        for(unsigned k = 0; k < n; ++k) {
            j += dy;
            if(j < 0 || j >= int(in.height())) continue;
            out(i, j) = search.findNearestColor(in(i, j));
        }

        std::cout << "." << std::flush;
        ++n;
        dx = -dx; dy = -dy;
    }

#if 0
    for(size_t j = 0; j < in.height(); ++j) {
        for(size_t i = 0; i < in.width(); ++i) {
            Color c = in(i, j);
            Color outc = search.findNearestColor(c);
            out(i, j) = outc;
        }
        std::cout << "." << std::flush;
    }
#endif

    std::cout << std::endl;

    return QImage(out);
}
