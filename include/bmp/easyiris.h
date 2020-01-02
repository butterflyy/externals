#ifndef __BMP_EASY_IRIS_H__
#define __BMP_EASY_IRIS_H__

#include <bmp\readwrite.h>
#include <bmp\resize.h>

namespace bmp{
	//trans 640*480 or 768*768 bmp to encode bmp. need resize
	static void TransEncodeBmpStream(const unsigned char *src, int src_width, int src_height,
		unsigned char *des, int des_width, int des_height, unsigned char fillcolor = 0x0, float scale = 1.25){
		if (scale == 1){
			CopyMiddle(src, src_width, src_height, des, des_width, des_height, fillcolor);
		}
		else{
			int scale_width = static_cast<int>(src_width / scale);
			int scale_height = static_cast<int>(src_height / scale);
			utils::Buffer scaleBuf(scale_width * scale_height);
			Resize(src, scaleBuf.data(), src_width, src_height, scale);
			CopyMiddle(scaleBuf.data(), scale_width, scale_height, des, des_width, des_height, fillcolor);
		}

	}

	/*
	* @brief Trans file to encode file, need resize
	* @param src src file
	* @param des des file 768*768 encode file
	* @return less than 0 failed, others des file size succeeded
	*/
	static int TransEncodeBmp(const std::string& src, const std::string& des, unsigned char fillcolor = 0x0, float scale = 1.25){
		int width = 768;
		int height = 768;
		utils::Buffer srcbuf(width*height);

		static bmp::RgbQuad rgb[256];
		int size = bmp::ReadData(src, srcbuf.data(), srcbuf.size(), width, height, rgb);
		if (size < 0){
			return size;
		}

		int type = bmp::CheckTableType(rgb);
		if (type == 0){
			;//ok
		}
		else if (type == 1){
			//need trand color.
			utils::Buffer tmpsrcbuff(srcbuf);

			for (size_t i = 0; i < tmpsrcbuff.size(); i++){
				//color
				bmp::RgbQuad oldcolor = rgb[tmpsrcbuff[i]];
				int newcolorindex = -1;
				for (int j = 0; j < 256; j++){
					if (oldcolor.Red == j &&
						oldcolor.Green == j &&
						oldcolor.Blue == j){
						newcolorindex = j;
						break;
					}
				}
				if (newcolorindex == -1){
					assert(false);
				}
				//new color index
				srcbuf[i] = newcolorindex;
			}
		}
		else{
			assert(false);
			return -5;
		}

		int des_width = 768;
		int des_height = 768;
		utils::Buffer desbuf(des_width * des_height);

		TransEncodeBmpStream(srcbuf.data(), width, height, desbuf.data(), des_width, des_height, fillcolor, scale);

		return bmp::WriteData(des, desbuf.data(), des_width, des_height);
	}
}//!namespace

#endif //!__BMP_EASY_IRIS_H__