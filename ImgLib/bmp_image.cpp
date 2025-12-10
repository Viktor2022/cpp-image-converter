#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    uint32_t biSize = sizeof(BitmapInfoHeader); // Размер этого заголовка
    int32_t biWidth = 0;                        // Ширина изображения
    int32_t biHeight = 0;                       // Высота изображения (положительная - снизу вверх)
    uint16_t biPlanes = 1;                      // Количество плоскостей
    uint16_t biBitCount = 24;                   // Бит на пиксель (24 для RGB)
    uint32_t biCompression = 0;                 // Тип сжатия (0 = BI_RGB без сжатия)
    uint32_t biSizeImage = 0;                   // Размер изображения в байтах
    int32_t biXPelsPerMeter = 11811;            // Горизонтальное разрешение
    int32_t biYPelsPerMeter = 11811;            // Вертикальное разрешение
    uint32_t biClrUsed = 0;                     // Количество используемых цветов
    uint32_t biClrImportant = 0x1000000;        // Количество важных цветов
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    uint16_t bfType = 0x4D42;      // 'BM'
    uint32_t bfSize = 0;           // Размер файла
    uint16_t bfReserved1 = 0;      // Зарезервировано
    uint16_t bfReserved2 = 0;      // Зарезервировано
    uint32_t bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);       // Смещение до данных пикселей
}
PACKED_STRUCT_END

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

// напишите эту функцию
bool SaveBMP(const Path& file, const Image& image)
{
    if (!image)
    {
        return false;
    }

    ofstream ofs(file, ios::binary);
    if (!ofs)
    {
        return false;
    }

    const int width = image.GetWidth();
    const int height = image.GetHeight();
    const int stride = GetBMPStride(width);
    const uint32_t image_size = stride * height;
    const uint32_t file_size = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + image_size;

    BitmapFileHeader file_header;
    file_header.bfSize = file_size;

    BitmapInfoHeader info_header;
    info_header.biWidth = width;
    // положительное - снизу вверх
    info_header.biHeight = height; 
    info_header.biSizeImage = image_size;

    ofs.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    ofs.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    if (!ofs)
    {
        return false;
    }

    vector<char> buffer(stride, 0);

    for (int y = height - 1; y >= 0; --y) 
    {
        const Color* line = image.GetLine(y);
        
        // Копируем данные из RGB в BGR формат
        for (int x = 0; x < width; ++x) 
        {
            const int offset = x * 3;
            buffer[offset] = static_cast<char>(line[x].b);    
            buffer[offset + 1] = static_cast<char>(line[x].g);
            buffer[offset + 2] = static_cast<char>(line[x].r);
        }

        ofs.write(buffer.data(), stride);
        if (!ofs) 
        {
            return false;
        }
    }

    return true;


}

// напишите эту функцию
Image LoadBMP(const Path& file) 
{
    ifstream ifs(file, ios::binary);
    if (!ifs)
    {
        return {};
    }

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

    if (file_header.bfType != 0x4D42)
    { // 'BM'
        return {};
    }

    if (info_header.biBitCount != 24 || info_header.biCompression != 0)
    {
        return {};
    }

    if (info_header.biWidth <= 0 || info_header.biHeight == 0)
    {
        return {};
    }

    const int width = info_header.biWidth;
    const int height = abs(info_header.biHeight);
    const bool top_down = info_header.biHeight < 0; // отрицательная высота означает top-down
    const int stride = GetBMPStride(width);

    ifs.seekg(file_header.bfOffBits, ios::beg);

    Image result(width, height, Color::Black());

    // Буфер для чтения строки
    vector<char> buffer(stride);

    for (int y = 0; y < height; ++y)
    {
        ifs.read(buffer.data(), stride);
        if (!ifs)
        {
            return {};
        }

        const int dest_y = top_down ? y : (height - 1 - y);
        Color* line = result.GetLine(dest_y);

        // Конвертируем из BGR в RGB
        for (int x = 0; x < width; ++x)
        {
            const int offset = x * 3;
            line[x].b = static_cast<byte>(buffer[offset]);    
            line[x].g = static_cast<byte>(buffer[offset + 1]);
            line[x].r = static_cast<byte>(buffer[offset + 2]);
            line[x].a = byte{255};
        }
    }
    return result;
}

}  // namespace img_lib