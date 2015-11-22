#include "Tga.h"

#include <cassert>
#include <cstring>

namespace Tga
{
	TgaImage::TgaImage(u8* RawData, u64 Size)
	{
		assert(RawData);
		assert(Size > sizeof(Header));

		u32 ReadOffset = 0;
		_ReadHeader(RawData, ReadOffset);
		_ReadData(RawData + ReadOffset);
	}

	TgaImage::~TgaImage()
	{
		delete[] _ImageData.ImageID;
		_ImageData.ImageID = nullptr;
		delete[] _ImageData.ImageData;
		_ImageData.ImageData = nullptr;
	}
	
	void TgaImage::GetImage(u32& Width, u32& Height, u8*& ImageData)
	{
		Width = _ImageHeader.ImageSpec.Width;
		Height = _ImageHeader.ImageSpec.Height;
		ImageData = new u8[Width * Height * 4];
		memcpy(ImageData, _ImageData.ImageData, Width * Height * 4);
	}

	void TgaImage::_Read(u8* Dst, const u8* Src, u32 Size, u32& ReadOffset)
	{
		memcpy(Dst, Src, Size);
		ReadOffset += Size;
	}

	void TgaImage::_ReadHeader(u8* RawData, u32& ReadOffset)
	{
		// Image general info
		_Read(&_ImageHeader.IDLength, RawData, sizeof(u8), ReadOffset);
		_Read(&_ImageHeader.ColorMapType, RawData + ReadOffset, sizeof(u8), ReadOffset);
		_Read(&_ImageHeader.ImageType, RawData + ReadOffset, sizeof(u8), ReadOffset);
		
		// Color Map Spec
		_Read((u8*)&_ImageHeader.ColorMapSpec.Origin, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read((u8*)&_ImageHeader.ColorMapSpec.Length, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read(&_ImageHeader.ColorMapSpec.PixelSize, RawData + ReadOffset, sizeof(u8), ReadOffset);
		
		// Image Spec
		_Read((u8*)&_ImageHeader.ImageSpec.XOrigin, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read((u8*)&_ImageHeader.ImageSpec.YOrigin, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read((u8*)&_ImageHeader.ImageSpec.Width, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read((u8*)&_ImageHeader.ImageSpec.Height, RawData + ReadOffset, sizeof(u16), ReadOffset);
		_Read(&_ImageHeader.ImageSpec.Depth, RawData + ReadOffset, sizeof(u8), ReadOffset);
		_Read(&_ImageHeader.ImageSpec.ImageDesc, RawData + ReadOffset, sizeof(u8), ReadOffset);
	}

	void TgaImage::_ReadData(u8* RawData)
	{
		_ReadImageID(RawData);
		u8* ImageData = RawData + _ImageHeader.IDLength;
		if (_ImageHeader.ColorMapType)
		{
			_ReadMappedImageData(ImageData);
		}
		else
		{
			_ReadDirectImageData(ImageData);
		}
		
		if (_IsVerticalInverted())
		{
			u32 PixelSize = 4;
			u32 Width = _ImageHeader.ImageSpec.Width;
			u32 Height = _ImageHeader.ImageSpec.Height;

			u32 LineSize = Width * PixelSize;
			u8* TempLine = new u8[_ImageHeader.ImageSpec.Width * PixelSize];
			for (u32 Y = 0, HeightDiv2 = Height / 2, SwappedY = Height - 1; Y < HeightDiv2; ++Y, --SwappedY)
			{
				u32 Offset = Y * LineSize;
				u32 OffsetSwapped = SwappedY * LineSize;
				memcpy(TempLine, &_ImageData.ImageData[Offset], LineSize);
				memcpy(&_ImageData.ImageData[Offset], &_ImageData.ImageData[OffsetSwapped], LineSize);
				memcpy(&_ImageData.ImageData[OffsetSwapped], TempLine, LineSize);
			}
		}
		
		if (_IsHorizontalInverted())
		{
			assert(false); // Not supported
		}
	}

	void TgaImage::_ReadImageID(u8* RawData)
	{
		_ImageData.ImageID = new u8[_ImageHeader.IDLength];
		memcpy(_ImageData.ImageID, RawData, sizeof(u8) * _ImageHeader.IDLength);
	}

	void TgaImage::_ReadDirectImageData(u8* RawData)
	{
		if (_IsUncompressed())
		{
			u32 PixelSize = _ImageHeader.ImageSpec.Depth / 8;
			u32 Width = _ImageHeader.ImageSpec.Width;
			u32 Height = _ImageHeader.ImageSpec.Height;
			u32 ImageSize = PixelSize * Width * Height;

			_ImageData.ImageData = new u8[Width * Height * 4];

			switch (PixelSize)
			{
			case 4:
				memcpy(_ImageData.ImageData, RawData, ImageSize);
				break;
			case 3:
			{
				for (u32 Y = 0; Y < Height; ++Y)
				{
					u32 Line = Y * Width;
					for (u32 X = 0; X < Width; ++X)
					{
						memcpy(&_ImageData.ImageData[(Line + X) * 4], &RawData[(Line + X) * PixelSize], PixelSize);
						_ImageData.ImageData[(Line + X) * 4 + 3] = 255;
					}
				}
				break;
			}
			default:
				assert(false); // Not supported
				break;
			}

			return;
		}
		
		if (_IsCompressed())
		{
			assert(false); // Not implemented
			
			return;
		}
	}
	void TgaImage::_ReadMappedImageData(u8* RawData)
	{
		assert(false); // Not implemented
	}

	bool TgaImage::_IsUncompressed() const
	{
		return _ImageHeader.ImageType == TGA_UNCOMPRESSED;
	}
	bool TgaImage::_IsCompressed() const
	{
		return _ImageHeader.ImageType == TGA_COMPRESSED;
	}
	bool TgaImage::_IsVerticalInverted() const
	{
		return !(_ImageHeader.ImageSpec.ImageDesc & TGA_VERTICAL_TOP_TO_BOTTOM);
	}
	bool TgaImage::_IsHorizontalInverted() const
	{
		return (_ImageHeader.ImageSpec.ImageDesc & TGA_HORIZONTAL_RIGHT_TO_LEFT) != 0;
	}
}
