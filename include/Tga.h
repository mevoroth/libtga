#ifndef _TGA_H_
#define _TGA_H_

namespace Tga
{
	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef unsigned long long u64;

	const u8 TGA_UNCOMPRESSED	= 2;
	const u8 TGA_GREYSCALE		= 3;
	const u8 TGA_COMPRESSED		= 10;
	const u8 TGA_VERTICAL_TOP_TO_BOTTOM = (1 << 4);
	const u8 TGA_HORIZONTAL_RIGHT_TO_LEFT = (1 << 5);

	struct ColorMapSpec
	{
		u16 Origin = 0;
		u16 Length = 0;
		u8 PixelSize = 0;
	};

	struct ImageSpec
	{
		u16 XOrigin = 0;
		u16 YOrigin = 0;
		u16 Width = 0;
		u16 Height = 0;
		u8 Depth = 0;
		u8 ImageDesc = 0;
	};

	struct Header
	{
		u8 IDLength = 0;
		u8 ColorMapType = 0;
		u8 ImageType = 0;
		ColorMapSpec ColorMapSpec;
		ImageSpec ImageSpec;
	};

	struct Data
	{
		u8* ImageID;
		u8* ColorMapData;
		/**
		 * BGRA block
		 */
		u8* ImageData;
	};

	class TgaImage
	{
	public:
		/**
		 * @param[in] Extern buffer
		 * @param[in] Size of extern buffer
		 */
		TgaImage(u8* RawData, u64 Size);
		~TgaImage();
		/**
		 * Get Image data
		 * @param[out] Width
		 * @param[out] Height
		 * @return Image Data (BGRA)
		 */
		u8* GetImage(u32& Width, u32& Height);

	private:
		/**
		 * Generic read buffer
		 * @param[out] Intern buffer
		 * @param[in] Extern buffer
		 * @param[in] Size of chunk to read
		 * @param[inout] Current offset after read
		 */
		void _Read(u8* Dst, const u8* Src, u32 Size, u32& ReadOffset);
		/**
		 * Read Header block
		 * @param[out] Intern buffer
		 * @param[inout] Current offset after read
		 */
		void _ReadHeader(u8* RawData, u32& ReadOffset);
		/**
		 * Read Image data
		 * @param[in] Extern buffer
		 */
		void _ReadData(u8* RawData);
		/**
		 * Read Image ID
		 * @param[in] Extern buffer
		 */
		void _ReadImageID(u8* RawData);
		/**
		 * Read Mapped Image Data
		 * @param[in] Extern buffer
		 */
		void _ReadMappedImageData(u8* RawData);
		/**
		 * Read Direct Image Data
		 * @param[in] Extern buffer
		 */
		void _ReadDirectImageData(u8* RawData);
		/**
		 * @return If TGA file is uncompressed
		 */
		bool _IsUncompressed() const;
		/**
		 * @return If TGA file is compressed
		 */
		bool _IsCompressed() const;
		/**
		 * @return If TGA file is greyscale
		 */
		bool _IsGreyScale() const;
		
		bool _IsVerticalInverted() const;
		bool _IsHorizontalInverted() const;

		Header _ImageHeader;
		Data _ImageData;
	};
}

#endif
