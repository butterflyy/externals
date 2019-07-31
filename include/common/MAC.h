#ifndef __MAC_H__
#define __MAC_H__

#include <stdlib.h>
#include <time.h>
#include <string>

class MAC{
public:
	MAC();
	~MAC();

	void GenMAC(unsigned char mac[6]);

	//AA-BB-CC-DD-EE
	std::string GenMAC();
};


MAC::MAC(){
	srand((unsigned)time(NULL));
}

MAC::~MAC(){

}

void MAC::GenMAC(unsigned char mac[6]){
	for (int i = 0; i < 6; i++){
		mac[i] = rand() % 255;
		if (i == 0){
			mac[i] = mac[i] / 2 * 2;
		}
	}
}

//AA-BB-CC-DD-EE
std::string MAC::GenMAC(){
	unsigned char mac[6];
	GenMAC(mac);

	std::string strmac;
	for (int i = 0; i < 6; i++){
		char buff[3] = { 0 };
		sprintf(buff, "%02X", mac[i]);
		strmac += buff;
		if (i != 5){
			strmac += '-';
		}
	}

	return strmac;
}


#endif //!__MAC_H__