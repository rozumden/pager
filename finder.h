#ifndef _FINDER_H
#define _FINDER_H

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

// const long long goal = 0x7211321f; 11721F32
const long long goal = 0x1f321172; 
const char *folder = "/proc/bus/pci";
const int verbose = 0;

struct device {
    long long address;
    char *path;
    int length;
    int found; 
};

void dfs(const char *dirname, struct device *d) {
    if (d->found) return;
    DIR *cur = opendir(dirname);
    struct dirent *dir; 
	long long *header = (long long *) malloc (8);
    if(cur != NULL) {
    	while (dir = readdir(cur)) {
    		if(!strcmp(dir->d_name,".") | !strcmp(dir->d_name,"..")) continue;
    		struct stat s;
    		char *full_dir = malloc(strlen(dirname)+strlen(dir->d_name)+2);
		    strcpy(full_dir, dirname);
		    strcat(full_dir, "/");
		    strcat(full_dir, dir->d_name);
    		stat(full_dir,&s);
    		if (S_ISREG(s.st_mode)) {
    			if (d->found) return;
    			if (verbose) fprintf(stdout, "File: %s\n", full_dir);
    			FILE *file = fopen(full_dir,"rb");
    			fread(header,5,1,file);
    			*header = *(header) & 0x0000FFFFFFFF;
    			if (verbose) fprintf(stdout, "Header: %p, Goal: %p\n", (void*) *header,(void*) goal);
    			fclose(file);
    			if(*header-goal == 0) {
    				if (verbose) fprintf(stdout,"File found on %s\n",full_dir);
    				d->found = 1;
    				d->path = full_dir;
    				return;
    			}
    		} else {
    			if (d->found) return;
    			if (verbose) fprintf(stdout, "Folder: %s\n", full_dir);
    			dfs(full_dir,d);
    		}
    	}
		closedir(cur);
    }
}

struct device findMe() {
    struct device d;
    d.found = 0;
    dfs(folder,&d);
    if (!d.found) perror("Cannot find device.");

    FILE * dev;
    dev = fopen(d.path, "rb");
    if (dev!=NULL) {
    	char buffer[4], temp[8];
		fseek(dev, 18, 0);
		fread(buffer, 4, 1, dev);
		sprintf(temp, "%02X%02X%02X%02X", (unsigned char) buffer[1],(unsigned char) buffer[0],(unsigned char) buffer[3],(unsigned char) buffer[2]);
	   	d.address = strtol(temp, NULL, 16);
	    fseek(dev, 0, SEEK_END);
	    d.length = ftell(dev);
	    fseek(dev, 0, SEEK_SET);
	} else {
		perror("Cannot open device.");
	}

    if (verbose) fprintf(stdout, "Found path: %s, address %x, length: %x\n",d.path,(unsigned int) d.address, d.length);
    return d;
}

#endif
