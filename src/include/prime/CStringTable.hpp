#pragma once

class CStringTable {
public:
  // The game's wchar_t is 16-bit; ours is 32-bit
  const char16_t *GetString(int idx) const;
};

extern CStringTable *gpStringTable;
