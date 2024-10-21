#include "intervaltree.h"
#include "../utils_inlines.h"


//#define USE_RECURSION     // Slower!

// externs
#ifdef STATS
size_t prunedRecords;
size_t freeResults;
#endif


#define PQUEUE

class QEntry
{
public:
    IntervalTreeNode *node;
    float lowerBound;
    float upperBound;
    bool performCheck;
    bool checkWhat;
    
    QEntry(IntervalTreeNode *node, float lb, float ub, bool check, bool what)
    {
        this->node = node;
        this->lowerBound = lb;
        this->upperBound = ub;
        this->performCheck = check;
        this->checkWhat = what;
    };
    
    bool operator < (const QEntry& rhs) const
    {
        if (this->upperBound != rhs.upperBound)
            return (this->upperBound < rhs.upperBound);
        else
            return (this->lowerBound < rhs.lowerBound);
    };
};

bool compareQEntries(const QEntry &lhs, const QEntry &rhs)
{
    if (lhs.upperBound != rhs.upperBound)
        return (lhs.upperBound > rhs.upperBound);
    else
        return (lhs.lowerBound > rhs.lowerBound);

}


IntervalTreeNode::IntervalTreeNode()
{
    this->left = NULL;
    this->right = NULL;
}


IntervalTreeNode::IntervalTreeNode(unsigned int level, const Relation &R)
{
    RelationIterator iterBegin = R.begin();
    RelationIterator iterEnd = R.end();
    Relation recordsL;


//    cout << "Level = " << level << " index " << (iterEnd-iterBegin) << " records: from <r" << iterBegin->id << "," << iterBegin->start << "," << iterBegin->end << "> to <r" << (iterEnd-1)->id << "," << (iterEnd-1)->start << "," << (iterEnd-1)->end<< ">" << endl;

    // Set the level of the node.
    this->level = level;
    
    // Determine center
    Timestamp gstart = min_element(iterBegin, iterEnd)->start;
    Timestamp gend = max_element(iterBegin, iterEnd, CompareByEnd)->end; // TODO by end
    this->center = gstart+ceil((gend-gstart) / 2.0);
    
//    cout << "\tgstart = " << gstart << endl;
//    cout << "\tgend   = " << gend << endl;
//    cout << "\tcenter = " << this->center << endl;
    
    // Determine the last record in the left subtree
    RelationIterator iterBeginR = lower_bound(iterBegin, iterEnd, Record(0, center, center));
//    iterBeginR->print('r');

    
    for (auto iter = iterBegin; iter != iterBeginR; iter++)
    {
        if (iter->end >= this->center)
        {
            this->recordsByStart.emplace_back(iter->id, iter->start, iter->end);
            this->recordsByEnd.emplace_back(iter->id, iter->start, iter->end);
        }
        else
            recordsL.emplace_back(iter->id, iter->start, iter->end);
    }

    // Relation recordsByStart is by construction sorted by start, hence sort recordsByEnd
    this->recordsByEnd.sortByEnd();

//    cout << "\t# records: " << this->recordsByStart.size() << endl << endl;

//    if (level == 2)
//    {
//        this->left = this->right = NULL;
//        return;
//    }

    // Create subtrees
    if (!recordsL.empty())
        this->left = new IntervalTreeNode(this->level+1, recordsL);
    else
        this->left = NULL;
    
    if (iterBeginR != iterEnd)
        this->right = new IntervalTreeNode(this->level+1, Relation(iterBeginR, iterEnd));
    else
        this->right = NULL;
}


// For now only informed metadata on original domain are supported
IntervalTreeNode::IntervalTreeNode(unsigned int level, const Relation &R, const short int typeMetadata)
{
    RelationIterator iterBegin = R.begin();
    RelationIterator iterEnd = R.end();
    Relation recordsL;


    // Set the level of the node.
    this->level = level;
    
    // Determine center
    Timestamp gstart = min_element(iterBegin, iterEnd)->start;
    Timestamp gend = max_element(iterBegin, iterEnd, CompareByEnd)->end; // TODO by end
    this->center = gstart+ceil((gend-gstart) / 2.0);
        
    // Determine the last record in the left subtree
    RelationIterator iterBeginR = lower_bound(iterBegin, iterEnd, Record(0, center, center));
    
    this->typeMetadata = typeMetadata;
    if (this->typeMetadata == METADATA_INFORMED_ORIGINAL_DOMAIN)
    {
        for (auto iter = iterBegin; iter != iterBeginR; iter++)
        {
            if (iter->end >= this->center)
            {
                this->recordsByStart.emplace_back(iter->id, iter->start, iter->end);
                this->recordsByEnd.emplace_back(iter->id, iter->start, iter->end);
                
                this->metadata.minStart = min(this->metadata.minStart, iter->start);
                this->metadata.maxStart = max(this->metadata.maxStart, iter->start);
                this->metadata.minEnd = min(this->metadata.minEnd, iter->end);
                this->metadata.maxEnd = max(this->metadata.maxEnd, iter->end);
            }
            else
                recordsL.emplace_back(iter->id, iter->start, iter->end);
        }
    }
    else
    {
        for (auto iter = iterBegin; iter != iterBeginR; iter++)
        {
            if (iter->end >= this->center)
            {
                this->recordsByStart.emplace_back(iter->id, iter->start, iter->end);
                this->recordsByEnd.emplace_back(iter->id, iter->start, iter->end);
            }
            else
                recordsL.emplace_back(iter->id, iter->start, iter->end);
        }
    }

    // Relation recordsByStart is by construction sorted by start, hence sort recordsByEnd
    this->recordsByEnd.sortByEnd();

    // Create subtrees
    if (!recordsL.empty())
        this->left = new IntervalTreeNode(this->level+1, recordsL, typeMetadata);
    else
        this->left = NULL;
    
    if (iterBeginR != iterEnd)
        this->right = new IntervalTreeNode(this->level+1, Relation(iterBeginR, iterEnd), typeMetadata);
    else
        this->right = NULL;
}


void IntervalTreeNode::print(const char c)
{
    cout << "Level = " << this->level << "\tcenter = " << this->center << endl;
    
//    cout << "\tByStart = {";
//    this->recordsByStart.print(c);
//    cout << "}" << endl;
    cout << "\tByStart (" << this->recordsByStart.size() << ")";
    
//    cout << "\tByEnd = {";
//    this->recordsByEnd.print(c);
//    cout << "}" << endl;
    cout << "\tByEnd (" << this->recordsByEnd.size() << ")";
    cout << endl;

    if (this->left)
    {
        cout << "left"<<endl;
        this->left->print(c);
    }

    if (this->right)
    {
        cout << "right"<<endl;
        this->right->print(c);
    }
}


IntervalTreeNode::~IntervalTreeNode()
{
    if (this->left)
        delete this->left;
    
    if (this->right)
        delete this->right;
}


inline void IntervalTreeNode::scan_NoChecks_gOverlaps(size_t &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= iter->id;
#endif
    }
}

inline void IntervalTreeNode::scan_CheckStart_gOverlaps(const RangeQuery &q, size_t &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    RelationIterator pivot = upper_bound(iterBegin, iterEnd, Record(0, q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != pivot; iter++)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= iter->id;
#endif
    }
}

inline void IntervalTreeNode::scan_CheckEnd_gOverlaps(const RangeQuery &q, size_t &result)
{
    RelationIterator iterBegin = this->recordsByEnd.begin();
    RelationIterator iterEnd = this->recordsByEnd.end();
    RelationIterator pivot = lower_bound(iterBegin, iterEnd, Record(0, q.start, q.start), CompareRecordsByEnd);
    
    for (auto iter = pivot; iter != iterEnd; iter++)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= iter->id;
#endif
    }
}


inline void IntervalTreeNode::scan_NoChecks_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= iter->id;
#endif
        }
    }
}

inline void IntervalTreeNode::scan_CheckStart_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    RelationIterator pivot = upper_bound(iterBegin, iterEnd, Record(0, q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != pivot; iter++)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= iter->id;
#endif
        }
    }
}

inline void IntervalTreeNode::scan_CheckEnd_gOverlaps(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), size_t &result)
{
    RelationIterator iterBegin = this->recordsByEnd.begin();
    RelationIterator iterEnd = this->recordsByEnd.end();
    RelationIterator pivot = lower_bound(iterBegin, iterEnd, Record(0, q.start, q.start), CompareRecordsByEnd);
    
    for (auto iter = pivot; iter != iterEnd; iter++)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= iter->id;
#endif
        }
    }
}


inline void IntervalTreeNode::scan_NoChecks_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
        updateTopK(result, k, computeRelevance(*iter, q));
}

inline void IntervalTreeNode::scan_CheckStart_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    RelationIterator iterBegin = this->recordsByStart.begin();
    RelationIterator iterEnd = this->recordsByStart.end();
    RelationIterator pivot = upper_bound(iterBegin, iterEnd, Record(0, q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != pivot; iter++)
        updateTopK(result, k, computeRelevance(*iter, q));
}

inline void IntervalTreeNode::scan_CheckEnd_gOverlaps(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    RelationIterator iterBegin = this->recordsByEnd.begin();
    RelationIterator iterEnd = this->recordsByEnd.end();
    RelationIterator pivot = lower_bound(iterBegin, iterEnd, Record(0, q.start, q.start), CompareRecordsByEnd);
    
    for (auto iter = pivot; iter != iterEnd; iter++)
        updateTopK(result, k, computeRelevance(*iter, q));
}


void IntervalTreeNode::executeDepthFirst_gOverlaps(const RangeQuery &q, size_t &result)
{
    if (q.start <= this->center)
    {
        if (this->center <= q.end)
        {
            // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
            for (const Record &r: this->recordsByStart)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= r.id;
#endif
            }
            
            if (this->right)
            {
//                    cout << "\tadd right node: level = " << curr->right->level << "\tcenter = " << curr->right->center << endl;
                this->right->executeDepthFirst_gOverlaps(q, result);
            }
        }
        else
        {
            for (const Record &r: this->recordsByStart)
            {
                if ((r.start <= q.end) && (q.start <= r.end))
                {
#ifdef WORKLOAD_COUNT
                    result++;
#else
                    result ^= r.id;
#endif
                }
            }
        }

        if (this->left)
        {
//                cout << "\tadd left node: level = " << curr->left->level << "\tcenter = " << curr->left->center << endl;
            this->left->executeDepthFirst_gOverlaps(q, result);
        }
    }
    else
    {
        for (const Record &r: this->recordsByEnd)
        {
            if ((r.start <= q.end) && (q.start <= r.end))
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= r.id;
#endif
            }
        }

        if (this->right)
        {
//                cout << "\tadd right node: level = " << curr->right->level << "\tcenter = " << curr->right->center << endl;
            this->right->executeDepthFirst_gOverlaps(q, result);
        }
    }
}


IntervalTree::IntervalTree(const Relation &R)
{
    this->typeMetadata = METADATA_NO;
    this->root = new IntervalTreeNode(0, R);
}


IntervalTree::IntervalTree(const Relation &R, const short int typeMetadata)
{
    this->typeMetadata = typeMetadata;
    this->root = new IntervalTreeNode(0, R, typeMetadata);
}


void IntervalTree::print(const char c)
{
    this->root->print(c);
}


void IntervalTree::getStats()
{
    size_t numIndexedRecords = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode *curr;
    
    this->numLevels = 0;
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();

        if (curr->level > this->numLevels)
            this->numLevels = curr->level;
        if (!curr->recordsByStart.empty())
            numIndexedRecords += curr->recordsByStart.size();
        
        if (curr->left)
            S.push(curr->left);
        if (curr->right)
            S.push(curr->right);
    }
    this->numLevels++;  // count root
    
//    cout<<numIndexedRecords<<endl;
}


size_t IntervalTree::getSize()
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode *curr;
    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();

        result += (curr->recordsByStart.size()+curr->recordsByStart.size()) * (sizeof(RecordId)+sizeof(Timestamp)+sizeof(Timestamp));
        
        if (this->typeMetadata != METADATA_NO)
            result += 4 * sizeof(Timestamp);
        
        if (curr->left)
            S.push(curr->left);
        if (curr->right)
            S.push(curr->right);
    }    

    return result;
}


IntervalTree::~IntervalTree()
{
    delete this->root;
}


size_t IntervalTree::executeDepthFirst_gOverlaps(const RangeQuery &q)
#ifdef USE_RECURSION
{
    size_t result = 0;
    
    this->root->executeDepthFirst_gOverlaps(q, result);
    
    return result;
}
#else
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                curr->scan_NoChecks_gOverlaps(result);
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
                curr->scan_CheckStart_gOverlaps(q, result);

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            curr->scan_CheckEnd_gOverlaps(q, result);

            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}
#endif



// Theta relevance: top-down search using no bounds
inline size_t IntervalTree::executeDepthFirst_gOverlaps_RelevanceThreshold(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&))
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                curr->scan_NoChecks_gOverlaps(q, theta, computeRelevance, result);
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
                curr->scan_CheckStart_gOverlaps(q, theta, computeRelevance, result);

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            curr->scan_CheckEnd_gOverlaps(q, theta, computeRelevance, result);

            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}

size_t IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, float const theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold(q, theta, computeAbsoluteRelevance);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, float const theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold(q, theta, computeRelativeRelevance);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold(q, theta, computeRecordRelativeRelevance);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold(q, theta, computeQueryRelativeRelevance);
}


// Theta relevance: top-down search using lower bounds
inline size_t IntervalTree::executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&))
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        curr->scan_NoChecks_gOverlaps(result);
                    else
                        curr->scan_NoChecks_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        freeResults += curr->recordsByStart.size();
#endif
                }
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                        curr->scan_CheckStart_gOverlaps(q, result);
                        curr->scan_NoChecks_gOverlaps(result);
                    else
                        curr->scan_CheckStart_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        freeResults += curr->recordsByStart.size();
#endif
                }
            }

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
            {
                if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                    curr->scan_CheckEnd_gOverlaps(q, result);
                    curr->scan_NoChecks_gOverlaps(result);
                else
                    curr->scan_CheckEnd_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                    freeResults += curr->recordsByEnd.size();
#endif
            }
            
            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}

size_t IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceLowerBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound);
}


// Theta relevance: top-down search using upper bounds
inline size_t IntervalTree::executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&))
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();

        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
                    curr->scan_NoChecks_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) <= theta))
                    prunedRecords += curr->recordsByStart.size();
#endif
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
                    curr->scan_CheckStart_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) <= theta))
                    prunedRecords += curr->recordsByStart.size();
#endif
            }

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if ((!curr->recordsByEnd.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
                curr->scan_CheckEnd_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
            if ((!curr->recordsByEnd.empty()) && (computeRelevanceUpperBound(curr->metadata, q) <= theta))
                prunedRecords += curr->recordsByEnd.size();
#endif

            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}

size_t IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound);
}


// Theta relevance: top-down search using both lower and upper bounds
inline size_t IntervalTree::executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&))
#ifdef LU
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        curr->scan_NoChecks_gOverlaps(result);
                    else if (computeRelevanceUpperBound(curr->metadata, q) > theta)
                        curr->scan_NoChecks_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        freeResults += curr->recordsByStart.size();
                    else if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                        prunedRecords += curr->recordsByStart.size();
#endif
                }
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                        curr->scan_CheckStart_gOverlaps(q, result);
                        curr->scan_NoChecks_gOverlaps(result);
                    else if (computeRelevanceUpperBound(curr->metadata, q) > theta)
                        curr->scan_CheckStart_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        freeResults += curr->recordsByStart.size();
                    else if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                        prunedRecords += curr->recordsByStart.size();
#endif
                }
            }

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
            {
                if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                    curr->scan_CheckEnd_gOverlaps(q, result);
                    curr->scan_NoChecks_gOverlaps(result);
                else if (computeRelevanceUpperBound(curr->metadata, q) > theta)
                    curr->scan_CheckEnd_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                    freeResults += curr->recordsByEnd.size();
                else if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                    prunedRecords += curr->recordsByEnd.size();
#endif
            }
            
            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}
#elif defined(UL)
{
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;
    size_t result = 0;
    RelationIterator iterBegin, iterEnd;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();

        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                        curr->scan_NoChecks_gOverlaps(result);
                    else
                        curr->scan_NoChecks_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (!curr->recordsByStart.empty())
                    {
                        if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                            prunedRecords += curr->recordsByStart.size();
                        else if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                            freeResults += curr->recordsByStart.size();
                    }
#endif
                }
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if ((!curr->recordsByStart.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
                {
                    if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                        curr->scan_CheckStart_gOverlaps(q, result);
                        curr->scan_NoChecks_gOverlaps(result);
                    else
                        curr->scan_CheckStart_gOverlaps(q, theta, computeRelevance, result);
#ifdef STATS
                    if (!curr->recordsByStart.empty())
                    {
                        if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                            prunedRecords += curr->recordsByStart.size();
                        else if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                            freeResults += curr->recordsByStart.size();
                    }
#endif
                }
            }

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if ((!curr->recordsByEnd.empty()) && (computeRelevanceUpperBound(curr->metadata, q) > theta))
            {
                if (computeRelevanceLowerBound(curr->metadata, q) > theta)
//                    curr->scan_CheckEnd_gOverlaps(q, result);
                    curr->scan_NoChecks_gOverlaps(result);
                else
                    curr->scan_CheckEnd_gOverlaps(q, theta, computeRelevance, result);
            }
#ifdef STATS
            if (!curr->recordsByEnd.empty())
            {
                if (computeRelevanceUpperBound(curr->metadata, q) <= theta)
                    prunedRecords += curr->recordsByEnd.size();
                else if (computeRelevanceLowerBound(curr->metadata, q) > theta)
                    freeResults += curr->recordsByEnd.size();
            }
#endif

            if (curr->right)
                S.push(curr->right);
        }
    }

    
    return result;
}
#endif

size_t IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound);
}

size_t IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeDepthFirst_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound);
}



// Top-k relevance: top-down search using no bounds
inline void IntervalTree::executeDepthFirst_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;
    float qrel = computeRelevance(Record(0, q.start, q.end), q);

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                curr->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                curr->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            curr->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if (curr->right)
                S.push(curr->right);
        }
    }
}

void IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK(q, k, computeAbsoluteRelevance, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK(q, k, computeRelativeRelevance, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK(q, k, computeRecordRelativeRelevance, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK(q, k, computeQueryRelativeRelevance, result);
}


// Top-k relevance: top-down search using upper bounds
inline void IntervalTree::executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;
    float qrel = computeRelevance(Record(0, q.start, q.end), q);

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
//                cout<<"level = " << curr->level << "\tcenter = " << curr->center<<"\t(" << curr->recordsByStart.size() << "): UB = " << computeRelevanceUpperBound(curr->metadata, q) << " with theta = " << ((result.size() == k) ? result.top(): 0) << endl;
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if ((!curr->recordsByStart.empty()) && ((result.size() < k) || (computeRelevanceUpperBound(curr->metadata, q) > result.top())))
                    curr->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
//                cout<<"level = " << curr->level << "\tcenter = " << curr->center<<"\t(" << curr->recordsByStart.size() << "): UB = " << computeRelevanceUpperBound(curr->metadata, q) << " with theta = " << ((result.size() == k)? result.top(): 0) << endl;
                if ((!curr->recordsByStart.empty()) && ((result.size() < k) || (computeRelevanceUpperBound(curr->metadata, q) > result.top())))
                    curr->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
//            cout<<"level = " << curr->level << "\tcenter = " << curr->center<<"\t(" << curr->recordsByStart.size() << "): UB = " << computeRelevanceUpperBound(curr->metadata, q) << " with theta = " << ((result.size() == k)? result.top(): 0) << endl;
            if ((!curr->recordsByEnd.empty()) && ((result.size() < k) || (computeRelevanceUpperBound(curr->metadata, q) > result.top())))
                curr->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if (curr->right)
                S.push(curr->right);
        }
    }
}

void IntervalTree::executeDepthFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeDepthFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeDepthFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound, result);
}


// Top-k relevance: best-first search using upper bounds
inline void IntervalTree::executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
#ifdef PQUEUE
{
    priority_queue<QEntry, vector<QEntry>> Q;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                    Q.push(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), false, false));
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                    Q.push(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), true, true));
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
                Q.push(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), true, false));

            if (curr->right)
                S.push(curr->right);
        }
    }
    
    while (!Q.empty())
    {
        QEntry e = Q.top();
//        cout << e.node->center << "\t" << e.upperBound << "\t" << e.lowerBound << endl;
        
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;
        Q.pop();
        
        if (e.performCheck)
        {
            if (e.checkWhat) // check start
            {
                e.node->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
            }
            else // check end
            {
                e.node->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
            }
        }
        else
            e.node->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
    }
}
#else
{
    vector<QEntry> Q;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                    Q.push_back(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), false, false));
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                    Q.push_back(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), true, true));
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
                Q.push_back(QEntry(curr, 0, computeRelevanceUpperBound(curr->metadata, q), true, false));

            if (curr->right)
                S.push(curr->right);
        }
    }
    
    sort(Q.begin(), Q.end(), compareQEntries);
    
    for (const QEntry &e: Q)
    {
//        cout << e.node->center << "\t" << e.upperBound << "\t" << e.lowerBound << endl;
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;
        
        if (e.performCheck)
        {
            if (e.checkWhat) // check start
            {
                e.node->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
            }
            else // check end
            {
                e.node->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
            }
        }
        else
            e.node->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
    }
}
#endif

void IntervalTree::executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound, result);
}


// Top-k relevance: best-first search using both lower and upper bounds
inline void IntervalTree::executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
#ifdef PQUEUE
{
    priority_queue<QEntry, vector<QEntry>> Q;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                    Q.push(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), false, false));
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                    Q.push(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), true, true));
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
                Q.push(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), true, false));

            if (curr->right)
                S.push(curr->right);
        }
    }
    
    while (!Q.empty())
    {
        QEntry e = Q.top();
        
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;
        Q.pop();
        
        if (e.performCheck)
        {
            if (e.checkWhat) // check start
            {
                e.node->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
            }
            else // check end
            {
                e.node->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
            }
        }
        else
            e.node->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
    }
}
#else
{
    vector<QEntry> Q;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                    Q.push_back(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), false, false));
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                    Q.push_back(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), true, true));
            }
            
            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
                Q.push_back(QEntry(curr, computeRelevanceLowerBound(curr->metadata, q), computeRelevanceUpperBound(curr->metadata, q), true, false));

            if (curr->right)
                S.push(curr->right);
        }
    }
    
    sort(Q.begin(), Q.end(), compareQEntries);
    
    for (const QEntry &e: Q)
    {
//        cout << e.node->center << "\t" << e.upperBound << "\t" << e.lowerBound << endl;
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;
        
        if (e.performCheck)
        {
            if (e.checkWhat) // check start
            {
                e.node->scan_CheckStart_gOverlaps(q, k, computeRelevance, result);
            }
            else // check end
            {
                e.node->scan_CheckEnd_gOverlaps(q, k, computeRelevance, result);
            }
        }
        else
            e.node->scan_NoChecks_gOverlaps(q, k, computeRelevance, result);
    }
}
#endif

void IntervalTree::executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound, result);
}

void IntervalTree::executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound, result);
}


void IntervalTree::executeDepthFirst_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const Record&, const RangeQuery&), float (*computeRelevanceLowerBound)(Metadata &md, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &md, const RangeQuery&))
{
    size_t result = 0;
    stack<IntervalTreeNode*> S;
    IntervalTreeNode* curr;

    
    S.push(this->root);
    while (!S.empty())
    {
        curr = S.top();
        S.pop();
        
        if (q.start <= curr->center)
        {
            if (curr->center <= q.end)
            {
                // No interval comparisons needed, all contents overlap query; doesn't matter which of recordsByStart or recordsByEnd is scanned.
                if (!curr->recordsByStart.empty())
                {
                    RelationIterator iterBegin = curr->recordsByStart.begin();
                    RelationIterator iterEnd = curr->recordsByStart.end();
                    float LB = computeRelevance(Record(0, q.start, q.end), q);
                    float UB = 0;
                    
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        auto rel = computeRelevance(*iter, q);
                        
                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                    }

//                    cout << curr->level << "\t" << curr->center << "\t" << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << endl;
//                    cout << curr->level << "\t" << curr->center << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q))/UB << endl;
                    cout << curr->level << "\t" << curr->center << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" <<  abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << endl;
                }
                
                if (curr->right)
                    S.push(curr->right);
            }
            else
            {
                if (!curr->recordsByStart.empty())
                {
                    RelationIterator iterBegin = curr->recordsByStart.begin();
                    RelationIterator iterEnd = curr->recordsByStart.end();
//                    RelationIterator pivot = iterEnd;
                    RelationIterator pivot = upper_bound(iterBegin, iterEnd, Record(0, q.end+1, q.end+1));
                    float LB = computeRelevance(Record(0, q.start, q.end), q);
                    float UB = 0;
                    
                    for (auto iter = iterBegin; iter != pivot; iter++)
                    {
                        auto rel = computeRelevance(*iter, q);
                        
                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                    }
                    if (iterBegin == pivot)
                        LB = UB = 0;

//                    cout << curr->level << "\t" << curr->center << "\t" << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << endl;
//                    cout << curr->level << "\t" << curr->center << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q))/UB << endl;
                    cout << curr->level << "\t" << curr->center << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" <<  abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << endl;
                }
            }

            if (curr->left)
                S.push(curr->left);
        }
        else
        {
            if (!curr->recordsByEnd.empty())
            {
                RelationIterator iterBegin = curr->recordsByEnd.begin();
                RelationIterator iterEnd = curr->recordsByEnd.end();
//                RelationIterator pivot = iterBegin;
                RelationIterator pivot = lower_bound(iterBegin, iterEnd, Record(0, q.start, q.start), CompareRecordsByEnd);
                float LB = computeRelevance(Record(0, q.start, q.end), q);
                float UB = 0;
                
                for (auto iter = pivot; iter != iterEnd; iter++)
                {
                    auto rel = computeRelevance(*iter, q);
                    
                    if (rel < LB)
                        LB = rel;
                    if (rel > UB)
                        UB = rel;
                }
                if (pivot == iterEnd)
                    LB = UB = 0;
                
//                cout << curr->level << "\t" << curr->center << "\t" << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << endl;
//                cout << curr->level << "\t" << curr->center << "\t";
//                cout << LB << "\t" << computeRelevanceLowerBound(curr->metadata, q) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q))/LB << "\t";
//                cout << UB << "\t" << computeRelevanceUpperBound(curr->metadata, q) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << "\t" << abs(UB-computeRelevanceUpperBound(curr->metadata, q))/UB << endl;
                cout << curr->level << "\t" << curr->center << "\t" << abs(LB-computeRelevanceLowerBound(curr->metadata, q)) << "\t" <<  abs(UB-computeRelevanceUpperBound(curr->metadata, q)) << endl;
            }

            if (curr->right)
                S.push(curr->right);
        }
    }
}
