#include "bmp/bmp.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace {

constexpr uint16_t BmpSignature = 0x4D42;
constexpr uint32_t BitmapInfoHeaderSize = 40;
constexpr uint32_t PixelDataOffset = 54;
constexpr uint16_t RequiredPlanes = 1;
constexpr uint16_t SupportedBitsPerPixel = 24;
constexpr uint32_t NoCompression = 0;
constexpr uint32_t NoColorTable = 0;
constexpr int BytesPerPixel = 3;
constexpr int RowAlignment = 4;
constexpr double ColorScale = 255.0;
constexpr int ByteMask = 0xFF;
constexpr std::streamsize U16Size = 2;
constexpr std::streamsize U32Size = 4;
constexpr int BitsPerByte = 8;
constexpr int TwoByteShift = 16;
constexpr int ThreeByteShift = 24;

void ReadExact(std::istream& stream, char* buffer, const std::streamsize size) {
    stream.read(buffer, size);
    if (!stream) {
        throw std::runtime_error("Unexpected end of BMP file");
    }
}

uint16_t ReadU16(std::istream& stream) {
    unsigned char bytes[U16Size] = {};
    ReadExact(stream, reinterpret_cast<char*>(bytes), U16Size);
    return static_cast<uint16_t>(bytes[0]) | static_cast<uint16_t>(bytes[1]) << BitsPerByte;
}

uint32_t ReadU32(std::istream& stream) {
    unsigned char bytes[U32Size] = {};
    ReadExact(stream, reinterpret_cast<char*>(bytes), U32Size);
    return static_cast<uint32_t>(bytes[0]) | static_cast<uint32_t>(bytes[1]) << BitsPerByte |
           static_cast<uint32_t>(bytes[2]) << TwoByteShift | static_cast<uint32_t>(bytes[3]) << ThreeByteShift;
}

int32_t ReadI32(std::istream& stream) {
    return static_cast<int32_t>(ReadU32(stream));
}

void WriteU16(std::ostream& stream, uint16_t value) {
    unsigned char bytes[U16Size] = {
        static_cast<unsigned char>(value & ByteMask),
        static_cast<unsigned char>(value >> BitsPerByte & ByteMask),
    };
    stream.write(reinterpret_cast<const char*>(bytes), U16Size);
}

void WriteU32(std::ostream& stream, uint32_t value) {
    unsigned char bytes[U32Size] = {
        static_cast<unsigned char>(value & ByteMask),
        static_cast<unsigned char>(value >> BitsPerByte & ByteMask),
        static_cast<unsigned char>((value >> TwoByteShift) & ByteMask),
        static_cast<unsigned char>((value >> ThreeByteShift) & ByteMask),
    };
    stream.write(reinterpret_cast<const char*>(bytes), U32Size);
}

int GetRowPadding(int width) {
    const int row_size = width * BytesPerPixel;
    return (RowAlignment - (row_size % RowAlignment)) % RowAlignment;
}

uint8_t ToByte(double value) {
    return static_cast<uint8_t>(std::round(Color::Clamp(value) * ColorScale));
}

}  // namespace

Image bmp::BmpReader::Read(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open input file: " + path);
    }

    const uint16_t signature = ReadU16(input);
    if (signature != BmpSignature) {
        throw std::runtime_error("Input file is not a BMP file");
    }

    ReadU32(input);
    ReadU16(input);
    ReadU16(input);
    const uint32_t pixel_offset = ReadU32(input);

    const uint32_t dib_size = ReadU32(input);
    if (dib_size != BitmapInfoHeaderSize) {
        throw std::runtime_error("Only BITMAPINFOHEADER is supported");
    }

    const int32_t width_raw = ReadI32(input);
    const int32_t height_raw = ReadI32(input);
    const uint16_t planes = ReadU16(input);
    const uint16_t bits_per_pixel = ReadU16(input);
    const uint32_t compression = ReadU32(input);
    ReadU32(input);
    ReadI32(input);
    ReadI32(input);
    const uint32_t colors_used = ReadU32(input);
    ReadU32(input);

    if (width_raw <= 0) {
        throw std::runtime_error("BMP width must be positive");
    }
    if (height_raw == 0 || height_raw == std::numeric_limits<int32_t>::min()) {
        throw std::runtime_error("BMP height is invalid");
    }
    if (planes != RequiredPlanes) {
        throw std::runtime_error("BMP planes field must be equal to 1");
    }
    if (bits_per_pixel != SupportedBitsPerPixel) {
        throw std::runtime_error("Only 24-bit BMP is supported");
    }
    if (compression != NoCompression) {
        throw std::runtime_error("Compressed BMP is not supported");
    }
    if (colors_used != NoColorTable) {
        throw std::runtime_error("BMP with color table is not supported");
    }
    if (pixel_offset < PixelDataOffset) {
        throw std::runtime_error("BMP pixel data offset is invalid");
    }

    const bool is_bottom_up = height_raw > 0;
    const int height = is_bottom_up ? static_cast<int>(height_raw) : -height_raw;
    const int width = width_raw;
    Image image(width, height);

    const std::streamoff offset =
        static_cast<std::streamoff>(pixel_offset) - static_cast<std::streamoff>(PixelDataOffset);
    if (offset > 0) {
        input.ignore(offset);
        if (!input) {
            throw std::runtime_error("Failed to seek BMP pixel data");
        }
    }

    const int padding = GetRowPadding(width);
    char padding_buffer[BytesPerPixel] = {};

    for (int row = 0; row < height; ++row) {
        const int y = is_bottom_up ? height - 1 - row : row;
        for (int x = 0; x < width; ++x) {
            unsigned char bgr[BytesPerPixel] = {};
            ReadExact(input, reinterpret_cast<char*>(bgr), BytesPerPixel);
            const Color color(static_cast<double>(bgr[2]) / ColorScale, static_cast<double>(bgr[1]) / ColorScale,
                              static_cast<double>(bgr[0]) / ColorScale);
            image.SetPixel(x, y, color);
        }
        if (padding > 0) {
            ReadExact(input, padding_buffer, padding);
        }
    }

    return image;
}

void bmp::BmpWriter::Write(const std::string& path, const Image& image) {
    if (image.IsEmpty()) {
        throw std::runtime_error("Cannot write empty image");
    }

    std::ofstream output(path, std::ios::binary);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot open output file: " + path);
    }

    const int width = image.GetWidth();
    const int height = image.GetHeight();
    const int padding = GetRowPadding(width);
    const uint32_t row_size = static_cast<uint32_t>(width * BytesPerPixel + padding);
    const uint32_t pixel_data_size = row_size * static_cast<uint32_t>(height);
    const uint32_t file_size = PixelDataOffset + pixel_data_size;

    WriteU16(output, BmpSignature);
    WriteU32(output, file_size);
    WriteU16(output, 0);
    WriteU16(output, 0);
    WriteU32(output, PixelDataOffset);

    WriteU32(output, BitmapInfoHeaderSize);
    WriteU32(output, static_cast<uint32_t>(width));
    WriteU32(output, static_cast<uint32_t>(height));
    WriteU16(output, RequiredPlanes);
    WriteU16(output, SupportedBitsPerPixel);
    WriteU32(output, NoCompression);
    WriteU32(output, pixel_data_size);
    WriteU32(output, 0);
    WriteU32(output, 0);
    WriteU32(output, NoColorTable);
    WriteU32(output, 0);

    const unsigned char zeros[BytesPerPixel] = {0, 0, 0};
    for (int row = height - 1; row >= 0; --row) {
        for (int x = 0; x < width; ++x) {
            const Color& color = image.At(x, row);
            const unsigned char bgr[BytesPerPixel] = {
                ToByte(color.GetBlue()),
                ToByte(color.GetGreen()),
                ToByte(color.GetRed()),
            };
            output.write(reinterpret_cast<const char*>(bgr), BytesPerPixel);
        }
        if (padding > 0) {
            output.write(reinterpret_cast<const char*>(zeros), padding);
        }
    }

    if (!output) {
        throw std::runtime_error("Failed while writing output BMP file");
    }
}
