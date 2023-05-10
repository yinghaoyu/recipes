#include <iostream>
#include <assert.h>
#include <string>

using namespace std;

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED \
  do {                       \
  } while (0)
#endif

// 从 ptr 中取出 4 个字节
inline uint32_t DecodeFixed32(const char* ptr) {
  const uint8_t* const buffer = reinterpret_cast<const uint8_t*>(ptr);

  // Recent clang and gcc optimize this to a single mov / ldr instruction.
  return (static_cast<uint32_t>(buffer[0])) |
         (static_cast<uint32_t>(buffer[1]) << 8) |
         (static_cast<uint32_t>(buffer[2]) << 16) |
         (static_cast<uint32_t>(buffer[3]) << 24);
}

uint32_t Hash(const char* data, size_t n, uint32_t seed) {
  // Similar to murmur hash
  const uint32_t m = 0xc6a4a793;
  const uint32_t r = 24;
  const char* limit = data + n;
  uint32_t h = seed ^ (n * m);

  // Pick up four bytes at a time
  while (data + 4 <= limit) {
    uint32_t w = DecodeFixed32(data);
    data += 4;
    h += w;
    // Mur = Mutiply and Rotate
    h *= m;
    // 高 16 位和低 16 位进行异或
    // 高16和低16位都能影响结果，起到了降维的作用
    h ^= (h >> 16);
  }

  // Pick up remaining bytes
  switch (limit - data) {
    case 3:
      h += static_cast<uint8_t>(data[2]) << 16;
      FALLTHROUGH_INTENDED;
    case 2:
      h += static_cast<uint8_t>(data[1]) << 8;
      FALLTHROUGH_INTENDED;
    case 1:
      h += static_cast<uint8_t>(data[0]);
      h *= m;
      h ^= (h >> r);
      break;
  }
  return h;
}

int main()
{
  const uint8_t data1[1] = {0x62};
  const uint8_t data2[2] = {0xc3, 0x97};
  const uint8_t data3[3] = {0xe2, 0x99, 0xa5};
  const uint8_t data4[4] = {0xe1, 0x80, 0xb9, 0x32};
  const uint8_t data5[48] = {
      0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
      0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18, 0x28, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

  assert(Hash(0, 0, 0xbc9f1d34) == 0xbc9f1d34);
  assert(Hash(reinterpret_cast<const char *>(data1), sizeof(data1), 0xbc9f1d34) == 0xef1345c4);
  assert(Hash(reinterpret_cast<const char *>(data2), sizeof(data2), 0xbc9f1d34) == 0x5b663814);
  assert(Hash(reinterpret_cast<const char *>(data3), sizeof(data3), 0xbc9f1d34) == 0x323c078f);
  assert(Hash(reinterpret_cast<const char *>(data4), sizeof(data4), 0xbc9f1d34) == 0xed21633a);
  assert(Hash(reinterpret_cast<const char *>(data5), sizeof(data5), 0x12345678) == 0xf333dabb);
  return 0;
}
