//
// Generated file, do not edit! Created by nedtool 5.4 from messages/CANMessage.msg.
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
#include "CANMessage_m.h"

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
    omnetpp::cEnum *e = omnetpp::cEnum::find("CANMsgType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("CANMsgType"));
    e->insert(CAN_LOOK_UP, "CAN_LOOK_UP");
    e->insert(CAN_FINAL, "CAN_FINAL");
    e->insert(JOIN, "JOIN");
    e->insert(JOIN_REPLY, "JOIN_REPLY");
    e->insert(ADD_NEIGHBOR, "ADD_NEIGHBOR");
    e->insert(RM_NEIGHBOR, "RM_NEIGHBOR");
    e->insert(JOIN_REPLY_FAIL, "JOIN_REPLY_FAIL");
    e->insert(UPDATE_AREA, "UPDATE_AREA");
    e->insert(NEIGHBOR_UPDATE, "NEIGHBOR_UPDATE");
    e->insert(NEIGHBOR_EXCHANGE, "NEIGHBOR_EXCHANGE");
    e->insert(ERS, "ERS");
    e->insert(ERS_REPLY, "ERS_REPLY");
    e->insert(NEIGHBOR_TAKEOVER, "NEIGHBOR_TAKEOVER");
    e->insert(CAN_STORE, "CAN_STORE");
    e->insert(CAN_GET, "CAN_GET");
    e->insert(CAN_REPLICATE, "CAN_REPLICATE");
    e->insert(CAN_FIX_REPLICA, "CAN_FIX_REPLICA");
    e->insert(FIX_LOAD, "FIX_LOAD");
    e->insert(CAN_REPLY, "CAN_REPLY");
)

Register_Class(CANMessage)

CANMessage::CANMessage(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->type = 0;
    this->sender = 0;
    this->hop = 0;
}

CANMessage::CANMessage(const CANMessage& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

CANMessage::~CANMessage()
{
}

CANMessage& CANMessage::operator=(const CANMessage& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void CANMessage::copy(const CANMessage& other)
{
    this->type = other.type;
    this->label = other.label;
    this->sender = other.sender;
    this->content = other.content;
    this->hop = other.hop;
}

void CANMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->label);
    doParsimPacking(b,this->sender);
    doParsimPacking(b,this->content);
    doParsimPacking(b,this->hop);
}

void CANMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->label);
    doParsimUnpacking(b,this->sender);
    doParsimUnpacking(b,this->content);
    doParsimUnpacking(b,this->hop);
}

int CANMessage::getType() const
{
    return this->type;
}

void CANMessage::setType(int type)
{
    this->type = type;
}

const char * CANMessage::getLabel() const
{
    return this->label.c_str();
}

void CANMessage::setLabel(const char * label)
{
    this->label = label;
}

unsigned long CANMessage::getSender() const
{
    return this->sender;
}

void CANMessage::setSender(unsigned long sender)
{
    this->sender = sender;
}

const char * CANMessage::getContent() const
{
    return this->content.c_str();
}

void CANMessage::setContent(const char * content)
{
    this->content = content;
}

int CANMessage::getHop() const
{
    return this->hop;
}

void CANMessage::setHop(int hop)
{
    this->hop = hop;
}

class CANMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    CANMessageDescriptor();
    virtual ~CANMessageDescriptor();

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

Register_ClassDescriptor(CANMessageDescriptor)

CANMessageDescriptor::CANMessageDescriptor() : omnetpp::cClassDescriptor("CANMessage", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

CANMessageDescriptor::~CANMessageDescriptor()
{
    delete[] propertynames;
}

bool CANMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CANMessage *>(obj)!=nullptr;
}

const char **CANMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *CANMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int CANMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int CANMessageDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *CANMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "type",
        "label",
        "sender",
        "content",
        "hop",
    };
    return (field>=0 && field<5) ? fieldNames[field] : nullptr;
}

int CANMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    if (fieldName[0]=='l' && strcmp(fieldName, "label")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sender")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "content")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "hop")==0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *CANMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "string",
        "unsigned long",
        "string",
        "int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : nullptr;
}

const char **CANMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *CANMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "CANMsgType";
            return nullptr;
        default: return nullptr;
    }
}

int CANMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    CANMessage *pp = (CANMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *CANMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    CANMessage *pp = (CANMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CANMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    CANMessage *pp = (CANMessage *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getType(), "CANMsgType");
        case 1: return oppstring2string(pp->getLabel());
        case 2: return ulong2string(pp->getSender());
        case 3: return oppstring2string(pp->getContent());
        case 4: return long2string(pp->getHop());
        default: return "";
    }
}

bool CANMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    CANMessage *pp = (CANMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setType((CANMsgType)string2enum(value, "CANMsgType")); return true;
        case 1: pp->setLabel((value)); return true;
        case 2: pp->setSender(string2ulong(value)); return true;
        case 3: pp->setContent((value)); return true;
        case 4: pp->setHop(string2long(value)); return true;
        default: return false;
    }
}

const char *CANMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *CANMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    CANMessage *pp = (CANMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


