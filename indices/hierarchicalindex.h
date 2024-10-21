#ifndef _HIERARCHICALINDEX_H_
#define _HIERARCHICALINDEX_H_

#include "../def_global.h"
#include "../containers/relation.h"



// Framework
class HierarchicalIndex
{
protected:
    size_t numIndexedRecords;
    unsigned int numBits;
    unsigned int maxBits;
    unsigned int height;
    
    // Construction
    virtual inline void updateCounters(const Record &r) {};
    virtual inline void updatePartitions(const Record &r) {};

public:
    // Statistics
    size_t numPartitions;
    size_t numEmptyPartitions;
    float avgPartitionSize;
    size_t numOriginals, numReplicas;
    size_t numOriginalsIn, numOriginalsAft, numReplicasIn, numReplicasAft;


    // Construction
    HierarchicalIndex(const Relation &R, const unsigned int numBits, const unsigned int maxBits);
    virtual void computeUninformedBoundsMetadata() {};
    virtual void computeUninformedBoundsMetadataOriginalSpace() {};
    virtual void print(const char c) {};
    virtual void getStats() {};
    virtual size_t getSize() {};
    virtual ~HierarchicalIndex() {};
    

    // Querying
    // HINT
    // Basic predicates of Allen's algebra
    virtual size_t execute_Equals(const RangeQuery &q) {return 0;};
    virtual size_t execute_Starts(const RangeQuery &q) {return 0;};
    virtual size_t execute_Started(const RangeQuery &q) {return 0;};
    virtual size_t execute_Finishes(const RangeQuery &q) {return 0;};
    virtual size_t execute_Finished(const RangeQuery &q) {return 0;};
    virtual size_t execute_Meets(const RangeQuery &q) {return 0;};
    virtual size_t execute_Met(const RangeQuery &q) {return 0;};
    virtual size_t execute_Overlaps(const RangeQuery &q) {return 0;};
    virtual size_t execute_Overlapped(const RangeQuery &q) {return 0;};
    virtual size_t execute_Contains(const RangeQuery &q) {return 0;};
    virtual size_t execute_Contained(const RangeQuery &q) {return 0;};
    virtual size_t execute_Precedes(const RangeQuery &q) {return 0;};
    virtual size_t execute_Preceded(const RangeQuery &q) {return 0;};
    
    // Generalized predicate, ACM SIGMOD'22 gOverlaps
    virtual size_t execute_gOverlaps(const StabbingQuery &q) {return 0;};
    virtual size_t execute_gOverlaps(const RangeQuery &q) {return 0;};

    
    // HINT^m
    // Basic predicates of Allen's algebra
    virtual size_t executeBottomUp_Equals(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Starts(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Started(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Finishes(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Finished(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Meets(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Met(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Overlaps(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Overlapped(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Contains(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Contained(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Precedes(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_Preceded(const RangeQuery &q) {return 0;};

    // Generalized predicate, ACM SIGMOD'22 gOverlaps
    virtual size_t executeTopDown_gOverlaps(const StabbingQuery &q) {return 0;};
    virtual size_t executeTopDown_gOverlaps(const RangeQuery &q) {return 0;};
    virtual size_t executeBottomUp_gOverlaps(const StabbingQuery &q) {return 0;};
    virtual size_t executeBottomUp_gOverlaps(const RangeQuery &q) {return 0;};
    
    // Theta-relevance
    virtual size_t executeTopDown_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeTopDown_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeTopDown_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeTopDown_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};

    virtual size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta) {return 0;};
    
    virtual size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta) {return 0;};

    virtual size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta) {return 0;};

    virtual size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta) {return 0;};
    virtual size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta) {return 0;};

    // Top-k relevance
    virtual void executeTopDown_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeTopDown_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeTopDown_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeTopDown_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };

    virtual void executeBottomUp_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &results) { };
    virtual void executeBottomUp_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &results) { };
    virtual void executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &results) { };
    virtual void executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &results) { };
    
    virtual void executeBottomUp_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBottomUp_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };

    virtual void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };

    virtual void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };
    virtual void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result) { };

    
    virtual void executeBottomUp_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) {};
    
//    virtual size_t executeBottomUp_RelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computerRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) {return 0;};
//    
//    virtual size_t executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computerRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) {return 0;};
//    
//    
//    
//    virtual size_t executeBottomUp_RelativeRecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computerRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) {return 0;};
//    virtual size_t executeBottomUp_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computerRelevanceLowerBound_MinStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceLowerBound_MinStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceLowerBound_MaxStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceLowerBound_MaxStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computerRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) {return 0;};
//    virtual size_t executeBottomUp_RelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&)) {return 0;};
    
    
//    virtual size_t executeBottomUp_BoundsBasic_RelevanceThreshold(const RangeQuery &q, const float theta. float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&)) {return 0;};
//    
//    virtual priority_queue<float, vector<float>, greater<float>> executeBottomUp_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&)) { };
//    virtual priority_queue<float, vector<float>, greater<float>> executeBottomUp_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computerRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)) { };
};
#endif // _HIERARCHICALINDEX_H_
