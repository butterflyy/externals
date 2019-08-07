#ifndef __BMP_RESIZE_H__
#define __BMP_RESIZE_H__

namespace bmp{
	static void Resize(const unsigned char *src, unsigned char *des, int width, int height, float scale = 1.25) {
		int dw = 0;
		int dh = 0;
		int i = 0, j = 0;
		float x = 0, y = 0;
		float ss = 0;
		int x11 = 0, y11 = 0, x12 = 0, y12 = 0;
		float u = 0, v = 0;

		dw = int((float)width / scale);
		dh = int((float)height / scale);

		for (i = 0; i < dh; i++)
		{
			y = (float)(i + 0.5)*scale - 0.5;
			if (y < 0)
			{
				y = (float)i*scale;
			}
			y11 = floor(y);
			y12 = y11 + 1;

			u = y - y11;
			for (j = 0; j < dw; j++)
			{
				x = (float)(j + 0.5)*scale - 0.5;
				if (x < 0)
				{
					x = (float)j*scale;
				}
				x11 = floor(x);
				x12 = x11 + 1;

				v = x - x11;
				*(des + i*dw + j) = (1 - u)*(1 - v)*(*(src + y11*width + x11)) + u*(1 - v)*(*(src + y12*width + x11))
					+ v*(1 - u)*(*(src + y11*width + x12)) + u*v*(*(src + y12*width + x12));
			}
		}
	}

	//src (small image), des(big image 640*480 or 768*768)
	static void CopyMiddle(const unsigned char *src, int src_width, int src_height,
		unsigned char *des, int des_width, int des_height, unsigned char fillcolor = 0x0/* or 0x7F*/){
		assert(src_width < des_width && src_height < des_height);

		memset(des, fillcolor, des_width*des_height);
		int less_width = (des_width - src_width) / 2;
		int less_height = (des_height - src_height) / 2;

		for (int i = less_width; i < des_width - less_width; i++){
			for (int j = less_height; j < des_height - less_height; j++){
				des[j * des_width + i] = src[(j - less_height) * src_width + (i - less_width)];
			}
		}
	}

}//!namespace

#endif //!__BMP_RESIZE_H__