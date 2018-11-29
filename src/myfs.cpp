//
//  myfs.cpp
//  myfs
//
//  Created by Oliver Waldhorst on 02.08.17.
//  Copyright © 2017 Oliver Waldhorst. All rights reserved.
//

// For documentation of FUSE methods see https://libfuse.github.io/doxygen/structfuse__operations.html

#undef DEBUG

// TODO: Comment this to reduce debug messages
#define DEBUG
#define DEBUG_METHODS
#define DEBUG_RETURN_VALUES

#include "macros.h"

#include "myfs.h"
#include "myfs-info.h"

MyFS* MyFS::_instance = NULL;

MyFS* MyFS::Instance() {
    if(_instance == NULL) {
        _instance = new MyFS();
    }
    return _instance;
}

MyFS::MyFS() {
    this->logFile= stderr;
}

MyFS::~MyFS() {
    
}

//TODOs:

int MyFS::fuseGetattr(const char *path, struct stat *st) {
    LOGM();
    //TODO
	printf("[getattr] Called\n");
	printf("\tAttributes of %s requested\n", path);

	myFile fcopy;
	if(root.getFile(path,&fcopy)==-1)
		RETURN(-1); //TODO: richtigen Fehlecode zurueckgeben

	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
	st->st_mtime = fcopy.lastMod; // The last "m"odification of the file/directory is right now
	st->st_ctime = time(NULL);

	if (strcmp(path, "/") == 0)
	{
		st->st_mode = S_IFDIR | 0444;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
		//size = sum size all files in dir
	}
	else
	{

		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		st->st_size = fcopy.size; //Tanja's comments: What is file's size???

	}


    RETURN(0);
}

int MyFS::fuseReadlink(const char *path, char *link, size_t size) {
    LOGM();
    return 0;
}


int MyFS::fuseMknod(const char *path, mode_t mode, dev_t dev) { //??? wir brauchen das nicht
    LOGM();
    
    // TODO: Implement this!
	// Tanja's comments:
    // Inode contains : number of blocks, file's name, mode, userId, groupId, size, number of links, ctime, atime, mtime
    // get Inode's attributes with  MyFS::fuseGetattr(const char *path, struct stat *st)
	// How to create file?
	// change Inode
	// change D-Map
	// describe new Block(s)
	// How to create Inode?
	// add new file to the Root
	// Information about the file is saved in the objekt File 
	// dev_t dev muss ID von unsere file system sein
	root.addFile(path, 512, S_IFREG | 0444);

	RETURN(0);
}


int MyFS::fuseUnlink(const char *path) {
    LOGM();
    
    // TODO: Implement this!
	// Tanja's comments:
	//How to unlink file?
	//1 delete file from root 
	//2 set bloks unused (change FAT, this changes are executed in Dmap.setUnused funktion)
	
	myFile fcopy;
	root.getFile(path, &fcopy);

	root.deleteFile(path);
	
	for (int current = fcopy.firstBlock;
		current != null; current = next)
	{
		int next = _fat.table[current]
		dMap.setUnused(current); // Changes in Fat : unLink(current)
	}
		
    
    RETURN(0);
}


int MyFS::fuseOpen(const char *path, struct fuse_file_info *fileInfo) { // How to open file from hier?
    LOGM();

    // Wofür braucht man  fuse_file_info *fileInfo
    // TODO: Implement this!
	/*string line;
	ifstream myfile(path);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			cout << line << '\n';
		}
		myfile.close();
		RETURN(0);
	}

	else cout << "Unable to open file";*/
	if(blocks.open(path)!=0) return -1;

	RETURN(0);
    //RETURN(-1);
}

int MyFS::fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
	if (offset > size) // not possible
		RETURN(-1);
	char * buffer;
	
	myFile fcopy = root.getFile(name);
	int blocksNumber = ceil(fcopy.size / BD_BLOCK_SIZE);
	int currentBlock = fcopy.firstBlock;
	while (currentBlock != 0)
	{
		if( blocks.read(currentBlock, buffer)==0)
		{
			while (*buffer != NULL)
				*(++buf) = *(buffer++);
		}
		else
		{
		//mistake
			RETURN(-1);
		}
		currentBlock = fat.table[currentBlock];

		
	}
	
	RETURN(0);
	
	/*


	printf("--> Trying to read %s, %u, %u\n", path, offset, size);
	string line;
	ifstream myfile(path);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			line.copy(buf, offset, size - offset);
			*(++buf) = '\n';
		}
		myfile.close();
		RETURN(0);
	}*/
}

int MyFS::fuseWrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this! // Schould we do this in first part?
    
    RETURN(0);
}

int MyFS::fuseRelease(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    //temporeres Zeug loeschen
    
    RETURN(0);
}

int MyFS::fuseOpendir(const char *path, struct fuse_file_info *fileInfo) { // Is it not the same as fuseReaddir?
    LOGM();
    
    // TODO: Implement this!
    
    RETURN(0);
}

int MyFS::fuseReaddir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!

	printf("--> Getting The List of Files of %s\n", path);

	filler(buffer, ".", NULL, 0); // Current Directory
	filler(buffer, "..", NULL, 0); // Parent Directory

	if (strcmp(path, "/") == 0) // If the user is trying to show the files/directories of the root directory show the following
	{
		for(int i=1;_root.files[i]!=null;i++)
		filler(buffer, root.files[i].name, NULL, 0);
	}
	else
	{
		//such path doesn't exist
		RETURN(-1);
	}


    
    RETURN(0);
    
    // <<< My new code
}

int MyFS::fuseReleasedir(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    
    // TODO: Implement this!
    //temporeres Zeug loeschen
    
    RETURN(0);
}

int MyFS::fuseCreate(const char *path, mode_t mode, struct fuse_file_info *fileInfo) { // Was wird es hier gemacht? Sien hier fat dmap superblock und root erstellt?
    LOGM();
    
    // TODO: Implement this!

	//Tanja's comments
	//
	//
	//
    
    RETURN(0);
}

void* MyFS::fuseInit(struct fuse_conn_info *conn) { // What schould we do hier? What is the different between fuseCreate
    // Open logfile
    this->logFile= fopen(((MyFsInfo *) fuse_get_context()->private_data)->logFile, "w+");
    if(this->logFile == NULL) {
        fprintf(stderr, "ERROR: Cannot open logfile %s\n", ((MyFsInfo *) fuse_get_context()->private_data)->logFile);
    } else {
        //    this->logFile= ((MyFsInfo *) fuse_get_context()->private_data)->logFile;
        
        // turn of logfile buffering
        setvbuf(this->logFile, NULL, _IOLBF, 0);
        
        LOG("Starting logging...\n");
        LOGM();
        
        // you can get the containfer file name here:
        LOGF("Container file name: %s", ((MyFsInfo *) fuse_get_context()->private_data)->contFile);
        
        // TODO: Implement your initialization methods here!
        // Konstruktor hier (vermuetlich) schreiben
    }
    
    RETURN(0);
}

// int fuseCreate(const char *, mode_t, struct fuse_file_info *);
void MyFS::addFile(const char * name, mode_t mode, off_t size)
{
	int blocksNumber = ceil(size / BD_BLOCK_SIZE);
	int*  blocks = new int[blocksNumber+1];
	blocks[blocksNumber + 1] = 0;
	if (dMap.getFreeBlocks(blocksNumber, &blocks) == 0)
	{
		root.addFile(name, size, mode);
		for (int i = 1; i <= blocksNumber; i++)
		{
			dMap.setUsed(i);
			fat.link(blocks[i], blocks[i+1]);

			char *buffer; // wofuer brauchen wir buffer hier
		    if( write(i, buffer)!=0)
		    {
		    	//fehlermeldung
		    }
		}
	}
	else
	{
		//no more place
	}
}

//int fuseUnlink(const char *path);
void MyFS::deleteFile(const char *name)
{
	myFile fcopy;
	root.getFile(name, &fcopy);
	root.deleteFile(name);
	int blocksNumber = ceil(fcopy.size / BD_BLOCK_SIZE);
	int currentBlock = fcopy.firstBlock;
	while (currentBlock != 0)
	{
		dmap.setUnused(currentBlock);
		currentBlock = fat.table[currentBlock];

	}


}

//int fuseRead(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int MyFS::readFile(const char *name, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
	return fuseRead(path, buf,  size,  offset, fileInfo);
}

//////////////////////////Todos end////////////////////////////////////////////////////////////////////

int MyFS::fuseMkdir(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseRmdir(const char *path) {
    LOGM();
    return 0;
}

int MyFS::fuseSymlink(const char *path, const char *link) {
    LOGM();
    return 0;
}

int MyFS::fuseRename(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseLink(const char *path, const char *newpath) {
    LOGM();
    return 0;
}

int MyFS::fuseChmod(const char *path, mode_t mode) {
    LOGM();
    return 0;
}

int MyFS::fuseChown(const char *path, uid_t uid, gid_t gid) {
    LOGM();
    return 0;
}

int MyFS::fuseTruncate(const char *path, off_t newSize) {
    LOGM();
    return 0;
}

int MyFS::fuseUtime(const char *path, struct utimbuf *ubuf) {
    LOGM();
    return 0;
}

int MyFS::fuseStatfs(const char *path, struct statvfs *statInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseFlush(const char *path, struct fuse_file_info *fileInfo) {
    LOGM();
    return 0;
}

int MyFS::fuseFsync(const char *path, int datasync, struct fuse_file_info *fi) {
    LOGM();
    return 0;
}

int MyFS::fuseListxattr(const char *path, char *list, size_t size) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseRemovexattr(const char *path, const char *name) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseFsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

int MyFS::fuseTruncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    LOGM();
    RETURN(0);
}

void MyFS::fuseDestroy() {
    LOGM();
}


#ifdef __APPLE__
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags, uint32_t x) {
#else
int MyFS::fuseSetxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
#endif
    LOGM();
    RETURN(0);
}

#ifdef __APPLE__
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size, uint x) {
#else
int MyFS::fuseGetxattr(const char *path, const char *name, char *value, size_t size) {
#endif
    LOGM();
    RETURN(0);
}



