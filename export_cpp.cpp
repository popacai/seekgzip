#include <string>
#include <stdio.h>
#include <stdexcept>
#include "seekgzip.h"
#include "export_cpp.h"

static std::string error_string(int errorcode)
{
    switch (errorcode) {
    case SEEKGZIP_SUCCESS:
        return "";
    case SEEKGZIP_OPENERROR:
        return "Failed to open a file";
    case SEEKGZIP_READERROR:
        return "Failed to read from a file";
    case SEEKGZIP_WRITEERROR:
        return "Failed to write to a file";
    case SEEKGZIP_DATAERROR:
        return "Data error";
    case SEEKGZIP_OUTOFMEMORY:
        return "Out of memory";
    case SEEKGZIP_IMCOMPATIBLE:
        return "Imcompatible data format";
    case SEEKGZIP_ZLIBERROR:
        return "ZLIB error";
    default:
    case SEEKGZIP_ERROR:
        return "Unknown error";
    }
}

reader::reader(const char *filename)
{
    int err = 0;
    seekgzip_t* sgz = seekgzip_open(filename, 0);
    m_obj = sgz;
    if ( (err = seekgzip_error(sgz)) != SEEKGZIP_SUCCESS){
        throw std::invalid_argument(error_string(err));
    }
    reader_buffer = new char[BUFFER_SIZE + 1];
    offset = BUFFER_SIZE + 1;
}

reader::~reader()
{
    delete [] reader_buffer;
    this->close();
}

void reader::close()
{
    if (m_obj != NULL) {
        seekgzip_close(reinterpret_cast<seekgzip_t*>(m_obj));
        m_obj = NULL;
    }
}

void reader::seek(long long offset)
{
    if (m_obj != NULL) {
        seekgzip_seek(
            reinterpret_cast<seekgzip_t*>(m_obj),
            offset
            );
    }
}

void reader::seek(long long offset, int whence)
{
    long long file_length;
    file_length = seekgzip_unpacked_length(reinterpret_cast<seekgzip_t*>(m_obj));

    //seek to the end
    if (whence == 2) {
        offset = file_length - offset;
    } else if (whence == 1) {
        offset = tell() + offset;
    } else if (whence == 0) {
        // pass 
    } else {
        throw std::invalid_argument(error_string(SEEKGZIP_ERROR));
    }
    if (offset < 0 || offset > file_length) {
        throw std::invalid_argument(error_string(SEEKGZIP_READERROR));
    }
    if (m_obj != NULL) {
        seekgzip_seek(
            reinterpret_cast<seekgzip_t*>(m_obj),
            offset
            );
    }
}


long long reader::tell()
{
    if (m_obj != NULL) {
        return seekgzip_tell(
            reinterpret_cast<seekgzip_t*>(m_obj)
            );
    } else {
        return -1;    
    }
}

std::string reader::read(int size)
{
    std::string ret;
    if (m_obj != NULL) {
        char *buffer = new char[size+1];
        int n = seekgzip_read(
            reinterpret_cast<seekgzip_t*>(m_obj),
            buffer,
            size
            );
        buffer[n] = 0;
        ret = buffer;
        delete[] buffer;
    }
    return ret;
}


std::string reader::readline()
{
    off_t old_offset;
    int found;
    std::string ret = "";
    while (1) {
        old_offset = offset;
        found = 0;
        for (; offset < BUFFER_SIZE; offset++) {
            if (reader_buffer[offset] == '\n') {
                found = 1;
                break;
            }
        }

        reader_buffer[offset] = 0;
        ret += (char*) (reader_buffer + old_offset);
        if (found) {
            offset++;
            return ret;
        } else {
            if (m_obj != NULL) {
                int n = seekgzip_read(
                    reinterpret_cast<seekgzip_t*>(m_obj),
                    reader_buffer,
                    BUFFER_SIZE 
                );
                if (n == 0) {
                    return ret;
                }
                reader_buffer[n] = 0;
                offset = 0; //reset offset
            }
        }
    }
}

