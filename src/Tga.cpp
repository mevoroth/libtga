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
	
	TgaImage::GetImage(u32& Width, u32& Height, u8* ImageData)
	{
		Width = _ImageHeader.ImageSpec.Width;
		Height = _ImageHeader.ImageSpec.Height;
		u32 PixelSize = _ImageHeader.ImageSpec.Depth / 8;
		memcpy(ImageData, _ImageData.ImageData, Width * Height * PixelSize);
	}

	void TgaImage::_Read(u8* Dst, const u8* Src, u32 Size, u32& ReadOffset)
	{
		memcpy(Dst, Src, Size);
		ReadOffset += Size;
	}

	void TgaImage::_ReadHeader(u8* RawData, u32& ReadOffset)
	{
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
			u32 LineSize = Width * PixelSize;

			_ImageData.ImageData = new u8[PixelSize * Width * Height];

			for (u32 Y = 0; Y < Height; ++Y)
			{
				u32 OffsetInMemory = Y * Width * PixelSize;
				u32 OffsetInFile = (Height - Y - 1) * Width * PixelSize;
				memcpy(&_ImageData.ImageData[OffsetInMemory], RawData + OffsetInFile, LineSize);
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
}
