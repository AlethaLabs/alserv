#include <filesystem>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "../include/fileopts/al_fileopts.hh"

File::File(const std::filesystem::path &filepath) : fd_(::open(filepath.c_str(), O_RDONLY)) {
        if (fd_ == -1) {
                throw std::runtime_error(std::string("Failed to open file: \t") +
                                         strerror(errno));
        }
}

File::~File() {
        if (fd_ != -1) ::close(fd_);
}

File::File(File &&other) noexcept : fd_(std::exchange(other.fd_, -1)) {}

File& File::operator=(File&& other) noexcept {
        if (this != &other) {
                if (fd_ >= 0) ::close(fd_);
                fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
}

std::vector<uint8_t> File::read() {
        off_t file_size = lseek(fd_, 0, SEEK_END);
        if (file_size == -1)
                throw std::runtime_error(std::string("Failed to seek file: ") + strerror(errno));

        if (lseek(fd_, 0, SEEK_SET) == -1)
                throw std::runtime_error(std::string("Failed to seek file: ") + strerror(errno));

        file_data_.resize(static_cast<size_t>(file_size));

        off_t curr_offset = 0;
        while (curr_offset < file_size) {
                ssize_t n = ::read(fd_, file_data_.data() + curr_offset,
                                   static_cast<size_t>(file_size - curr_offset));
                if (n == -1)
                        throw std::runtime_error(std::string("Failed to read file: ") + strerror(errno));
                if (n == 0)
                        break;
                curr_offset += n;
        }
        return file_data_;
}