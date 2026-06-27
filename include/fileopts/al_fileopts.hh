#include <filesystem>
#include <stdexcept>
#include <vector>
#include <unordered_map>

class File {
private:
        // --------- File descriptor object ---------
        int fd_;

        // --------- Huffman coding compression struct ---------
        struct HuffNode {
                int symbol;
                uint64_t frequency;
                std::unique_ptr<HuffNode> left;
                std::unique_ptr<HuffNode> right;

                HuffNode(int symbol, uint64_t frequency)
                    : symbol(symbol),
                      frequency(frequency),
                      left(nullptr),
                      right(nullptr)
                {}
        };

        struct Compare {
                bool operator()(HuffNode* l, HuffNode* r) const {
                        return l->frequency > r->frequency;
                }
        };

        struct Bits {
                uint64_t bits;
                uint8_t len;
        };

        std::vector<uint8_t> read();

        void build_table();
        void make_tree();
        void gen_codes(HuffNode* node, uint64_t bits, uint8_t len);
        void write_hdr(std::vector<uint8_t>& output);
        void encode(std::vector<uint8_t>& output);

        std::array<uint64_t, 256> frequencies_{};

        std::unordered_map<uint8_t, Bits> codes_;

        std::unique_ptr<HuffNode> root_;

        std::vector<uint8_t> file_data_;

public:
        explicit File(const std::filesystem::path& filepath);
        ~File();

        File(const File &) = delete;
        File& operator=(const File &) = delete;

        File(File &&) noexcept;
        File &operator=(File &&) noexcept;

        int compress(std::vector<uint8_t> &bytes);
};