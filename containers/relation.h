#ifndef _RELATION_H_
#define _RELATION_H_

#include "../def_global.h"



class Record
{
public:
    RecordId id;
    Timestamp start;
    Timestamp end;
    
    Record();
    Record(RecordId id, Timestamp start, Timestamp end);
    
    bool operator < (const Record &rhs) const;
    bool operator >= (const Record &rhs) const;
    void print(const char c) const;
    ~Record();
};

inline bool CompareRecordsByEnd(const Record &lhs, const Record &rhs)
{
    if (lhs.end == rhs.end)
        return (lhs.id < rhs.id);
    else
        return (lhs.end < rhs.end);
}




class RecordStart
{
public:
    RecordId id;
    Timestamp start;
    
    RecordStart();
    RecordStart(RecordId id, Timestamp start);
    
    bool operator < (const RecordStart &rhs) const;
    bool operator >= (const RecordStart &rhs) const;
    void print(const char c) const;
    ~RecordStart();
};



// copy of RecordStart
class RecordEnd
{
public:
    RecordId id;
    Timestamp end;
    
    RecordEnd();
    RecordEnd(RecordId id, Timestamp end);
    
    bool operator < (const RecordEnd &rhs) const;
    bool operator >= (const RecordEnd &rhs) const;
    void print(const char c) const;
    ~RecordEnd();
};



class TimestampPair
{
public:
    Timestamp start;
    Timestamp end;
    
    TimestampPair();
    TimestampPair(Timestamp start, Timestamp end);
    
    bool operator < (const TimestampPair &rhs) const;
    bool operator >= (const TimestampPair &rhs) const;
    void print(const char c) const;
    ~TimestampPair();
};



// Descending order
inline bool CompareByEnd(const Record &lhs, const Record &rhs)
{
    if (lhs.end == rhs.end)
        return (lhs.id < rhs.id);
    else
        return (lhs.end < rhs.end);
};



class Relation : public vector<Record>
{
public:
    Timestamp gstart;
    Timestamp gend;
    Timestamp longestRecord;
    float avgRecordExtent;
    
    Relation();
    Relation(Relation &R);
    Relation(vector<Record>::const_iterator iterBegin, vector<Record>::const_iterator iterEnd);
    void load(const char *filename);
    void sortByStart();
    void sortByEnd();
    void print(char c);
    ~Relation();
    
    // Querying
    // Basic predicates of Allen's algebra
    size_t execute_Equals(RangeQuery q);
    size_t execute_Starts(RangeQuery q);
    size_t execute_Started(RangeQuery q);
    size_t execute_Finishes(RangeQuery q);
    size_t execute_Finished(RangeQuery q);
    size_t execute_Meets(RangeQuery q);
    size_t execute_Met(RangeQuery q);
    size_t execute_Overlaps(RangeQuery q);
    size_t execute_Overlapped(RangeQuery q);
    size_t execute_Contains(RangeQuery q);
    size_t execute_Contained(RangeQuery q);
    size_t execute_Precedes(RangeQuery q);
    size_t execute_Preceded(RangeQuery q);

    // Generalized predicate, ACM SIGMOD'22 gOverlaps
    size_t execute_gOverlaps(StabbingQuery q);
    size_t execute_gOverlaps(RangeQuery q);
};
typedef Relation::const_iterator RelationIterator;



class RelationStart : public vector<RecordStart>
{
public:
    RelationStart();
    void sortByStart();
    void print(char c);
    ~RelationStart();
};
typedef RelationStart::const_iterator RelationStartIterator;



// Copy of RelationStart
class RelationEnd : public vector<RecordEnd>
{
public:
    RelationEnd();
    void sortByEnd();
    void print(char c);
    ~RelationEnd();
};
typedef RelationEnd::const_iterator RelationEndIterator;



class RelationId : public vector<RecordId>
{
public:
    RelationId();
    void print(char c);
    ~RelationId();
};
typedef RelationId::iterator RelationIdIterator;
#endif //_RELATION_H_
