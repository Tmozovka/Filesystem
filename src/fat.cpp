


#include "fat.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;


void MyFAT::showFat(int start, int end) {
	printf(
			"****************************************************************\n");
	printf("FAT: \n");
	for (int i = start; i != end; i++) {

		if((i+1)!=table[i])
		printf("%i -> %i \n", i, table[i]);
	}
}

int MyFAT::unLink(int current) { //todo fehle behandeln + header bearbeiten
	if (current >= size || current < 0)
		return -1;
	else
		table[current] = -1;
	return 0;

}

int MyFAT::link(int current, int* next) {

	if (*next >= size || *next < 0 || current >= size || current < 0)
		return -1;
	else
		table[current] = *next;
	return 0;
}

int MyFAT::getNext(int current, int*next) {
	if (*next >= size || *next < 0 || current >= size || current < 0)
		return -1;
	else
		*next = table[current];
	return 0;
}

MyFAT::MyFAT() {

	for (int i = 0; i < BLOCKS_START; i++)
		table[i] = -2;

	for (int i = BLOCKS_START; i < size; i++)
		table[i] = -1;

	//printf("Konstruktor von MyFat ist beendet \n");

}

MyFAT::~MyFAT() {
	for (int i = 0; i < size; i++)
		table[i] = -1;

	//printf("Destruktor von MyFat ist beendet \n");
}

char * MyFAT::writeBlock() {
	char * result = new char[size * 6];

	strcpy(result, to_string(this->table[0]).c_str());
	strcat(result, "_");

	for (int i = 1; i < size; i++) {

		char * next = new char[5];
		strcpy(next, to_string(this->table[i]).c_str());
		strcat(result, next);
		strcat(result, "_");
		delete[] next;

	}

	return result;
}

void MyFAT::read(int start, BlockDevice * blocks) {
	printf("start MyFAT(BlockDevice * blocks, int start) start : %i \n", start);
	char * buf = new char[BLOCK_NUMBER * 6];
	this->readBlockDevice(blocks, start, buf);

	//printf("readBlockDevice fat : %s \n", buf);
	//printf("readBlockDevice fat end of first Block: %s \n", buf + 510);
	//printf("readBlockDevice fat end of second Block: %s \n", buf + 510 * 2);

//MyFAT::MyFAT(char * buf)
	for (int i = 0; i < size; i++) {

		char * next = new char[5];
		for (int j = 0; *buf != '_'; j++, buf++) {
			next[j] = *buf;
		}
		buf++;
		table[i] = atoi(next);
		delete[] next;
	}
}



MyFAT::MyFAT(BlockDevice * blocks, int start) {
	printf("start MyFAT(BlockDevice * blocks, int start) start : %i \n", start);
	char * buf = new char[BLOCK_NUMBER * 6];
	this->readBlockDevice(blocks, start, buf);

	//printf("readBlockDevice fat : %s \n", buf);
	//printf("readBlockDevice fat end of first Block: %s \n", buf + 510);
	//printf("readBlockDevice fat end of second Block: %s \n", buf + 510 * 2);

//MyFAT::MyFAT(char * buf)
	for (int i = 0; i < size; i++) {

		char * next = new char[5];
		for (int j = 0; *buf != '_'; j++, buf++) {
			next[j] = *buf;
		}
		buf++;
		table[i] = atoi(next);
		delete[] next;
	}
}

/*MyFAT::MyFAT(char * buf) {

 for (int i = 0; i < size; i++) {

 char * next = new char[5];
 for(int j=0;*buf!='_'; j++, buf++)
 {
 next[j]=*buf;
 }
 buf++;
 table[i] = atoi(next);
 delete[] next;
 }

 }*/
bool operator !=(MyFAT const &f1, MyFAT const& f2) {
	return compare(f1, f2) == -1;
}
bool operator ==(MyFAT const &f1, MyFAT const& f2) {
	return compare(f1, f2) == 0;
}

int compare(MyFAT f1, MyFAT f2) {
	for (int i = 0; i < f1.getSize(); i++) {
		if (f1.table[i] != f2.table[i])
			return -1;
	}

	return 0;
}

void MyFAT::writeBlockDevice(BlockDevice * blocks, int start) {
	printf("start for fat writeBlockDevice  start : %i \n", start);
	int nrBlocks = 0;
	char * buf;
	buf = this->writeBlock();
	//printf("fat: %s \n", buf);

	int firstBlock = start;
	start++;

	while (*buf != '\0') {
		char * writeBuf = new char[BLOCK_SIZE];
		//	char * startWriteBuf = writeBuf;

		int i = 0;
		for (; i < BLOCK_SIZE - 1; i++) {
			writeBuf[i] = *buf;
			buf++;
		}
		writeBuf[i] = char(0);

		//writeBuf=buf;
		blocks->write(start++, writeBuf);
		(nrBlocks)++;
		//buf+=BLOCK_SIZE;

		//writeBuf = startWriteBuf;

		delete[] writeBuf;
	}

	char * temp = new char[BLOCK_SIZE];

	strcpy(temp, to_string(nrBlocks).c_str());
	temp[to_string(nrBlocks).length()] = char(0);
	//resize(buf, to_string(this->sizeRoot).length(), BLOCK_SIZE);

	printf("nrBlocks write in %i : %s \n", firstBlock, temp);
	blocks->write(firstBlock, temp);

	delete[] temp;
}

void MyFAT::readBlockDevice(BlockDevice * blocks, int start, char * newBuf) {
	char * readBuf = new char[512];

	blocks->read(start, readBuf);
	int j = atoi(readBuf);
	printf("nrBlocks read from %i : %i \n", start, j);
	start++;

	int i = start;
	while (j != 0) {
		blocks->read(i++, readBuf);

		strcat(newBuf, readBuf);
		j--;

	}

	delete[] readBuf;
}

void MyFAT::resize(char * text, int oldSize, int newSize) {

	int i = newSize - oldSize;
	text += oldSize - 1;
	while (i != 0) {
		*(text++) = char(0);
		i--;
	}
	//*(text-1)='\0';
	text -= newSize;

}
/*char * MyFAT::BlockIntoBuffer(BlockDevice fatBlocks) {

 for (int i = 0; i < size; i++) {
 for (int j = 0; j < size; j++) {
 * buf[i]= * *fatBlocks[i];
 i++;
 j++;
 }
 }
 return * buf;
 }

 MyFAT::pufferIntoBlock(char * buf) {

 BlockDevice fatBlocks = new BlockDevice;

 for (int i = 0; i < size; i++) {
 for (int j = 0; j < size; j++) {
 *fatBlocks[i]= * buf[j];
 i++;
 j++;
 }
 }
 return fatBlocks;
 }*/
