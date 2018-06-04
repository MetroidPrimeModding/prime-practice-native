#ifndef PRIMEWATCH_PRIME_CSIMPLEPOOL_HPP
#define PRIMEWATCH_PRIME_CSIMPLEPOOL_HPP

class SObjectTag {
public:
    SObjectTag(u32 fourCC, u32 id) : fourCC(fourCC), id(id) {}

    u32 fourCC;
    u32 id;
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

#endif //PRIMEWATCH_PRIME_CSIMPLEPOOL_HPP
