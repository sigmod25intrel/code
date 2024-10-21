#ifndef _INTERVAL_TREE_H_
#define _INTERVAL_TREE_H_

#include "../def_global.h"
#include "../containers/relation.h"



class IntervalTreeNode
{
public:
    Timestamp center;
    unsigned int level;
    IntervalTreeNode *left, *right;
    Relation recordsByStart;
    Relation recordsByEnd;
    
    short int typeMetadata;
    Metadata metadata;

    // Construction
    IntervalTreeNode();
    IntervalTreeNode(unsigned int level, const Relation &R);
    IntervalTreeNode(unsigned int level, const Relation &R, const short int typeMetadata);
    void print(const char c);
    ~IntervalTreeNode();
    
    // Querying
    inline void scan_NoChecks_gOverlaps(size_t &result);
    inline void scan_CheckStart_gOverlaps(const RangeQuery &q, size_t &result);
    inline void scan_CheckEnd_gOverlaps(const RangeQuery &q, size_t &result);

    inline void scan_NoChecks_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result);
    inline void scan_CheckStart_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result);
    inline void scan_CheckEnd_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result);

    inline void scan_NoChecks_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scan_CheckStart_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    inline void scan_CheckEnd_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);

    void executeDepthFirst_gOverlaps(const RangeQuery &q, size_t &result);
};


class IntervalTree
{
private:
    IntervalTreeNode *root;
    short int typeMetadata;
    
public:
    unsigned int numLevels;
    
    // Construction
    IntervalTree(const Relation &R);
    IntervalTree(const Relation &R, const short int typeMetadata);
    void print(const char c);
    void getStats();
    size_t getSize();
    ~IntervalTree();
    
    // Querying
    size_t executeDepthFirst_gOverlaps(const RangeQuery &q);
    
    // Theta relevance
    inline size_t executeDepthFirst_gOverlaps_RelevanceThreshold(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&));
    size_t executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta);
    
    inline size_t executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(const RangeQuery &q, float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&));
    size_t executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta);
    
    inline size_t executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&));
    size_t executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta);
    
    inline size_t executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&));
    size_t executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    size_t executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta);
    
    // Top-k relevance
    inline void executeDepthFirst_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeDepthFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    inline void executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    void executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result);
    
    void executeDepthFirst_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&));
};
#endif //_INTERVAL_TREE_H_
