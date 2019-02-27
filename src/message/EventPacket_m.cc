//
// Generated file, do not edit! Created by nedtool 5.2 from message/EventPacket.msg.
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
#include "EventPacket_m.h"

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

NodeMessage::NodeMessage()
{
    this->nodeID = 0;
    this->capacity = 0;
    for (unsigned int i=0; i<1024; i++)
        this->neighborNodeID[i] = 0;
    for (unsigned int i=0; i<1024; i++)
        this->neighborCapacity[i] = 0;
    this->neighborNum = 0;
    for (unsigned int i=0; i<1024; i++)
        this->transmission_rate[i] = 0;
}

void __doPacking(omnetpp::cCommBuffer *b, const NodeMessage& a)
{
    doParsimPacking(b,a.nodeID);
    doParsimPacking(b,a.capacity);
    doParsimArrayPacking(b,a.neighborNodeID,1024);
    doParsimArrayPacking(b,a.neighborCapacity,1024);
    doParsimPacking(b,a.neighborNum);
    doParsimArrayPacking(b,a.transmission_rate,1024);
}

void __doUnpacking(omnetpp::cCommBuffer *b, NodeMessage& a)
{
    doParsimUnpacking(b,a.nodeID);
    doParsimUnpacking(b,a.capacity);
    doParsimArrayUnpacking(b,a.neighborNodeID,1024);
    doParsimArrayUnpacking(b,a.neighborCapacity,1024);
    doParsimUnpacking(b,a.neighborNum);
    doParsimArrayUnpacking(b,a.transmission_rate,1024);
}

class NodeMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    NodeMessageDescriptor();
    virtual ~NodeMessageDescriptor();

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

Register_ClassDescriptor(NodeMessageDescriptor)

NodeMessageDescriptor::NodeMessageDescriptor() : omnetpp::cClassDescriptor("NodeMessage", "")
{
    propertynames = nullptr;
}

NodeMessageDescriptor::~NodeMessageDescriptor()
{
    delete[] propertynames;
}

bool NodeMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NodeMessage *>(obj)!=nullptr;
}

const char **NodeMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *NodeMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int NodeMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount() : 6;
}

unsigned int NodeMessageDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *NodeMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "nodeID",
        "capacity",
        "neighborNodeID",
        "neighborCapacity",
        "neighborNum",
        "transmission_rate",
    };
    return (field>=0 && field<6) ? fieldNames[field] : nullptr;
}

int NodeMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeID")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "capacity")==0) return base+1;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborNodeID")==0) return base+2;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborCapacity")==0) return base+3;
    if (fieldName[0]=='n' && strcmp(fieldName, "neighborNum")==0) return base+4;
    if (fieldName[0]=='t' && strcmp(fieldName, "transmission_rate")==0) return base+5;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *NodeMessageDescriptor::getFieldTypeString(int field) const
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
        "int",
        "int",
        "int",
        "double",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : nullptr;
}

const char **NodeMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *NodeMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int NodeMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    NodeMessage *pp = (NodeMessage *)object; (void)pp;
    switch (field) {
        case 2: return 1024;
        case 3: return 1024;
        case 5: return 1024;
        default: return 0;
    }
}

const char *NodeMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NodeMessage *pp = (NodeMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string NodeMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NodeMessage *pp = (NodeMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->nodeID);
        case 1: return long2string(pp->capacity);
        case 2: if (i>=1024) return "";
                return long2string(pp->neighborNodeID[i]);
        case 3: if (i>=1024) return "";
                return long2string(pp->neighborCapacity[i]);
        case 4: return long2string(pp->neighborNum);
        case 5: if (i>=1024) return "";
                return double2string(pp->transmission_rate[i]);
        default: return "";
    }
}

bool NodeMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    NodeMessage *pp = (NodeMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->nodeID = string2long(value); return true;
        case 1: pp->capacity = string2long(value); return true;
        case 2: if (i>=1024) return false;
                pp->neighborNodeID[i] = string2long(value); return true;
        case 3: if (i>=1024) return false;
                pp->neighborCapacity[i] = string2long(value); return true;
        case 4: pp->neighborNum = string2long(value); return true;
        case 5: if (i>=1024) return false;
                pp->transmission_rate[i] = string2double(value); return true;
        default: return false;
    }
}

const char *NodeMessageDescriptor::getFieldStructName(int field) const
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

void *NodeMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    NodeMessage *pp = (NodeMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Placement::Placement()
{
    this->placementNum = 0;
    for (unsigned int i=0; i<128; i++)
        this->OperatorGraphIndex[i] = 0;
    for (unsigned int i=0; i<128; i++)
        this->operatorID[i] = 0;
}

void __doPacking(omnetpp::cCommBuffer *b, const Placement& a)
{
    doParsimPacking(b,a.placementNum);
    doParsimArrayPacking(b,a.OperatorGraphIndex,128);
    doParsimArrayPacking(b,a.operatorID,128);
}

void __doUnpacking(omnetpp::cCommBuffer *b, Placement& a)
{
    doParsimUnpacking(b,a.placementNum);
    doParsimArrayUnpacking(b,a.OperatorGraphIndex,128);
    doParsimArrayUnpacking(b,a.operatorID,128);
}

class PlacementDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    PlacementDescriptor();
    virtual ~PlacementDescriptor();

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

Register_ClassDescriptor(PlacementDescriptor)

PlacementDescriptor::PlacementDescriptor() : omnetpp::cClassDescriptor("Placement", "")
{
    propertynames = nullptr;
}

PlacementDescriptor::~PlacementDescriptor()
{
    delete[] propertynames;
}

bool PlacementDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Placement *>(obj)!=nullptr;
}

const char **PlacementDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *PlacementDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int PlacementDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int PlacementDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *PlacementDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "placementNum",
        "OperatorGraphIndex",
        "operatorID",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int PlacementDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "placementNum")==0) return base+0;
    if (fieldName[0]=='O' && strcmp(fieldName, "OperatorGraphIndex")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "operatorID")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *PlacementDescriptor::getFieldTypeString(int field) const
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
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **PlacementDescriptor::getFieldPropertyNames(int field) const
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

const char *PlacementDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int PlacementDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    Placement *pp = (Placement *)object; (void)pp;
    switch (field) {
        case 1: return 128;
        case 2: return 128;
        default: return 0;
    }
}

const char *PlacementDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Placement *pp = (Placement *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string PlacementDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    Placement *pp = (Placement *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->placementNum);
        case 1: if (i>=128) return "";
                return long2string(pp->OperatorGraphIndex[i]);
        case 2: if (i>=128) return "";
                return long2string(pp->operatorID[i]);
        default: return "";
    }
}

bool PlacementDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    Placement *pp = (Placement *)object; (void)pp;
    switch (field) {
        case 0: pp->placementNum = string2long(value); return true;
        case 1: if (i>=128) return false;
                pp->OperatorGraphIndex[i] = string2long(value); return true;
        case 2: if (i>=128) return false;
                pp->operatorID[i] = string2long(value); return true;
        default: return false;
    }
}

const char *PlacementDescriptor::getFieldStructName(int field) const
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

void *PlacementDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    Placement *pp = (Placement *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(EventPacket)

EventPacket::EventPacket(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->placementMessage = false;
    this->monitorID = 0;
    this->monitorMessage = false;
    this->monitorFlag = false;
    this->monitorH = 0;
    this->used = false;
    this->srcAddr = 0;
    this->destAddr = 0;
    this->port = 0;
    this->appNum = 0;
    this->hopCount = 0;
    this->markerMessage = false;
    this->sendTime = 0;
    this->queueTime = 0;
    this->queueBeginTime = 0;
    this->transmissionTime = 0;
    this->transmissionBeginTime = 0;
    this->processTime = 0;
    this->markerID = 0;
    this->networkUsage = 0;
    this->managerNetworkUsage = 0;
    this->eventNum = 0;
    for (unsigned int i=0; i<100; i++)
        this->operatorType[i] = 0;
    this->reMarkerMessage = false;
    this->time = 0;
    this->vehicleID = 0;
    this->speed = 0;
}

EventPacket::EventPacket(const EventPacket& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

EventPacket::~EventPacket()
{
}

EventPacket& EventPacket::operator=(const EventPacket& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void EventPacket::copy(const EventPacket& other)
{
    this->placementMessage = other.placementMessage;
    this->placement = other.placement;
    this->fognode = other.fognode;
    this->monitorID = other.monitorID;
    this->monitorMessage = other.monitorMessage;
    this->monitorFlag = other.monitorFlag;
    this->monitorH = other.monitorH;
    this->used = other.used;
    this->destAddrs = other.destAddrs;
    this->srcAddr = other.srcAddr;
    this->destAddr = other.destAddr;
    this->port = other.port;
    this->appNum = other.appNum;
    this->hopCount = other.hopCount;
    this->markerMessage = other.markerMessage;
    this->sendTime = other.sendTime;
    this->queueTime = other.queueTime;
    this->queueBeginTime = other.queueBeginTime;
    this->transmissionTime = other.transmissionTime;
    this->transmissionBeginTime = other.transmissionBeginTime;
    this->processTime = other.processTime;
    this->markerID = other.markerID;
    this->networkUsage = other.networkUsage;
    this->managerNetworkUsage = other.managerNetworkUsage;
    this->eventNum = other.eventNum;
    for (unsigned int i=0; i<100; i++)
        this->operatorType[i] = other.operatorType[i];
    this->reMarkerMessage = other.reMarkerMessage;
    this->time = other.time;
    this->vehicleID = other.vehicleID;
    this->speed = other.speed;
    this->laneID = other.laneID;
}

void EventPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->placementMessage);
    doParsimPacking(b,this->placement);
    doParsimPacking(b,this->fognode);
    doParsimPacking(b,this->monitorID);
    doParsimPacking(b,this->monitorMessage);
    doParsimPacking(b,this->monitorFlag);
    doParsimPacking(b,this->monitorH);
    doParsimPacking(b,this->used);
    doParsimPacking(b,this->destAddrs);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->destAddr);
    doParsimPacking(b,this->port);
    doParsimPacking(b,this->appNum);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->markerMessage);
    doParsimPacking(b,this->sendTime);
    doParsimPacking(b,this->queueTime);
    doParsimPacking(b,this->queueBeginTime);
    doParsimPacking(b,this->transmissionTime);
    doParsimPacking(b,this->transmissionBeginTime);
    doParsimPacking(b,this->processTime);
    doParsimPacking(b,this->markerID);
    doParsimPacking(b,this->networkUsage);
    doParsimPacking(b,this->managerNetworkUsage);
    doParsimPacking(b,this->eventNum);
    doParsimArrayPacking(b,this->operatorType,100);
    doParsimPacking(b,this->reMarkerMessage);
    doParsimPacking(b,this->time);
    doParsimPacking(b,this->vehicleID);
    doParsimPacking(b,this->speed);
    doParsimPacking(b,this->laneID);
}

void EventPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->placementMessage);
    doParsimUnpacking(b,this->placement);
    doParsimUnpacking(b,this->fognode);
    doParsimUnpacking(b,this->monitorID);
    doParsimUnpacking(b,this->monitorMessage);
    doParsimUnpacking(b,this->monitorFlag);
    doParsimUnpacking(b,this->monitorH);
    doParsimUnpacking(b,this->used);
    doParsimUnpacking(b,this->destAddrs);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->destAddr);
    doParsimUnpacking(b,this->port);
    doParsimUnpacking(b,this->appNum);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->markerMessage);
    doParsimUnpacking(b,this->sendTime);
    doParsimUnpacking(b,this->queueTime);
    doParsimUnpacking(b,this->queueBeginTime);
    doParsimUnpacking(b,this->transmissionTime);
    doParsimUnpacking(b,this->transmissionBeginTime);
    doParsimUnpacking(b,this->processTime);
    doParsimUnpacking(b,this->markerID);
    doParsimUnpacking(b,this->networkUsage);
    doParsimUnpacking(b,this->managerNetworkUsage);
    doParsimUnpacking(b,this->eventNum);
    doParsimArrayUnpacking(b,this->operatorType,100);
    doParsimUnpacking(b,this->reMarkerMessage);
    doParsimUnpacking(b,this->time);
    doParsimUnpacking(b,this->vehicleID);
    doParsimUnpacking(b,this->speed);
    doParsimUnpacking(b,this->laneID);
}

bool EventPacket::getPlacementMessage() const
{
    return this->placementMessage;
}

void EventPacket::setPlacementMessage(bool placementMessage)
{
    this->placementMessage = placementMessage;
}

Placement& EventPacket::getPlacement()
{
    return this->placement;
}

void EventPacket::setPlacement(const Placement& placement)
{
    this->placement = placement;
}

NodeMessage& EventPacket::getFognode()
{
    return this->fognode;
}

void EventPacket::setFognode(const NodeMessage& fognode)
{
    this->fognode = fognode;
}

int EventPacket::getMonitorID() const
{
    return this->monitorID;
}

void EventPacket::setMonitorID(int monitorID)
{
    this->monitorID = monitorID;
}

bool EventPacket::getMonitorMessage() const
{
    return this->monitorMessage;
}

void EventPacket::setMonitorMessage(bool monitorMessage)
{
    this->monitorMessage = monitorMessage;
}

bool EventPacket::getMonitorFlag() const
{
    return this->monitorFlag;
}

void EventPacket::setMonitorFlag(bool monitorFlag)
{
    this->monitorFlag = monitorFlag;
}

int EventPacket::getMonitorH() const
{
    return this->monitorH;
}

void EventPacket::setMonitorH(int monitorH)
{
    this->monitorH = monitorH;
}

bool EventPacket::getUsed() const
{
    return this->used;
}

void EventPacket::setUsed(bool used)
{
    this->used = used;
}

const char * EventPacket::getDestAddrs() const
{
    return this->destAddrs.c_str();
}

void EventPacket::setDestAddrs(const char * destAddrs)
{
    this->destAddrs = destAddrs;
}

int EventPacket::getSrcAddr() const
{
    return this->srcAddr;
}

void EventPacket::setSrcAddr(int srcAddr)
{
    this->srcAddr = srcAddr;
}

int EventPacket::getDestAddr() const
{
    return this->destAddr;
}

void EventPacket::setDestAddr(int destAddr)
{
    this->destAddr = destAddr;
}

int EventPacket::getPort() const
{
    return this->port;
}

void EventPacket::setPort(int port)
{
    this->port = port;
}

int EventPacket::getAppNum() const
{
    return this->appNum;
}

void EventPacket::setAppNum(int appNum)
{
    this->appNum = appNum;
}

int EventPacket::getHopCount() const
{
    return this->hopCount;
}

void EventPacket::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

bool EventPacket::getMarkerMessage() const
{
    return this->markerMessage;
}

void EventPacket::setMarkerMessage(bool markerMessage)
{
    this->markerMessage = markerMessage;
}

double EventPacket::getSendTime() const
{
    return this->sendTime;
}

void EventPacket::setSendTime(double sendTime)
{
    this->sendTime = sendTime;
}

double EventPacket::getQueueTime() const
{
    return this->queueTime;
}

void EventPacket::setQueueTime(double queueTime)
{
    this->queueTime = queueTime;
}

double EventPacket::getQueueBeginTime() const
{
    return this->queueBeginTime;
}

void EventPacket::setQueueBeginTime(double queueBeginTime)
{
    this->queueBeginTime = queueBeginTime;
}

double EventPacket::getTransmissionTime() const
{
    return this->transmissionTime;
}

void EventPacket::setTransmissionTime(double transmissionTime)
{
    this->transmissionTime = transmissionTime;
}

double EventPacket::getTransmissionBeginTime() const
{
    return this->transmissionBeginTime;
}

void EventPacket::setTransmissionBeginTime(double transmissionBeginTime)
{
    this->transmissionBeginTime = transmissionBeginTime;
}

double EventPacket::getProcessTime() const
{
    return this->processTime;
}

void EventPacket::setProcessTime(double processTime)
{
    this->processTime = processTime;
}

int EventPacket::getMarkerID() const
{
    return this->markerID;
}

void EventPacket::setMarkerID(int markerID)
{
    this->markerID = markerID;
}

double EventPacket::getNetworkUsage() const
{
    return this->networkUsage;
}

void EventPacket::setNetworkUsage(double networkUsage)
{
    this->networkUsage = networkUsage;
}

double EventPacket::getManagerNetworkUsage() const
{
    return this->managerNetworkUsage;
}

void EventPacket::setManagerNetworkUsage(double managerNetworkUsage)
{
    this->managerNetworkUsage = managerNetworkUsage;
}

int EventPacket::getEventNum() const
{
    return this->eventNum;
}

void EventPacket::setEventNum(int eventNum)
{
    this->eventNum = eventNum;
}

unsigned int EventPacket::getOperatorTypeArraySize() const
{
    return 100;
}

int EventPacket::getOperatorType(unsigned int k) const
{
    if (k>=100) throw omnetpp::cRuntimeError("Array of size 100 indexed by %lu", (unsigned long)k);
    return this->operatorType[k];
}

void EventPacket::setOperatorType(unsigned int k, int operatorType)
{
    if (k>=100) throw omnetpp::cRuntimeError("Array of size 100 indexed by %lu", (unsigned long)k);
    this->operatorType[k] = operatorType;
}

bool EventPacket::getReMarkerMessage() const
{
    return this->reMarkerMessage;
}

void EventPacket::setReMarkerMessage(bool reMarkerMessage)
{
    this->reMarkerMessage = reMarkerMessage;
}

int EventPacket::getTime() const
{
    return this->time;
}

void EventPacket::setTime(int time)
{
    this->time = time;
}

int EventPacket::getVehicleID() const
{
    return this->vehicleID;
}

void EventPacket::setVehicleID(int vehicleID)
{
    this->vehicleID = vehicleID;
}

double EventPacket::getSpeed() const
{
    return this->speed;
}

void EventPacket::setSpeed(double speed)
{
    this->speed = speed;
}

const char * EventPacket::getLaneID() const
{
    return this->laneID.c_str();
}

void EventPacket::setLaneID(const char * laneID)
{
    this->laneID = laneID;
}

class EventPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    EventPacketDescriptor();
    virtual ~EventPacketDescriptor();

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

Register_ClassDescriptor(EventPacketDescriptor)

EventPacketDescriptor::EventPacketDescriptor() : omnetpp::cClassDescriptor("EventPacket", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

EventPacketDescriptor::~EventPacketDescriptor()
{
    delete[] propertynames;
}

bool EventPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<EventPacket *>(obj)!=nullptr;
}

const char **EventPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *EventPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int EventPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 31+basedesc->getFieldCount() : 31;
}

unsigned int EventPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<31) ? fieldTypeFlags[field] : 0;
}

const char *EventPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "placementMessage",
        "placement",
        "fognode",
        "monitorID",
        "monitorMessage",
        "monitorFlag",
        "monitorH",
        "used",
        "destAddrs",
        "srcAddr",
        "destAddr",
        "port",
        "appNum",
        "hopCount",
        "markerMessage",
        "sendTime",
        "queueTime",
        "queueBeginTime",
        "transmissionTime",
        "transmissionBeginTime",
        "processTime",
        "markerID",
        "networkUsage",
        "managerNetworkUsage",
        "eventNum",
        "operatorType",
        "reMarkerMessage",
        "time",
        "vehicleID",
        "speed",
        "laneID",
    };
    return (field>=0 && field<31) ? fieldNames[field] : nullptr;
}

int EventPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "placementMessage")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "placement")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "fognode")==0) return base+2;
    if (fieldName[0]=='m' && strcmp(fieldName, "monitorID")==0) return base+3;
    if (fieldName[0]=='m' && strcmp(fieldName, "monitorMessage")==0) return base+4;
    if (fieldName[0]=='m' && strcmp(fieldName, "monitorFlag")==0) return base+5;
    if (fieldName[0]=='m' && strcmp(fieldName, "monitorH")==0) return base+6;
    if (fieldName[0]=='u' && strcmp(fieldName, "used")==0) return base+7;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddrs")==0) return base+8;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddr")==0) return base+9;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddr")==0) return base+10;
    if (fieldName[0]=='p' && strcmp(fieldName, "port")==0) return base+11;
    if (fieldName[0]=='a' && strcmp(fieldName, "appNum")==0) return base+12;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+13;
    if (fieldName[0]=='m' && strcmp(fieldName, "markerMessage")==0) return base+14;
    if (fieldName[0]=='s' && strcmp(fieldName, "sendTime")==0) return base+15;
    if (fieldName[0]=='q' && strcmp(fieldName, "queueTime")==0) return base+16;
    if (fieldName[0]=='q' && strcmp(fieldName, "queueBeginTime")==0) return base+17;
    if (fieldName[0]=='t' && strcmp(fieldName, "transmissionTime")==0) return base+18;
    if (fieldName[0]=='t' && strcmp(fieldName, "transmissionBeginTime")==0) return base+19;
    if (fieldName[0]=='p' && strcmp(fieldName, "processTime")==0) return base+20;
    if (fieldName[0]=='m' && strcmp(fieldName, "markerID")==0) return base+21;
    if (fieldName[0]=='n' && strcmp(fieldName, "networkUsage")==0) return base+22;
    if (fieldName[0]=='m' && strcmp(fieldName, "managerNetworkUsage")==0) return base+23;
    if (fieldName[0]=='e' && strcmp(fieldName, "eventNum")==0) return base+24;
    if (fieldName[0]=='o' && strcmp(fieldName, "operatorType")==0) return base+25;
    if (fieldName[0]=='r' && strcmp(fieldName, "reMarkerMessage")==0) return base+26;
    if (fieldName[0]=='t' && strcmp(fieldName, "time")==0) return base+27;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleID")==0) return base+28;
    if (fieldName[0]=='s' && strcmp(fieldName, "speed")==0) return base+29;
    if (fieldName[0]=='l' && strcmp(fieldName, "laneID")==0) return base+30;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *EventPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "bool",
        "Placement",
        "NodeMessage",
        "int",
        "bool",
        "bool",
        "int",
        "bool",
        "string",
        "int",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "double",
        "double",
        "double",
        "double",
        "double",
        "double",
        "int",
        "double",
        "double",
        "int",
        "int",
        "bool",
        "int",
        "int",
        "double",
        "string",
    };
    return (field>=0 && field<31) ? fieldTypeStrings[field] : nullptr;
}

const char **EventPacketDescriptor::getFieldPropertyNames(int field) const
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

const char *EventPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int EventPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    EventPacket *pp = (EventPacket *)object; (void)pp;
    switch (field) {
        case 25: return 100;
        default: return 0;
    }
}

const char *EventPacketDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    EventPacket *pp = (EventPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string EventPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    EventPacket *pp = (EventPacket *)object; (void)pp;
    switch (field) {
        case 0: return bool2string(pp->getPlacementMessage());
        case 1: {std::stringstream out; out << pp->getPlacement(); return out.str();}
        case 2: {std::stringstream out; out << pp->getFognode(); return out.str();}
        case 3: return long2string(pp->getMonitorID());
        case 4: return bool2string(pp->getMonitorMessage());
        case 5: return bool2string(pp->getMonitorFlag());
        case 6: return long2string(pp->getMonitorH());
        case 7: return bool2string(pp->getUsed());
        case 8: return oppstring2string(pp->getDestAddrs());
        case 9: return long2string(pp->getSrcAddr());
        case 10: return long2string(pp->getDestAddr());
        case 11: return long2string(pp->getPort());
        case 12: return long2string(pp->getAppNum());
        case 13: return long2string(pp->getHopCount());
        case 14: return bool2string(pp->getMarkerMessage());
        case 15: return double2string(pp->getSendTime());
        case 16: return double2string(pp->getQueueTime());
        case 17: return double2string(pp->getQueueBeginTime());
        case 18: return double2string(pp->getTransmissionTime());
        case 19: return double2string(pp->getTransmissionBeginTime());
        case 20: return double2string(pp->getProcessTime());
        case 21: return long2string(pp->getMarkerID());
        case 22: return double2string(pp->getNetworkUsage());
        case 23: return double2string(pp->getManagerNetworkUsage());
        case 24: return long2string(pp->getEventNum());
        case 25: return long2string(pp->getOperatorType(i));
        case 26: return bool2string(pp->getReMarkerMessage());
        case 27: return long2string(pp->getTime());
        case 28: return long2string(pp->getVehicleID());
        case 29: return double2string(pp->getSpeed());
        case 30: return oppstring2string(pp->getLaneID());
        default: return "";
    }
}

bool EventPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    EventPacket *pp = (EventPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setPlacementMessage(string2bool(value)); return true;
        case 3: pp->setMonitorID(string2long(value)); return true;
        case 4: pp->setMonitorMessage(string2bool(value)); return true;
        case 5: pp->setMonitorFlag(string2bool(value)); return true;
        case 6: pp->setMonitorH(string2long(value)); return true;
        case 7: pp->setUsed(string2bool(value)); return true;
        case 8: pp->setDestAddrs((value)); return true;
        case 9: pp->setSrcAddr(string2long(value)); return true;
        case 10: pp->setDestAddr(string2long(value)); return true;
        case 11: pp->setPort(string2long(value)); return true;
        case 12: pp->setAppNum(string2long(value)); return true;
        case 13: pp->setHopCount(string2long(value)); return true;
        case 14: pp->setMarkerMessage(string2bool(value)); return true;
        case 15: pp->setSendTime(string2double(value)); return true;
        case 16: pp->setQueueTime(string2double(value)); return true;
        case 17: pp->setQueueBeginTime(string2double(value)); return true;
        case 18: pp->setTransmissionTime(string2double(value)); return true;
        case 19: pp->setTransmissionBeginTime(string2double(value)); return true;
        case 20: pp->setProcessTime(string2double(value)); return true;
        case 21: pp->setMarkerID(string2long(value)); return true;
        case 22: pp->setNetworkUsage(string2double(value)); return true;
        case 23: pp->setManagerNetworkUsage(string2double(value)); return true;
        case 24: pp->setEventNum(string2long(value)); return true;
        case 25: pp->setOperatorType(i,string2long(value)); return true;
        case 26: pp->setReMarkerMessage(string2bool(value)); return true;
        case 27: pp->setTime(string2long(value)); return true;
        case 28: pp->setVehicleID(string2long(value)); return true;
        case 29: pp->setSpeed(string2double(value)); return true;
        case 30: pp->setLaneID((value)); return true;
        default: return false;
    }
}

const char *EventPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 1: return omnetpp::opp_typename(typeid(Placement));
        case 2: return omnetpp::opp_typename(typeid(NodeMessage));
        default: return nullptr;
    };
}

void *EventPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    EventPacket *pp = (EventPacket *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getPlacement()); break;
        case 2: return (void *)(&pp->getFognode()); break;
        default: return nullptr;
    }
}


