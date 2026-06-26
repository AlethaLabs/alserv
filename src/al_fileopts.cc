#include <filesystem>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

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