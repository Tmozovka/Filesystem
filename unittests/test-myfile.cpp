#include "catch.hpp"

#include "helper.hpp"
#include "myfs.h"

#include "MyFile.h"

TEST_CASE( "Compare File// Getter- and Settermethods", "[MyFile]" ) {

	//printf("START MYFILE TEST \n");
	MyFile* myfile = new MyFile("myfile.txt", getuid(), getgid(), 0, NULL,
			time(NULL), time(NULL), time(NULL), 2);

	SECTION("Compare File"){
	MyFile* similarFile = new MyFile("myfile.txt",getuid(),getgid(),0,NULL,time(NULL),time(NULL),time(NULL),2);

	REQUIRE(*myfile == *similarFile);
	delete similarFile;

}

	delete myfile;


	SECTION("Write File in Block using char *"){
		MyFile * tryFile = new MyFile("newfile.txt",2000,2000,12345,12345,1111111,111111111,11111111,60000);

		//printf("START TO WRITE IN BLOCK \n");

		char* readBuf = new char [BLOCK_SIZE];
		readBuf = tryFile->writeBlock();
		MyFile * newFile = new MyFile(readBuf);
		REQUIRE(*newFile == *tryFile);

		//remove("containerFileTest.bin");
		delete [] readBuf; //?? valdrid leaks
		delete tryFile;
		delete newFile;
	//	delete blocks;


	}

	SECTION("Read File in Block"){
		MyFile * tryFile = new MyFile("testfile.txt",199,991,12567,34512,1111101,110111111,11001111,50000);


		char * buf = new char [512];
		char * readBuf = new char [512];
		tryFile->writeFileChar(buf);

		printf("buf mit tryFile: %s \n", buf);
		printf("write to block \n");

		BlockDevice  blocks ;

		printf("START TO READ IN BLOCK \n");
		blocks.create("containerFileTest2.bin");
		blocks.write(0,buf);
		tryFile->readBlock(&readBuf,0,blocks);

		printf("buf aus BlockDevice %s \n", readBuf);

		REQUIRE(strcmp(buf, readBuf)==0);
		MyFile * newFile = new MyFile(readBuf);
		REQUIRE(*newFile == *tryFile);

		remove("containerFileTest2.bin");
		delete tryFile;
		delete newFile;
	//	delete blocks;
		delete [] readBuf;
		delete [] buf;

	}
}


