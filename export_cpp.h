#ifndef __EXPORT_H__
#define __EXPORT_H__

#include <string>

class reader
{
protected:
    void *m_obj;

public:
    reader(const char *filename);

    virtual ~reader();

    void close();

    void seek(long long offset);

    void seek(long long offset, int whence);

    long long tell();

    std::string read(int size);

    std::string readline();
};

#endif/*__EXPORT_H__*/




