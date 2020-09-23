#ifndef XMEM_FILE_H
#define XMEM_FILE_H

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include <fcntl.h>
#include <unistd.h>

#include "exception.h"
#include "prelude.h"


namespace xmem {


struct FileDeleter {
    void operator()(FILE* ptr) const {
        std::fclose(ptr);
    }
};


class FilePtr : public std::unique_ptr<FILE, FileDeleter> {
private:
    using Super = std::unique_ptr<FILE, FileDeleter>;

public:
    explicit FilePtr(const char* path, const char* mode = "r")
        : Super(std::fopen(path, mode))
    {
        XMEM_ENSURE(get(), "Failed to open file: " << path);
    }

    FilePtr(const std::string& path, const char* mode = "r")
        : FilePtr(path.data(), mode)
    {}
};


class File {
public:
    explicit File(const char* path, int flags, mode_t mode = 0) {
        fd = open(path, flags, mode);
        XMEM_ENSURE(fd > -1, "Failed to open file: " << path);
    }

    explicit File(const std::string& path, int flags, mode_t mode = 0)
        : File(path.data(), flags, mode)
    {}

    File(const File& that) = delete;

    ~File() {
        close(fd);
    }

    int get() const {
        return fd;
    }

    void read(u64 pos, usize size, u8* data) const {
        do_read_write(
            "Read", pos, size,
            [fd = fd, data] (usize data_pos, usize size) {
                return ::read(fd, data + data_pos, size);
            }
        );
    }

    void write(u64 pos, usize size, const u8* data) const {
        do_read_write(
            "Write", pos, size,
            [fd = fd, data] (usize data_pos, usize size) {
                return ::write(fd, data + data_pos, size);
            }
        );
    }

    void sync_data() const {
#ifdef __linux__
        XMEM_ENSURE(
            fdatasync(fd) == 0,
            "Sync failed: " << std::strerror(errno)
        );
#elif defined(__APPLE__)
        XMEM_ENSURE(
            fcntl(fd, F_FULLFSYNC, 0) > -1,
            // fsync(fd) == 0,
            "Sync failed: " << std::strerror(errno)
        );
#else
        XMEM_ENSURE(false, "Sync failed: not implemented");
#endif
    }

private:
    template <typename F>
    void do_read_write(
        const char* msg,
        u64 pos, usize size,
        F read_write
    ) const {
        static_assert(sizeof(off_t) == 8, "only 64-bit off_t is supported");

        usize data_pos = 0;

        while (true) {
            XMEM_ENSURE(
                lseek(fd, static_cast<off_t>(pos), SEEK_SET) != static_cast<off_t>(-1),
                "Seek failed: " << std::strerror(errno)
            );

            const ssize_t rw_size = read_write(data_pos, size);

            if (rw_size < 0) {
                XMEM_ENSURE(
                    errno == EINTR,
                    msg << " failed: " << std::strerror(errno)
                );
            } else {
                pos += rw_size;
                size -= rw_size;
                data_pos += rw_size;

                if (size == 0) {
                    break;
                }

                XMEM_ENSURE(
                    rw_size > 0,
                    msg << " failed: EOF"
                );
            }
        }
    }

private:
    int fd;
};


}


#endif
