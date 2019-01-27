//
// Generated file, do not edit! Created by nedtool 5.4 from messages/UDPControlInfo.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "UDPControlInfo_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("UDPCommandCode");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("UDPCommandCode"));
    e->insert(UDP_C_DATA, "UDP_C_DATA");
    e->insert(UDP_C_BIND, "UDP_C_BIND");
    e->insert(UDP_C_CONNECT, "UDP_C_CONNECT");
    e->insert(UDP_C_UNBIND, "UDP_C_UNBIND");
)

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("UDPStatusInd");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("UDPStatusInd"));
    e->insert(UDP_I_DATA, "UDP_I_DATA");
    e->insert(UDP_I_ERROR, "UDP_I_ERROR");
)

Register_Class(UDPControlInfo)

UDPControlInfo::UDPControlInfo() : ::omnetpp::cObject()
{
    this->sockId = -1;
    this->userId = -1;
    this->srcPort = 0;
    this->destPort = 0;
    this->interfaceId = -1;
}

UDPControlInfo::UDPControlInfo(const UDPControlInfo& other) : ::omnetpp::cObject(other)
{
    copy(other);
}

UDPControlInfo::~UDPControlInfo()
{
}

UDPControlInfo& UDPControlInfo::operator=(const UDPControlInfo& other)
{
    if (this==&other) return *this;
    ::omnetpp::cObject::operator=(other);
    copy(other);
    return *this;
}

void UDPControlInfo::copy(const UDPControlInfo& other)
{
    this->sockId = other.sockId;
    this->userId = other.userId;
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
    this->srcPort = other.srcPort;
    this->destPort = other.destPort;
    this->interfaceId = other.interfaceId;
}

void UDPControlInfo::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->sockId);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
    doParsimPacking(b,this->srcPort);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->interfaceId);
}

void UDPControlInfo::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->sockId);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
    doParsimUnpacking(b,this->srcPort);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->interfaceId);
}

int UDPControlInfo::getSockId() const
{
    return this->sockId;
}

void UDPControlInfo::setSockId(int sockId)
{
    this->sockId = sockId;
}

int UDPControlInfo::getUserId() const
{
    return this->userId;
}

void UDPControlInfo::setUserId(int userId)
{
    this->userId = userId;
}

IPvXAddress& UDPControlInfo::getSrcAddr()
{
    return this->srcAddr;
}

void UDPControlInfo::setSrcAddr(const IPvXAddress& srcAddr)
{
    this->srcAddr = srcAddr;
}

IPvXAddress& UDPControlInfo::getDestAddr()
{
    return this->destAddr;
}

void UDPControlInfo::setDestAddr(const IPvXAddress& destAddr)
{
    this->destAddr = destAddr;
}

int UDPControlInfo::getSrcPort() const
{
    return this->srcPort;
}

void UDPControlInfo::setSrcPort(int srcPort)
{
    this->srcPort = srcPort;
}

int UDPControlInfo::getDestPort() const
{
    return this->destPort;
}

void UDPControlInfo::setDestPort(int destPort)
{
    this->destPort = destPort;
}

int UDPControlInfo::getInterfaceId() const
{
    return this->interfaceId;
}

void UDPControlInfo::setInterfaceId(int interfaceId)
{
    this->interfaceId = interfaceId;
}

class UDPControlInfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    UDPControlInfoDescriptor();
    virtual ~UDPControlInfoDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(UDPControlInfoDescriptor)

UDPControlInfoDescriptor::UDPControlInfoDescriptor() : omnetpp::cClassDescriptor("UDPControlInfo", "omnetpp::cObject")
{
    propertynames = nullptr;
}

UDPControlInfoDescriptor::~UDPControlInfoDescriptor()
{
    delete[] propertynames;
}

bool UDPControlInfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<UDPControlInfo *>(obj)!=nullptr;
}

const char **UDPControlInfoDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *UDPControlInfoDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int UDPControlInfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 7+basedesc->getFieldCount() : 7;
}

unsigned int UDPControlInfoDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<7) ? fieldTypeFlags[field] : 0;
}

const char *UDPControlInfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sockId",
        "userId",
        "srcAddr",
        "destAddr",
        "srcPort",
        "destPort",
        "interfaceId",
    };
    return (field>=0 && field<7) ? fieldNames[field] : nullptr;
}

int UDPControlInfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sockId")==0) return base+0;
    if (fieldName[0]=='u' && strcmp(fieldName, "userId")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddr")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddr")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcPort")==0) return base+4;
    if (fieldName[0]=='d' && strcmp(fieldName, "destPort")==0) return base+5;
    if (fieldName[0]=='i' && strcmp(fieldName, "interfaceId")==0) return base+6;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *UDPControlInfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "IPvXAddress",
        "IPvXAddress",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<7) ? fieldTypeStrings[field] : nullptr;
}

const char **UDPControlInfoDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *UDPControlInfoDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int UDPControlInfoDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    UDPControlInfo *pp = (UDPControlInfo *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *UDPControlInfoDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    UDPControlInfo *pp = (UDPControlInfo *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string UDPControlInfoDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    UDPControlInfo *pp = (UDPControlInfo *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSockId());
        case 1: return long2string(pp->getUserId());
        case 2: {std::stringstream out; out << pp->getSrcAddr(); return out.str();}
        case 3: {std::stringstream out; out << pp->getDestAddr(); return out.str();}
        case 4: return long2string(pp->getSrcPort());
        case 5: return long2string(pp->getDestPort());
        case 6: return long2string(pp->getInterfaceId());
        default: return "";
    }
}

bool UDPControlInfoDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    UDPControlInfo *pp = (UDPControlInfo *)object; (void)pp;
    switch (field) {
        case 0: pp->setSockId(string2long(value)); return true;
        case 1: pp->setUserId(string2long(value)); return true;
        case 4: pp->setSrcPort(string2long(value)); return true;
        case 5: pp->setDestPort(string2long(value)); return true;
        case 6: pp->setInterfaceId(string2long(value)); return true;
        default: return false;
    }
}

const char *UDPControlInfoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 2: return omnetpp::opp_typename(typeid(IPvXAddress));
        case 3: return omnetpp::opp_typename(typeid(IPvXAddress));
        default: return nullptr;
    };
}

void *UDPControlInfoDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    UDPControlInfo *pp = (UDPControlInfo *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getSrcAddr()); break;
        case 3: return (void *)(&pp->getDestAddr()); break;
        default: return nullptr;
    }
}


