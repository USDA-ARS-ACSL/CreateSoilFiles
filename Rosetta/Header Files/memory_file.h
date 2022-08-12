#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H

#include <cassert>

class Memory_File
{
 protected:
  size_t n;
  char* data;
  unsigned int index;

 public:
  Memory_File(char* bin, size_t size)
    {
      assert(bin);
      assert(size>0);
      data=bin;
      n=size;
      index=0;
    }

  size_t fread(char *ptr, size_t size, size_t nmemb)
  {
    assert(ptr);
    for (size_t i=0;i<nmemb;i++)
      {
	for (size_t j=0;j<size;j++)
	  {
	    if (index<n)
	      {
		ptr[i*size+j]=data[index];
		index++;
	      }
	    else
	      return(i);
	  }
      }
    return(nmemb);
  }
};

#endif
