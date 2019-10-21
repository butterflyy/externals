#ifndef __BMP_READ_WRITE_H__
#define __BMP_READ_WRITE_H__

#include <common/utils.h>

#pragma pack (push, 1) 

namespace bmp{
	//文件信息头
	struct BitMapFileHeader
	{
		unsigned char   bfType[2];            //文件类型
		unsigned int   bfSize;               //位图大小
		unsigned short  bfReserved1;          //位0 
		unsigned short  bfReserved2;          //位0
		unsigned int   bfOffBits;            //到数据偏移量
	};

	//信息头结构体
	struct BitMapInfoHeader
	{
		unsigned int     biSize;                  // BitMapFileHeader 字节数
		int              biWidth;                 //位图宽度 
		int              biHeight;                //位图高度，正位正向，反之为倒图 
		unsigned short    biPlanes;                //为目标设备说明位面数，其值将总是被设为1
		unsigned short    biBitCount;              //说明比特数/象素，为1、4、8、16、24、或32。 
		unsigned int     biCompression;           //图象数据压缩的类型没有压缩的类型：BI_RGB 
		unsigned int     biSizeImage;             //说明图象的大小，以字节为单位 
		int              biXPelsPerMeter;         //说明水平分辨率 
		int              biYPelsPerMeter;         //说明垂直分辨率 
		unsigned int     biClrUsed;               //说明位图实际使用的彩色表中的颜色索引数
		unsigned int     biClrImportant;          //对图象显示有重要影响的索引数，0都重要。 
	};

	//像素点结构体
	struct RgbQuad
	{
		unsigned char Blue;     //该颜色的蓝色分量 
		unsigned char Green;    //该颜色的绿色分量 
		unsigned char Red;      //该颜色的红色分量 
		unsigned char Reserved; //保留值（亮度）   
	};


	/*
	* @brief Read 8 bit index bmp info
	* @param filebuf file butter
	* @param size file size
	* @param width bmp file width
	* @param height bmp file height
	* @return less than 0 failed, others file size succeeded
	*/
	static int ReadInfoStream(const byte* filebuf, int size, int& width, int& height){
		int headsize = sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad);
		if (size < headsize){
			return -1;
		}

		BitMapFileHeader* pFileHead = (BitMapFileHeader*)filebuf;
		if (memcmp(pFileHead->bfType, "BM", 2)){//is not bmp
			return -2;
		}

		BitMapInfoHeader* pInfoHead = (BitMapInfoHeader*)(filebuf + sizeof(BitMapFileHeader));
		if (pInfoHead->biBitCount != 8){//is not 3bit index bmp
			return -3;
		}

		width = pInfoHead->biWidth;
		if (pInfoHead->biHeight > 0){
			height = pInfoHead->biHeight;
		}
		else{
			height = 0 - pInfoHead->biHeight;
		}

		assert(pFileHead->bfOffBits == sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad));

		assert(pFileHead->bfOffBits + width * height == pFileHead->bfSize);

		return pFileHead->bfOffBits + width * height;
	}


	/*
	* @brief Read 8 bit index bmp info
	* @param path file path
	* @param width bmp file width
	* @param height bmp file height
	* @return less than 0 failed, others file size succeeded
	*/
	static int ReadInfo(const std::string& path, int& width, int& height){
		int headsize = sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad);

		utils::Buffer info(headsize);
		int size = utils::ReadFile(path, info);
		if (size != headsize){
			return -1;
		}

		return ReadInfoStream(info.data(), (int)info.size(), width, height);
	}


	/*
	* @brief Check table type
	* @param table table buffer 
	* @return 0 defualt; 1 no stantard table; 2 no gray table
	*/
	static int CheckTableType(const RgbQuad* table){
		//stantard table
		int tableSize = 256 * sizeof(RgbQuad);
		utils::Buffer tableBuf(tableSize);
		RgbQuad* rgb = (RgbQuad*)tableBuf.data();
		for (int i = 0; i < 256; i++){
			rgb[i].Red = i;
			rgb[i].Green = i;
			rgb[i].Blue = i;
			rgb[i].Reserved = 255;
		}

		bool isdefault = true;
		for (int i = 0; i < 256; i++){
			if (!(table[i].Red == rgb[i].Red &&
				table[i].Green == rgb[i].Green &&
				table[i].Blue == rgb[i].Blue)){
				isdefault = false;
				break;
			}
		}

		if (isdefault){
			return 0;
		}

		//check no gray table
		for (int i = 0; i < 256; i++){
			bool find(false);
			for (int j = 0; j < 256; j++){
				if (table[i].Red == rgb[j].Red &&
					table[i].Green == rgb[j].Green &&
					table[i].Blue == rgb[j].Blue){
					find = true;
					break;
				}
			}

			if (!find){
				return 2;
			}
		}

		return 1;
	}

	/*
	* @brief Read 8 bit index bmp data
	* @param filebuf file butter
	* @param filesize file size
	* @param data data buffer
	* @param size data buffer
	* @param width bmp file width
	* @param height bmp file height
	* @param table table buffer 
	* @return less than 0 failed, others file size succeeded
	*/
	static int ReadDataStream(const byte* filebuf, int filesize, byte* buf, int size, int* width, int* height, RgbQuad* table = nullptr){
		int headsize = sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad);

		if (filesize <= headsize){
			return -1;
		}

		BitMapFileHeader* pFileHead = (BitMapFileHeader*)filebuf;
		if (memcmp(pFileHead->bfType, "BM", 2)){//is not bmp
			return -2;
		}

		BitMapInfoHeader* pInfoHead = (BitMapInfoHeader*)(filebuf + sizeof(BitMapFileHeader));
		if (pInfoHead->biBitCount != 8){//is not 3bit index bmp
			return -3;
		}


		if (table){
			//check color table
			int tableSize = 256 * sizeof(RgbQuad);
			RgbQuad* rgb = (RgbQuad*)(filebuf + sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader));

			memcpy(table, rgb, tableSize);
		}

		bool isReverse = (pInfoHead->biHeight < 0);
		const byte* pData = filebuf + headsize;

		*width = pInfoHead->biWidth;
		*height = isReverse ? (0 - 0 - pInfoHead->biHeight) : pInfoHead->biHeight;

		int bufsize = (*width) * (*height);
		if (size < bufsize){
			return -1;
		}

		if (isReverse){
			memcpy(buf, pData, bufsize);
		}
		else{

			for (int j = 0; j < *height; j++)
			for (int i = 0; i < *width; i++)
			{
				buf[j*(*width) + i] = pData[(*height - j - 1)*(*width) + i];
			}
		}

		assert(filesize == pFileHead->bfSize);

		return filesize;
	}

	/*
	* @brief Read 8 bit index bmp data
	* @param path file path
	* @param data data buffer
	* @param size data size
	* @param width bmp file width
	* @param height bmp file height
	* @param table table buffer
	* @return less than 0 failed, others file size succeeded
	*/
	static int ReadData(const std::string& path, byte* buf, int size, int* width, int* height, RgbQuad* table = nullptr){
		byte* filebuff = nullptr;
		int filesize = utils::ReadFile(path, &filebuff);
		if (filesize < 0){
			SAFE_DELETE_ARRAY(filebuff);
			return -1;
		}

		int ret = ReadDataStream(filebuff, filesize, buf, size, width, height, table);
		SAFE_DELETE_ARRAY(filebuff);
		return ret;
	}

	/*
	* @brief Write 8 bit index bmp data
	* @param filebuf file buffer
	* @param sized file buffer size
	* @param buf data buffer
	* @param width bmp file width
	* @param height bmp file height
	* @return less than 0 failed, others file size succeeded
	*/
	static int WriteDataStream(byte* filebuf, int size, const byte* buf, int width, int height, const RgbQuad* table = nullptr){
		int headsize = sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad);
		int datasize = width * height;

		if (size < headsize + datasize){
			return -1;
		}

		BitMapFileHeader* FileHead = (BitMapFileHeader*)filebuf;
		memcpy(FileHead->bfType, "BM", 2);
		FileHead->bfOffBits = headsize;
		FileHead->bfSize = headsize + datasize;
		FileHead->bfReserved1 = 0;
		FileHead->bfReserved2 = 0;

		int offset = sizeof(BitMapFileHeader);
		BitMapInfoHeader* InfoHead = (BitMapInfoHeader*)(filebuf + offset);
		InfoHead->biSize = 40;
		InfoHead->biWidth = width;
		InfoHead->biHeight = height;
		InfoHead->biPlanes = 1;
		InfoHead->biBitCount = 8;
		InfoHead->biCompression = 0;
		InfoHead->biSizeImage = datasize;
		InfoHead->biXPelsPerMeter = 3780;
		InfoHead->biYPelsPerMeter = 3780;
		InfoHead->biClrUsed = 256;
		InfoHead->biClrImportant = 256;

		offset += sizeof(BitMapInfoHeader);
		RgbQuad* rgb = (RgbQuad*)(filebuf + offset);

		if (table){
			memcpy(rgb, table, 256 * sizeof(RgbQuad));
		}
		else{//default table
			for (int i = 0; i < 256; i++){
				rgb[i].Red = i;
				rgb[i].Green = i;
				rgb[i].Blue = i;
				rgb[i].Reserved = 255;
			}
		}


		offset += 256 * sizeof(RgbQuad);

		byte* image = filebuf + offset;
		for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			image[j*width + i] = buf[(height - j - 1)*width + i];
		}

		return headsize + datasize;
	}

	/*
	* @brief Write 8 bit index bmp data
	* @param path file path
	* @param buf data buffer
	* @param width bmp file width
	* @param height bmp file height
	* @return less than 0 failed, others file size succeeded
	*/
	static int WriteData(const std::string& path, const byte* buf, int width, int height, const RgbQuad* table = nullptr){
		int headsize = sizeof(BitMapFileHeader)+sizeof(BitMapInfoHeader)
			+256 * sizeof(RgbQuad);
		int datasize = width * height;

		utils::Buffer filedata(headsize + datasize);
		int ret = WriteDataStream(filedata.data(), (int)filedata.size(), buf, width, height, table);
		if (ret < 0){
			return ret;
		}

		return utils::WriteFile(path, filedata);
	}
	

}//!namespace


#pragma pack (pop) 

#endif //!__BMP_READ_WRITE_H__