#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

namespace ImageFormats
{

class ImageFormatInterface
{
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class Ppm : public ImageFormatInterface
{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override
    {
        return img_lib::SavePPM(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override
    {
        return img_lib::LoadPPM(file);
    }
};

class Jpeg : public ImageFormatInterface
{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override
    {
        return img_lib::SaveJPEG(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override
    {
        return img_lib::LoadJPEG(file);
    }
};

class BMP : public ImageFormatInterface
{
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override
    {
        return img_lib::SaveBMP(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const override
    {
        return img_lib::LoadBMP(file);
    }
};
} // ImageFormats


enum class Format : uint8_t
{
    UNKNOWN,
    JPEG,
    PPM,
    BMP,
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

ImageFormats::ImageFormatInterface* GetFormatInterface(const img_lib::Path& path)
{
    ImageFormats::ImageFormatInterface *res = nullptr;
    switch (GetFormatByExtension(path))
    {
        case Format::JPEG:
        {
            static ImageFormats::Jpeg jpeg;
            res = &jpeg;
            break;
        }
        case Format::PPM:
        {
            static ImageFormats::Ppm ppm;
            res = &ppm;
            break;
        }
        case Format::BMP:
        {
            static ImageFormats::BMP ppm;
            res = &ppm;
            break;
        }
        default:
        {
            break;
        }
    }
    return res;
}





int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    auto inputFormat = GetFormatInterface(in_path);
    if (!inputFormat)
    {
        cout << "Unknown format of the input file" << endl;
        return 2;
    }

    auto outputFormat = GetFormatInterface(out_path);
    if (!outputFormat)
    {
        cout << "Unknown format of the output file" << endl;
        return 3;
    }

    img_lib::Image image = inputFormat->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }

    if (!outputFormat->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}