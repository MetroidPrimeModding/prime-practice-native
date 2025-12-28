#pragma once

const char *const HEX = "0123456789ABCDEF";

class SObjectTag {
public:
  SObjectTag(u32 fourCC, u32 id) : fourCC(fourCC), id(id) {}

  u32 fourCC;
  u32 id;

  /** Requires 14 length output str! */
  inline void writeToString(char *res) {
    for (int i = 0; i < 8; i++) {
      res[i] = HEX[(id >> ((7 - i) * 4)) & 0xF];
    }
    res[8] = '.';
    for (int i = 0; i < 4; i++) {
      res[9 + i] = (fourCC >> ((3 - i) * 8)) & 0xFF;
    }
    res[13] = 0;
  }
};

class CSimplePool {
public:
  //    CSimplePool(IFactory &factory);
  //    CToken GetObj(const SObjectTag &, const CVParamTransfer &);
  //    CToken GetObj(const SObjectTag &);
  //    CToken GetObj(const char *);
  //    CToken GetObj(const char *, const CVParamTransfer &);
  //    bool HasObject(const SObjectTag &) const;
  //    bool ObjectIsLive(const SObjectTag &) const;

  //    IFactory &GetFactory() const { return x18_factory; }

  //    void Flush();
  //    void ObjectUnreferenced(const SObjectTag &);
  //    std::vector <SObjectTag> GetReferencedTags() const;
};
