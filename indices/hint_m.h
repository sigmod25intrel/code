#ifndef _HINT_M_H_
#define _HINT_M_H_

#include "../def_global.h"
#include "../containers/relation.h"
#include "../indices/hierarchicalindex.h"


// HINT^m with subs+sort and cash misses optimizations activated
class HINT_M_SubsSort_CM : public HierarchicalIndex
{
private:
    Relation      **pOrgsInTmp;
    Relation      **pOrgsAftTmp;
    Relation      **pRepsInTmp;
    Relation      **pRepsAftTmp;
    
    RelationId    **pOrgsInIds;
    RelationId    **pOrgsAftIds;
    RelationId    **pRepsInIds;
    RelationId    **pRepsAftIds;
    vector<pair<Timestamp, Timestamp> > **pOrgsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pOrgsAftTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsInTimestamps;
    vector<pair<Timestamp, Timestamp> > **pRepsAftTimestamps;
    
    // Metadata for bounds
    short int typeMetadata;
    Metadata    **pOrgsInMetadata;
    Metadata    **pOrgsAftMetadata;
    Metadata    **pRepsInMetadata;
    Metadata    **pRepsAftMetadata;
    
    RecordId      **pOrgsIn_sizes, **pOrgsAft_sizes;
    size_t        **pRepsIn_sizes, **pRepsAft_sizes;
    
    
    // Construction
    inline void updateCounters(const Record &r);
    inline void updatePartitions(const Record &r);
    inline void updatePartitions_withBounds(const Record &r);
    
    // Querying
    // Auxiliary functions to scan a partition.
    inline void scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, size_t &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result);
    inline void scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result);
    inline void scanPartition_NoChecks_gOverlaps(RelationId &ids, size_t &result);
//    inline void scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, size_t &result);
    
    inline void scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
    inline void scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
    inline void scanPartition_NoChecks_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
//    inline void scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, RangeQuery &Q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result);
    
    inline void scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scanPartition_NoChecks_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
//    inline void scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, RangeQuery &Q, unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    
    
    //    pair<Timestamp, Timestamp> getBestInterval(unsigned int level, Timestamp t, Metadata **md, Timestamp a, Timestamp b);
    //    Timestamp computeUpperBoundAbsoluteRelevance(unsigned int level, Timestamp t, Metadata **md, Timestamp a, Timestamp b);
    
public:
    // Construction
    HINT_M_SubsSort_CM(const Relation &R, const unsigned int numBits, const unsigned int maxBits, const short int typeMetadata);
    void computeUninformedBoundsMetadata();
    void computeUninformedBoundsMetadataOriginalSpace();
    void getStats();
    size_t getSize();
    ~HINT_M_SubsSort_CM();
    
    // Querying
    size_t executeBottomUp_gOverlaps(const RangeQuery &q);
    
    // Theta relevance
    inline size_t executeBottomUp_gOverlaps_RelevanceThreshold(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&));
    size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    
    inline size_t executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    
    inline size_t executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &, const RangeQuery&));
    size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    
    inline size_t executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    //    size_t executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    //    size_t executeBottomUp_RelativeRecordRelativeRelevanceThreshold_UsingBothBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound_MinStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MinStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MaxStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MaxStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    size_t executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    
    
    //    size_t executeBottomUp_RelevanceThreshold_UsingBothBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    //
    //    size_t executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    //    size_t executeBottomUp_RecordRelativeRelevanceThreshold_UsingBothBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound_MinStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MinStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MaxStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceLowerBound_MaxStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
    
    //    size_t executeBottomUp_RelevanceThreshold_UsingUpperBounds(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&));
    //    size_t executeBottomUp_AbsoluteRelevanceThreshold_UpperBounds(RangeQuery q, Timestamp theta);
    
    
    //    size_t executeBottomUp_RelevanceThreshold(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&));
    //    size_t executeBottomUp_BoundsBasic_RelevanceThreshold(RangeQuery q, float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&));
    //
    //    size_t executeBottomUp_UpperBounds_RelevanceThreshold(RangeQuery q, unsigned int theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&));
    
    // Top-k relevance
    inline void executeTopDown_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeTopDown_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeTopDown_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeTopDown_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeTopDown_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);

    inline void executeBottomUp_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBottomUp_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb));
};



// Comparators
inline bool CompareTimestampPairsByStart(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
{
    return (lhs.first < rhs.first);
}

inline bool CompareTimestampPairsByEnd(const pair<Timestamp, Timestamp> &lhs, const pair<Timestamp, Timestamp> &rhs)
{
    return (lhs.second < rhs.second);
}
#endif // _HINT_M_H_
