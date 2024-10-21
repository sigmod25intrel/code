#include "hint_m.h"
#include "../utils_inlines.h"


// externs
#ifdef STATS
size_t prunedIntervals[4];
size_t freeResults[4];
#endif


#define PQUEUE

class QEntry
{
public:
    unsigned int level;
    Timestamp pid;
    float lowerBound;
    float upperBound;
    bool isOriginal;
    bool isIn;
    bool performCheck;
    short int checkWhat;
    
    QEntry(unsigned int level, Timestamp pid, float lb, float ub, bool isOriginal, bool isIn, bool check, short int what)
    {
        this->level = level;
        this->pid = pid;
        this->lowerBound = lb;
        this->upperBound = ub;
        this->isOriginal = isOriginal;
        this->isIn = isIn;
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


inline void HINT_M_SubsSort_CM::updateCounters(const Record &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    
    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                    this->pRepsAft_sizes[level][a]++;
            }
            else
            {
                if ((a == b) && (!lastfound))
                    this->pOrgsIn_sizes[level][a]++;
                else
                    this->pOrgsAft_sizes[level][a]++;
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                    this->pOrgsIn_sizes[level][prevb]++;
                else
                    this->pOrgsAft_sizes[level][prevb]++;
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


inline void HINT_M_SubsSort_CM::updatePartitions(const Record &r)
//{
//    int level = 0;
//    Timestamp a = r.start >> (this->maxBits-this->numBits);
//    Timestamp b = r.end   >> (this->maxBits-this->numBits);
//    Timestamp prevb;
//    int firstfound = 0, lastfound = 0;
//
//
//    while (level < this->height && a <= b)
//    {
//        if (a%2)
//        { //last bit of a is 1
//            if (firstfound)
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][a]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][a]++;
//                }
//            }
//            else
//            {
//                if ((a == b) && (!lastfound))
//                {
//                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][a]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][a]++;
//                }
//                firstfound = 1;
//            }
//            a++;
//        }
//        if (!(b%2))
//        { //last bit of b is 0
//            prevb = b;
//            b--;
//            if ((!firstfound) && b < a)
//            {
//                if (!lastfound)
//                {
//                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsIn_sizes[level][prevb]++;
//                }
//                else
//                {
//                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pOrgsAft_sizes[level][prevb]++;
//                }
//            }
//            else
//            {
//                if (!lastfound)
//                {
//                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsIn_sizes[level][prevb]++;
//                    lastfound = 1;
//                }
//                else
//                {
//                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
//                    this->pRepsAft_sizes[level][prevb]++;
//                }
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//        level++;
//    }
//}
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    
    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);;
                    this->pRepsIn_sizes[level][a]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);;
                    this->pRepsAft_sizes[level][a]++;
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
                    this->pOrgsIn_sizes[level][a]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
                    this->pOrgsAft_sizes[level][a]++;
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pOrgsIn_sizes[level][prevb]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pOrgsAft_sizes[level][prevb]++;
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


inline void HINT_M_SubsSort_CM::updatePartitions_withBounds(const Record &r)
{
    int level = 0;
    Timestamp a = r.start >> (this->maxBits-this->numBits);
    Timestamp b = r.end   >> (this->maxBits-this->numBits);
    Timestamp prevb;
    int firstfound = 0, lastfound = 0;
    
    
    while (level < this->height && a <= b)
    {
        if (a%2)
        { //last bit of a is 1
            if (firstfound)
            {
                if ((a == b) && (!lastfound))
                {
                    this->pRepsInTmp[level][a][this->pRepsIn_sizes[level][a]] = Record(r.id, r.start, r.end);;
                    this->pRepsIn_sizes[level][a]++;
                    this->pRepsInMetadata[level][a].minStart = min(this->pRepsInMetadata[level][a].minStart, r.start);
                    this->pRepsInMetadata[level][a].maxStart = max(this->pRepsInMetadata[level][a].maxStart, r.start);
                    this->pRepsInMetadata[level][a].minEnd = min(this->pRepsInMetadata[level][a].minEnd, r.end);
                    this->pRepsInMetadata[level][a].maxEnd = max(this->pRepsInMetadata[level][a].maxEnd, r.end);
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][a][this->pRepsAft_sizes[level][a]] = Record(r.id, r.start, r.end);;
                    this->pRepsAftMetadata[level][a].minStart = min(this->pRepsAftMetadata[level][a].minStart, r.start);
                    this->pRepsAftMetadata[level][a].maxStart = max(this->pRepsAftMetadata[level][a].maxStart, r.start);
                    this->pRepsAftMetadata[level][a].minEnd = min(this->pRepsAftMetadata[level][a].minEnd, r.end);
                    this->pRepsAftMetadata[level][a].maxEnd = max(this->pRepsAftMetadata[level][a].maxEnd, r.end);
                    this->pRepsAft_sizes[level][a]++;
                }
            }
            else
            {
                if ((a == b) && (!lastfound))
                {
                    this->pOrgsInTmp[level][a][this->pOrgsIn_sizes[level][a]] = Record(r.id, r.start, r.end);
                    this->pOrgsInMetadata[level][a].minStart = min(this->pOrgsInMetadata[level][a].minStart, r.start);
                    this->pOrgsInMetadata[level][a].maxStart = max(this->pOrgsInMetadata[level][a].maxStart, r.start);
                    this->pOrgsInMetadata[level][a].minEnd = min(this->pOrgsInMetadata[level][a].minEnd, r.end);
                    this->pOrgsInMetadata[level][a].maxEnd = max(this->pOrgsInMetadata[level][a].maxEnd, r.end);
                    this->pOrgsIn_sizes[level][a]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][a][this->pOrgsAft_sizes[level][a]] = Record(r.id, r.start, r.end);
                    this->pOrgsAftMetadata[level][a].minStart = min(this->pOrgsAftMetadata[level][a].minStart, r.start);
                    this->pOrgsAftMetadata[level][a].maxStart = max(this->pOrgsAftMetadata[level][a].maxStart, r.start);
                    this->pOrgsAftMetadata[level][a].minEnd = min(this->pOrgsAftMetadata[level][a].minEnd, r.end);
                    this->pOrgsAftMetadata[level][a].maxEnd = max(this->pOrgsAftMetadata[level][a].maxEnd, r.end);
                    this->pOrgsAft_sizes[level][a]++;
                }
                firstfound = 1;
            }
            a++;
        }
        if (!(b%2))
        { //last bit of b is 0
            prevb = b;
            b--;
            if ((!firstfound) && b < a)
            {
                if (!lastfound)
                {
                    this->pOrgsInTmp[level][prevb][this->pOrgsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pOrgsInMetadata[level][prevb].minStart = min(this->pOrgsInMetadata[level][prevb].minStart, r.start);
                    this->pOrgsInMetadata[level][prevb].maxStart = max(this->pOrgsInMetadata[level][prevb].maxStart, r.start);
                    this->pOrgsInMetadata[level][prevb].minEnd = min(this->pOrgsInMetadata[level][prevb].minEnd, r.end);
                    this->pOrgsInMetadata[level][prevb].maxEnd = max(this->pOrgsInMetadata[level][prevb].maxEnd, r.end);
                    this->pOrgsIn_sizes[level][prevb]++;
                }
                else
                {
                    this->pOrgsAftTmp[level][prevb][this->pOrgsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pOrgsAftMetadata[level][prevb].minStart = min(this->pOrgsAftMetadata[level][prevb].minStart, r.start);
                    this->pOrgsAftMetadata[level][prevb].maxStart = max(this->pOrgsAftMetadata[level][prevb].maxStart, r.start);
                    this->pOrgsAftMetadata[level][prevb].minEnd = min(this->pOrgsAftMetadata[level][prevb].minEnd, r.end);
                    this->pOrgsAftMetadata[level][prevb].maxEnd = max(this->pOrgsAftMetadata[level][prevb].maxEnd, r.end);
                    this->pOrgsAft_sizes[level][prevb]++;
                }
            }
            else
            {
                if (!lastfound)
                {
                    this->pRepsInTmp[level][prevb][this->pRepsIn_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pRepsInMetadata[level][prevb].minStart = min(this->pRepsInMetadata[level][prevb].minStart, r.start);
                    this->pRepsInMetadata[level][prevb].maxStart = max(this->pRepsInMetadata[level][prevb].maxStart, r.start);
                    this->pRepsInMetadata[level][prevb].minEnd = min(this->pRepsInMetadata[level][prevb].minEnd, r.end);
                    this->pRepsInMetadata[level][prevb].maxEnd = max(this->pRepsInMetadata[level][prevb].maxEnd, r.end);
                    this->pRepsIn_sizes[level][prevb]++;
                    lastfound = 1;
                }
                else
                {
                    this->pRepsAftTmp[level][prevb][this->pRepsAft_sizes[level][prevb]] = Record(r.id, r.start, r.end);
                    this->pRepsAftMetadata[level][prevb].minStart = min(this->pRepsAftMetadata[level][prevb].minStart, r.start);
                    this->pRepsAftMetadata[level][prevb].maxStart = max(this->pRepsAftMetadata[level][prevb].maxStart, r.start);
                    this->pRepsAftMetadata[level][prevb].minEnd = min(this->pRepsAftMetadata[level][prevb].minEnd, r.end);
                    this->pRepsAftMetadata[level][prevb].maxEnd = max(this->pRepsAftMetadata[level][prevb].maxEnd, r.end);
                    this->pRepsAft_sizes[level][prevb]++;
                }
            }
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
        level++;
    }
}


HINT_M_SubsSort_CM::HINT_M_SubsSort_CM(const Relation &R, const unsigned int numBits, const unsigned int maxBits, const short int typeMetadata) : HierarchicalIndex(R, numBits, maxBits)
{
    this->typeMetadata = typeMetadata;

    // Step 1: one pass to count the contents inside each partition.
    this->pOrgsIn_sizes  = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pOrgsAft_sizes = (RecordId **)malloc(this->height*sizeof(RecordId *));
    this->pRepsIn_sizes  = (size_t **)malloc(this->height*sizeof(size_t *));
    this->pRepsAft_sizes = (size_t **)malloc(this->height*sizeof(size_t *));
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        //calloc allocates memory and sets each counter to 0
        this->pOrgsIn_sizes[l]  = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pOrgsAft_sizes[l] = (RecordId *)calloc(cnt, sizeof(RecordId));
        this->pRepsIn_sizes[l]  = (size_t *)calloc(cnt, sizeof(size_t));
        this->pRepsAft_sizes[l] = (size_t *)calloc(cnt, sizeof(size_t));
    }
    
    for (const Record &r : R)
        this->updateCounters(r);
    
    // Step 2: allocate necessary memory.
    this->pOrgsInTmp  = new Relation*[this->height];
    this->pOrgsAftTmp = new Relation*[this->height];
    this->pRepsInTmp  = new Relation*[this->height];
    this->pRepsAftTmp = new Relation*[this->height];
    
    if (this->typeMetadata != METADATA_NO)
    {
        this->pOrgsInMetadata  = new Metadata*[this->height];
        this->pOrgsAftMetadata = new Metadata*[this->height];
        this->pRepsInMetadata  = new Metadata*[this->height];
        this->pRepsAftMetadata = new Metadata*[this->height];
    }
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInTmp[l]  = new Relation[cnt];
        this->pOrgsAftTmp[l] = new Relation[cnt];
        this->pRepsInTmp[l]  = new Relation[cnt];
        this->pRepsAftTmp[l] = new Relation[cnt];
        
        if (this->typeMetadata != METADATA_NO)
        {
            this->pOrgsInMetadata[l]  = new Metadata[cnt];
            this->pOrgsAftMetadata[l] = new Metadata[cnt];
            this->pRepsInMetadata[l]  = new Metadata[cnt];
            this->pRepsAftMetadata[l] = new Metadata[cnt];
        }
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].resize(this->pOrgsIn_sizes[l][pId]);
            this->pOrgsAftTmp[l][pId].resize(this->pOrgsAft_sizes[l][pId]);
            this->pRepsInTmp[l][pId].resize(this->pRepsIn_sizes[l][pId]);
            this->pRepsAftTmp[l][pId].resize(this->pRepsAft_sizes[l][pId]);
        }
    }
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        memset(this->pOrgsIn_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pOrgsAft_sizes[l], 0, cnt*sizeof(RecordId));
        memset(this->pRepsIn_sizes[l], 0, cnt*sizeof(size_t));
        memset(this->pRepsAft_sizes[l], 0, cnt*sizeof(size_t));
    }
    
    // Step 3: fill partitions.
    if (this->typeMetadata == METADATA_INFORMED_ORIGINAL_DOMAIN)
    {
        for (const Record &r : R)
            this->updatePartitions_withBounds(r);
    }
    else
    {
        for (const Record &r : R)
            this->updatePartitions(r);        
    }
    
    // Step 4: sort partition contents.
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        for (auto pId = 0; pId < cnt; pId++)
        {
            this->pOrgsInTmp[l][pId].sortByStart();
            this->pOrgsAftTmp[l][pId].sortByStart();
            this->pRepsInTmp[l][pId].sortByEnd();
        }
    }
    
    // Free auxiliary memory.
    for (auto l = 0; l < this->height; l++)
    {
        free(this->pOrgsIn_sizes[l]);
        free(this->pOrgsAft_sizes[l]);
        free(this->pRepsIn_sizes[l]);
        free(this->pRepsAft_sizes[l]);
    }
    free(this->pOrgsIn_sizes);
    free(this->pOrgsAft_sizes);
    free(this->pRepsIn_sizes);
    free(this->pRepsAft_sizes);
    
    // Copy and free auxiliary memory.
    this->pOrgsInIds  = new RelationId*[this->height];
    this->pOrgsAftIds = new RelationId*[this->height];
    this->pRepsInIds  = new RelationId*[this->height];
    this->pRepsAftIds = new RelationId*[this->height];
    this->pOrgsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pOrgsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsInTimestamps  = new vector<pair<Timestamp, Timestamp> >*[this->height];
    this->pRepsAftTimestamps = new vector<pair<Timestamp, Timestamp> >*[this->height];
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
        this->pOrgsInIds[l]  = new RelationId[cnt];
        this->pOrgsAftIds[l] = new RelationId[cnt];
        this->pRepsInIds[l]  = new RelationId[cnt];
        this->pRepsAftIds[l] = new RelationId[cnt];
        this->pOrgsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pOrgsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsInTimestamps[l]  = new vector<pair<Timestamp, Timestamp> >[cnt];
        this->pRepsAftTimestamps[l] = new vector<pair<Timestamp, Timestamp> >[cnt];
        
        for (auto pId = 0; pId < cnt; pId++)
        {
            auto cnt = this->pOrgsInTmp[l][pId].size();
            this->pOrgsInIds[l][pId].resize(cnt);
            this->pOrgsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsInIds[l][pId][j] = this->pOrgsInTmp[l][pId][j].id;
                this->pOrgsInTimestamps[l][pId][j].first = this->pOrgsInTmp[l][pId][j].start;
                this->pOrgsInTimestamps[l][pId][j].second = this->pOrgsInTmp[l][pId][j].end;
            }
            
            cnt = this->pOrgsAftTmp[l][pId].size();
            this->pOrgsAftIds[l][pId].resize(cnt);
            this->pOrgsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pOrgsAftIds[l][pId][j] = this->pOrgsAftTmp[l][pId][j].id;
                this->pOrgsAftTimestamps[l][pId][j].first = this->pOrgsAftTmp[l][pId][j].start;
                this->pOrgsAftTimestamps[l][pId][j].second = this->pOrgsAftTmp[l][pId][j].end;
            }
            
            cnt = this->pRepsInTmp[l][pId].size();
            this->pRepsInIds[l][pId].resize(cnt);
            this->pRepsInTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsInIds[l][pId][j] = this->pRepsInTmp[l][pId][j].id;
                this->pRepsInTimestamps[l][pId][j].first = this->pRepsInTmp[l][pId][j].start;
                this->pRepsInTimestamps[l][pId][j].second = this->pRepsInTmp[l][pId][j].end;
            }

            cnt = this->pRepsAftTmp[l][pId].size();
            this->pRepsAftIds[l][pId].resize(cnt);
            this->pRepsAftTimestamps[l][pId].resize(cnt);
            for (auto j = 0; j < cnt; j++)
            {
                this->pRepsAftIds[l][pId][j] = this->pRepsAftTmp[l][pId][j].id;
                this->pRepsAftTimestamps[l][pId][j].first = this->pRepsAftTmp[l][pId][j].start;
                this->pRepsAftTimestamps[l][pId][j].second = this->pRepsAftTmp[l][pId][j].end;
            }
        }
        
        delete[] this->pOrgsInTmp[l];
        delete[] this->pOrgsAftTmp[l];
        delete[] this->pRepsInTmp[l];
        delete[] this->pRepsAftTmp[l];
    }
    delete[] this->pOrgsInTmp;
    delete[] this->pOrgsAftTmp;
    delete[] this->pRepsInTmp;
    delete[] this->pRepsAftTmp;
    
    
//    for (auto l = 0; l < this->numBits; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        for (auto i = 0; i < cnt; i++)
//        {
//            cout << "pOrgsInMetadata[" << l << "][" << i << "]: <" << this->pOrgsInMetadata[l][i].minStart << ", " << this->pOrgsInMetadata[l][i].maxStart << ", " << this->pOrgsInMetadata[l][i].minEnd << ", " << this->pOrgsInMetadata[l][i].maxEnd << ">\t\t";
//
//            cout << "pOrgsAftMetadata[" << l << "][" << i << "]: <";
//            if ((l == 0) && ((i == 0) || !(i%2)))
//                cout << "N/A, N/A, N/A, N/A>\t\t";
//            else
//                 cout << this->pOrgsAftMetadata[l][i].minStart << ", " << this->pOrgsAftMetadata[l][i].maxStart << ", " << this->pOrgsAftMetadata[l][i].minEnd << ", " << this->pOrgsAftMetadata[l][i].maxEnd << ">\t\t";
//
//            cout << "\t\tpRepsInMetadata[" << l << "][" << i << "]: <";
//            if ((l == 0) && ((i == 0) || (i%2)))
//                cout << "N/A, N/A, N/A, N/A>\t\t";
//            else
//                cout << this->pRepsInMetadata[l][i].minStart << ", " << this->pRepsInMetadata[l][i].maxStart << ", " << this->pRepsInMetadata[l][i].minEnd << ", " << this->pRepsInMetadata[l][i].maxEnd << ">\t\t";
//
//            cout << "\t\tpRepsAftMetadata[" << l << "][" << i << "]: <";
//            if (l == 0)
//                cout << "N/A, N/A, N/A, N/A>" << endl;
//            else
//                cout << this->pRepsAftMetadata[l][i].minStart << ", " << this->pRepsAftMetadata[l][i].maxStart << ", " << this->pRepsAftMetadata[l][i].minEnd << ", " << this->pRepsAftMetadata[l][i].maxEnd << ">" << endl;
//        }
//    }

//    for (auto l = 0; l < this->numBits; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        for (auto i = 0; i < cnt; i++)
//        {
//            cout << "pOrgsInMetadata[" << l << "][" << i << "]: ";
//            if (!this->pOrgsInIds[l][i].empty())
//                cout << "<" << this->pOrgsInMetadata[l][i].minStart << ", " << this->pOrgsInMetadata[l][i].maxStart << ", " << this->pOrgsInMetadata[l][i].minEnd << ", " << this->pOrgsInMetadata[l][i].maxEnd << ">\t\t";
//            else
//                cout << "<N/A, N/A, N/A, N/A>\t\t";
//
//            cout << "pOrgsAftMetadata[" << l << "][" << i << "]: ";
//            if (!this->pOrgsAftIds[l][i].empty())
//                 cout << "<" << this->pOrgsAftMetadata[l][i].minStart << ", " << this->pOrgsAftMetadata[l][i].maxStart << ", " << this->pOrgsAftMetadata[l][i].minEnd << ", " << this->pOrgsAftMetadata[l][i].maxEnd << ">\t\t";
//            else
//                cout << "<N/A, N/A, N/A, N/A>\t\t";
//
//            cout << "\t\tpRepsInMetadata[" << l << "][" << i << "]: ";
//            if (!this->pRepsInIds[l][i].empty())
//                cout << "<" << this->pRepsInMetadata[l][i].minStart << ", " << this->pRepsInMetadata[l][i].maxStart << ", " << this->pRepsInMetadata[l][i].minEnd << ", " << this->pRepsInMetadata[l][i].maxEnd << ">\t\t";
//            else
//                cout << "<N/A, N/A, N/A, N/A>\t\t";
//
//            cout << "\t\tpRepsAftMetadata[" << l << "][" << i << "]: ";
//            if (!this->pRepsAftIds[l][i].empty())
//                cout << "<" << this->pRepsAftMetadata[l][i].minStart << ", " << this->pRepsAftMetadata[l][i].maxStart << ", " << this->pRepsAftMetadata[l][i].minEnd << ", " << this->pRepsAftMetadata[l][i].maxEnd << ">" << endl;
//            else
//                cout << "<N/A, N/A, N/A, N/A>" << endl;
//        }
//    }
}


void HINT_M_SubsSort_CM::computeUninformedBoundsMetadata()
{
    auto cnt = (int)(pow(2, this->numBits));
    auto gend = cnt-1;
    
//    this->pOrgsInMetadata  = new Metadata*[this->height];
//    this->pOrgsAftMetadata = new Metadata*[this->height];
//    this->pRepsInMetadata  = new Metadata*[this->height];
//    this->pRepsAftMetadata = new Metadata*[this->height];
//
//    // Leaf level
//    this->pOrgsInMetadata[0]  = new Metadata[cnt];
//    this->pOrgsAftMetadata[0] = new Metadata[cnt];
//    this->pRepsInMetadata[0]  = new Metadata[cnt];
//    this->pRepsAftMetadata[0] = new Metadata[cnt];      // TODO, not needed, pRepsAft[0] always empty
    for (auto i = 0; i < cnt; i++)
    {
        // Partition i is odd: pRepsIn, pRepsAft are both empty by construction
        if (i%2)
        {
            this->pOrgsInMetadata[0][i].minStart = i;
            this->pOrgsInMetadata[0][i].maxStart = i+1;
            this->pOrgsInMetadata[0][i].minEnd   = i;
            this->pOrgsInMetadata[0][i].maxEnd   = i+1;
            
            this->pOrgsAftMetadata[0][i].minStart = i;
            this->pOrgsAftMetadata[0][i].maxStart = i+1;
            this->pOrgsAftMetadata[0][i].minEnd   = i+1;
            this->pOrgsAftMetadata[0][i].maxEnd   = gend;
        }
        else // Partition i is even: pOrgsAft, pRepsAft are both empty by construction
        {
            this->pOrgsInMetadata[0][i].minStart = i;
            this->pOrgsInMetadata[0][i].maxStart = i+1;
            this->pOrgsInMetadata[0][i].minEnd = i;
            this->pOrgsInMetadata[0][i].maxEnd = i+1;
            
            this->pRepsInMetadata[0][i].minStart = 0;
            this->pRepsInMetadata[0][i].maxStart = i;
            this->pRepsInMetadata[0][i].minEnd = i;
            this->pRepsInMetadata[0][i].maxEnd = i+1;
        }
    }
    this->pOrgsInMetadata[0][cnt-1].maxStart = gend;
    this->pOrgsInMetadata[0][cnt-1].maxEnd = gend;
    this->pOrgsAftMetadata[0][cnt-1].maxStart = gend;
    this->pOrgsAftMetadata[0][cnt-1].minEnd = gend;
    this->pRepsInMetadata[0][cnt-1].maxEnd = gend;
    
    // Internal levels
    for (auto l = 1; l < this->numBits; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
//        this->pOrgsInMetadata[l]  = new Metadata[cnt];
//        this->pOrgsAftMetadata[l] = new Metadata[cnt];
//        this->pRepsInMetadata[l]  = new Metadata[cnt];
//        this->pRepsAftMetadata[l]  = new Metadata[cnt];
        
        for (auto i = 0; i < cnt; i++)
        {
            // Partition i is odd
            if (i%2)
            {
                this->pOrgsInMetadata[l][i].minStart = i * pow(2, l);
                this->pOrgsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart+1;      // i * pow(2, l) + 1
                this->pOrgsInMetadata[l][i].maxEnd   = (i+1) * pow(2, l);
                this->pOrgsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].maxEnd-1;        // = (i+1) * pow(2, l) - 1;
                
                this->pOrgsAftMetadata[l][i].minStart = this->pOrgsInMetadata[l][i].minStart;       // = i * pow(2, l)
                this->pOrgsAftMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].maxStart;       // = i * pow(2, l) + 1
                this->pOrgsAftMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].maxEnd;         // = (i+1) * pow(2, l)
                this->pOrgsAftMetadata[l][i].maxEnd   = gend;
                
                this->pRepsInMetadata[l][i].minStart = (i-1)*pow(2, l) + 1;
                this->pRepsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart;        // = i * pow(2, l) + 1
                this->pRepsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].minEnd;          // = (i+1) * pow(2, l) - 1
                this->pRepsInMetadata[l][i].maxEnd   = this->pOrgsInMetadata[l][i].maxEnd;          // = (i+1) * pow(2, l)
                
                this->pRepsAftMetadata[l][i].minStart = this->pRepsInMetadata[l][i].minStart;
                this->pRepsAftMetadata[l][i].maxStart = this->pRepsInMetadata[l][i].maxStart;
                this->pRepsAftMetadata[l][i].minEnd   = this->pOrgsAftMetadata[l][i].minEnd;
                this->pRepsAftMetadata[l][i].maxEnd   = this->pOrgsAftMetadata[l][i].maxEnd;
            }
            else // Partition i is even
            {
                this->pOrgsInMetadata[l][i].minStart = i * pow(2, l);
                this->pOrgsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart+1;      // i * pow(2, l) + 1
                this->pOrgsInMetadata[l][i].maxEnd   = (i+1) * pow(2, l);
                this->pOrgsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].maxEnd-1;        // = (i+1) * pow(2, l) - 1;
                
                this->pOrgsAftMetadata[l][i].minStart = this->pOrgsInMetadata[l][i].minStart;       // = i * pow(2, l)
                this->pOrgsAftMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].maxStart;       // = i * pow(2, l) + 1
                this->pOrgsAftMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].maxEnd;         // = (i+1) * pow(2, l)
//                this->pOrgsAftMetadata[l][i].maxEnd   = gend;
                this->pOrgsAftMetadata[l][i].maxEnd   = this->pOrgsInMetadata[l][i].maxEnd + pow(2,l);
                
                this->pRepsInMetadata[l][i].minStart = 0;
                this->pRepsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart;        // = i * pow(2, l) + 1
                this->pRepsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].minEnd;          // = (i+1) * pow(2, l) - 1
                this->pRepsInMetadata[l][i].maxEnd   = this->pOrgsInMetadata[l][i].maxEnd;          // = (i+1) * pow(2, l)
                
                this->pRepsAftMetadata[l][i].minStart = this->pRepsInMetadata[l][i].minStart;
                this->pRepsAftMetadata[l][i].maxStart = this->pRepsInMetadata[l][i].maxStart;
                this->pRepsAftMetadata[l][i].minEnd   = this->pOrgsAftMetadata[l][i].minEnd;
                this->pRepsAftMetadata[l][i].maxEnd   = this->pOrgsAftMetadata[l][i].maxEnd;
            }
        }
    }
    
//    for (auto l = 0; l < this->numBits; l++)
//    {
//        auto cnt = (int)(pow(2, this->numBits-l));
//        for (auto i = 0; i < cnt; i++)
//        {
//            cout << "pOrgsInMetadata[" << l << "][" << i << "]: <" << this->pOrgsInMetadata[l][i].minStart << ", " << this->pOrgsInMetadata[l][i].maxStart << ", " << this->pOrgsInMetadata[l][i].minEnd << ", " << this->pOrgsInMetadata[l][i].maxEnd << ">\t\t";
//            
//            cout << "pOrgsAftMetadata[" << l << "][" << i << "]: <";
//            if ((l == 0) && ((i == 0) || !(i%2)))
//                cout << "N/A, N/A, N/A, N/A>\t\t";
//            else
//                 cout << this->pOrgsAftMetadata[l][i].minStart << ", " << this->pOrgsAftMetadata[l][i].maxStart << ", " << this->pOrgsAftMetadata[l][i].minEnd << ", " << this->pOrgsAftMetadata[l][i].maxEnd << ">\t\t";
//
//            cout << "\t\tpRepsInMetadata[" << l << "][" << i << "]: <";
//            if ((l == 0) && ((i == 0) || (i%2)))
//                cout << "N/A, N/A, N/A, N/A>\t\t";
//            else
//                cout << this->pRepsInMetadata[l][i].minStart << ", " << this->pRepsInMetadata[l][i].maxStart << ", " << this->pRepsInMetadata[l][i].minEnd << ", " << this->pRepsInMetadata[l][i].maxEnd << ">\t\t";
//            
//            cout << "\t\tpRepsAftMetadata[" << l << "][" << i << "]: <";
//            if (l == 0)
//                cout << "N/A, N/A, N/A, N/A>" << endl;
//            else
//                cout << this->pRepsAftMetadata[l][i].minStart << ", " << this->pRepsAftMetadata[l][i].maxStart << ", " << this->pRepsAftMetadata[l][i].minEnd << ", " << this->pRepsAftMetadata[l][i].maxEnd << ">" << endl;
//        }
//    }
}


void HINT_M_SubsSort_CM::computeUninformedBoundsMetadataOriginalSpace()
{
    auto cnt = (int)(pow(2, this->numBits));
    auto pext = 1 << (this->maxBits-this->numBits);
    auto gend = cnt*pext;
    
    
//    this->pOrgsInMetadata  = new Metadata*[this->height];
//    this->pOrgsAftMetadata = new Metadata*[this->height];
//    this->pRepsInMetadata  = new Metadata*[this->height];
//    this->pRepsAftMetadata = new Metadata*[this->height];
//    
//    // Leaf level
//    this->pOrgsInMetadata[0]  = new Metadata[cnt];
//    this->pOrgsAftMetadata[0] = new Metadata[cnt];
//    this->pRepsInMetadata[0]  = new Metadata[cnt];
//    this->pRepsAftMetadata[0] = new Metadata[cnt];      // TODO, not needed, pRepsAft[0] always empty
    for (auto i = 0; i < cnt; i++)
    {
        // Partition i is odd: pRepsIn, pRepsAft are both empty by construction
        if (i%2)
        {
            this->pOrgsInMetadata[0][i].minStart = i*pext;
            this->pOrgsInMetadata[0][i].maxStart = (i+1)*pext-1;
            this->pOrgsInMetadata[0][i].minEnd   = i*pext;
            this->pOrgsInMetadata[0][i].maxEnd   = (i+1)*pext-1;
            
            this->pOrgsAftMetadata[0][i].minStart = i*pext;
            this->pOrgsAftMetadata[0][i].maxStart = (i+1)*pext-1;
            this->pOrgsAftMetadata[0][i].minEnd   = (i+1)*pext;
            this->pOrgsAftMetadata[0][i].maxEnd   = gend;
        }
        else // Partition i is even: pOrgsAft, pRepsAft are both empty by construction
        {
            this->pOrgsInMetadata[0][i].minStart = i*pext;
            this->pOrgsInMetadata[0][i].maxStart = (i+1)*pext-1;
            this->pOrgsInMetadata[0][i].minEnd = i*pext;
            this->pOrgsInMetadata[0][i].maxEnd = (i+1)*pext-1;
            
            this->pRepsInMetadata[0][i].minStart = 0;
            this->pRepsInMetadata[0][i].maxStart = i*pext-1;
            this->pRepsInMetadata[0][i].minEnd = i*pext;
            this->pRepsInMetadata[0][i].maxEnd = (i+1)*pext-1;
        }
    }
    this->pOrgsInMetadata[0][cnt-1].maxStart = gend;
    this->pOrgsInMetadata[0][cnt-1].maxEnd = gend;
    this->pOrgsAftMetadata[0][cnt-1].maxStart = gend;
    this->pOrgsAftMetadata[0][cnt-1].minEnd = gend;
    this->pRepsInMetadata[0][cnt-1].maxEnd = gend;
    
    // Internal levels
    for (auto l = 1; l < this->numBits; l++)
    {
        auto cnt = (int)(pow(2, this->numBits-l));
        
//        this->pOrgsInMetadata[l]  = new Metadata[cnt];
//        this->pOrgsAftMetadata[l] = new Metadata[cnt];
//        this->pRepsInMetadata[l]  = new Metadata[cnt];
//        this->pRepsAftMetadata[l]  = new Metadata[cnt];
        
        for (auto i = 0; i < cnt; i++)
        {
            // Partition i is odd
            if (i%2)
            {
                this->pOrgsInMetadata[l][i].minStart = i*pow(2, l) * pext;
                this->pOrgsInMetadata[l][i].maxStart = (i*pow(2,l) + 1) * pext - 1;
                this->pOrgsInMetadata[l][i].maxEnd   = (i+1)*pow(2, l) * pext - 1;
                this->pOrgsInMetadata[l][i].minEnd   = ((i+1)*pow(2, l)-1) * pext;
                
                this->pOrgsAftMetadata[l][i].minStart = this->pOrgsInMetadata[l][i].minStart;
                this->pOrgsAftMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].maxStart;
                this->pOrgsAftMetadata[l][i].minEnd   = (i+1)*pow(2, l) * pext;
                this->pOrgsAftMetadata[l][i].maxEnd   = gend;
                
                this->pRepsInMetadata[l][i].minStart = ((i-1)*pow(2, l) + 1)*pext;
                this->pRepsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart-1;
                this->pRepsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].minEnd;
                this->pRepsInMetadata[l][i].maxEnd   = this->pOrgsInMetadata[l][i].maxEnd;
                
                this->pRepsAftMetadata[l][i].minStart = this->pRepsInMetadata[l][i].minStart;
                this->pRepsAftMetadata[l][i].maxStart = this->pRepsInMetadata[l][i].maxStart;
                this->pRepsAftMetadata[l][i].minEnd   = this->pOrgsAftMetadata[l][i].minEnd;
                this->pRepsAftMetadata[l][i].maxEnd   = this->pOrgsAftMetadata[l][i].maxEnd;
            }
            else // Partition i is even
            {
                this->pOrgsInMetadata[l][i].minStart = i * pow(2, l) * pext;
                this->pOrgsInMetadata[l][i].maxStart = (i * pow(2, l)+1) * pext - 1;
                this->pOrgsInMetadata[l][i].maxEnd   = (i+1) * pow(2, l) * pext - 1 ;
                this->pOrgsInMetadata[l][i].minEnd   = ((i+1) * pow(2, l) - 1) * pext;
                
                this->pOrgsAftMetadata[l][i].minStart = this->pOrgsInMetadata[l][i].minStart;
                this->pOrgsAftMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].maxStart;
                this->pOrgsAftMetadata[l][i].minEnd   = (i+1) * pow(2, l) * pext;
//                this->pOrgsAftMetadata[l][i].maxEnd   = gend-1;
                this->pOrgsAftMetadata[l][i].maxEnd = ((i+2)*pow(2,l) - 1)*pext;
                
                this->pRepsInMetadata[l][i].minStart = 0;
                this->pRepsInMetadata[l][i].maxStart = this->pOrgsInMetadata[l][i].minStart-1;
                this->pRepsInMetadata[l][i].minEnd   = this->pOrgsInMetadata[l][i].minEnd;
                this->pRepsInMetadata[l][i].maxEnd   = this->pOrgsInMetadata[l][i].maxEnd;
                
                this->pRepsAftMetadata[l][i].minStart = this->pRepsInMetadata[l][i].minStart;
                this->pRepsAftMetadata[l][i].maxStart = this->pRepsInMetadata[l][i].maxStart;
                this->pRepsAftMetadata[l][i].minEnd   = this->pOrgsAftMetadata[l][i].minEnd;
                this->pRepsAftMetadata[l][i].maxEnd   = this->pOrgsAftMetadata[l][i].maxEnd;
            }
        }
    }
}


void HINT_M_SubsSort_CM::getStats()
{
    size_t sum = 0;
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        this->numPartitions += cnt;
        for (int pid = 0; pid < cnt; pid++)
        {
            this->numOriginalsIn  += this->pOrgsInIds[l][pid].size();
            this->numOriginalsAft += this->pOrgsAftIds[l][pid].size();
            this->numReplicasIn   += this->pRepsInIds[l][pid].size();
            this->numReplicasAft  += this->pRepsAftIds[l][pid].size();
            if ((this->pOrgsInIds[l][pid].empty()) && (this->pOrgsAftIds[l][pid].empty()) && (this->pRepsInIds[l][pid].empty()) && (this->pRepsAftIds[l][pid].empty()))
                this->numEmptyPartitions++;
        }
    }
    
    this->avgPartitionSize = (float)(this->numIndexedRecords+this->numReplicasIn+this->numReplicasAft)/(this->numPartitions-numEmptyPartitions);
}


size_t HINT_M_SubsSort_CM::getSize()
{
    size_t size = 0;
    
    for (auto l = 0; l < this->height; l++)
    {
        auto cnt = pow(2, this->numBits-l);
        
        for (int pid = 0; pid < cnt; pid++)
        {
            size += this->pOrgsInIds[l][pid].size()  * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pOrgsAftIds[l][pid].size() * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pRepsInIds[l][pid].size()  * (sizeof(RecordId) + 2*sizeof(Timestamp));
            size += this->pRepsAftIds[l][pid].size() * (sizeof(RecordId) + 2*sizeof(Timestamp));
            
            if (this->typeMetadata == METADATA_INFORMED_ORIGINAL_DOMAIN)
            {
                if (!this->pOrgsInIds[l][pid].empty())
                    size += 4 * sizeof(Timestamp);
                if (!this->pOrgsAftIds[l][pid].empty())
                    size += 4 * sizeof(Timestamp);
                if (!this->pRepsInIds[l][pid].empty())
                    size += 4 * sizeof(Timestamp);
                if (!this->pRepsAftIds[l][pid].empty())
                    size += 4 * sizeof(Timestamp);
            }
        }
    }
    
    return size;
}



HINT_M_SubsSort_CM::~HINT_M_SubsSort_CM()
{
    for (auto l = 0; l < this->height; l++)
    {
        delete[] this->pOrgsInIds[l];
        delete[] this->pOrgsInTimestamps[l];
        delete[] this->pOrgsAftIds[l];
        delete[] this->pOrgsAftTimestamps[l];
        delete[] this->pRepsInIds[l];
        delete[] this->pRepsInTimestamps[l];
        delete[] this->pRepsAftIds[l];
        
        if (this->typeMetadata != METADATA_NO)
        {
            delete[] this->pOrgsInMetadata[l];
            delete[] this->pOrgsAftMetadata[l];
            delete[] this->pRepsInMetadata[l];
            delete[] this->pRepsAftMetadata[l];
        }
    }
    
    delete[] this->pOrgsInIds;
    delete[] this->pOrgsInTimestamps;
    delete[] this->pOrgsAftIds;
    delete[] this->pOrgsAftTimestamps;
    delete[] this->pRepsInIds;
    delete[] this->pRepsInTimestamps;
    delete[] this->pRepsAftIds;
    
    if (this->typeMetadata != METADATA_NO)
    {
        delete[] this->pOrgsInMetadata;
        delete[] this->pOrgsAftMetadata;
        delete[] this->pRepsInMetadata;
        delete[] this->pRepsAftMetadata;
    }
}


// Querying
inline void HINT_M_SubsSort_CM::scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();
    auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= (*iterI);
#endif

        iter++;
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= (*iterI);
#endif

        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_NoChecks_gOverlaps(RelationId &ids, size_t &result)
{
    auto iterIBegin = ids.begin();
    auto iterIEnd = ids.end();

    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
    {
#ifdef WORKLOAD_COUNT
        result++;
#else
        result ^= (*iterI);
#endif
    }
}


//inline void HINT_M_SubsSort_CM::scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, size_t &result)
//{
//    for (auto j = ts; j <= te; j++)
//    {
//        auto iterIBegin = ids[level][j].begin();
//        auto iterIEnd = ids[level][j].end();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//#ifdef WORKLOAD_COUNT
//            result++;
//#else
//            result ^= (*iterI);
//#endif
//        }
//    }
//}


inline void HINT_M_SubsSort_CM::scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((q.start <= iter->second) && (computeRelevance(*iter, q) > theta))
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if ((q.start <= iter->second) && (computeRelevance(*iter, q) > theta))
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();
    auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iter++;
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_NoChecks_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
{
    auto iterIBegin = ids.begin();
    auto iterIEnd = ids.end();
    auto iter = timestamps.begin();

    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
    {
        if (computeRelevance(*iter, q) > theta)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
        iter++;
    }
}


//inline void HINT_M_SubsSort_CM::scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), size_t &result)
//{
//    for (auto j = ts; j <= te; j++)
//    {
//        auto iterIBegin = ids[level][j].begin();
//        auto iterIEnd = ids[level][j].end();
//        auto iter = timestamps[level][j].begin();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//            if (computeRelevance(*iter, q) > theta)
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iter++;
//        }
//    }
//}


inline void HINT_M_SubsSort_CM::scanPartition_CheckBothTimestamps_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
    
    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            updateTopK(result, k, computeRelevance(*iter, q));

        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        if (q.start <= iter->second)
            updateTopK(result, k, computeRelevance(*iter, q));
        
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckEnd_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = timestamps.end();
    auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), compare);
    
    iterI += iter-iterBegin;
    while (iter != iterEnd)
    {
        updateTopK(result, k, computeRelevance(*iter, q));

        iter++;
        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_CheckStart_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, bool (*compare)(const pair<Timestamp, Timestamp>&, const pair<Timestamp, Timestamp>&), const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    auto iterI = ids.begin();
    auto iterBegin = timestamps.begin();
    auto iterEnd = lower_bound(iterBegin, timestamps.end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));

    for (auto iter = iterBegin; iter != iterEnd; iter++)
    {
        updateTopK(result, k, computeRelevance(*iter, q));

        iterI++;
    }
}


inline void HINT_M_SubsSort_CM::scanPartition_NoChecks_gOverlaps(RelationId &ids, vector<pair<Timestamp, Timestamp>> &timestamps, const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    auto iterIBegin = ids.begin();
    auto iterIEnd = ids.end();
    auto iter = timestamps.begin();

    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
    {
        updateTopK(result, k, computeRelevance(*iter, q));

        iter++;
    }
}


//inline void HINT_M_SubsSort_CM::scanPartitions_NoChecks_gOverlaps(unsigned int level, Timestamp ts, Timestamp te, RelationId **ids, vector<pair<Timestamp, Timestamp>> **timestamps, RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
//{
//    for (auto j = ts; j <= te; j++)
//    {
//        auto iterIBegin = ids[level][j].begin();
//        auto iterIEnd = ids[level][j].end();
//        auto iter = timestamps[level][j].begin();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//            updateTopK(result, k, computeRelevance(*iter, q));
//
//            iter++;
//        }
//    }
//}


//pair<Timestamp, Timestamp> HINT_M_SubsSort_CM::getBestInterval(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
//{
////    cout<<endl;
////    cout << "\tminStart: " << md[level][t].minStart << endl;
////    cout << "\tmaxStart: " << md[level][t].maxStart << endl;
////    cout << "\tminEnd  : " << md[level][t].minEnd << endl;
////    cout << "\tmaxEnd  : " << md[level][t].maxEnd << endl;
//    auto bstart = min(max(qa, md[level][t].minStart), md[level][t].maxStart);
//    auto bend   = max(min(qb, md[level][t].maxEnd), md[level][t].minEnd);
//
//    return make_pair(bstart, bend);
//}



//Timestamp HINT_M_SubsSort_CM::computeUpperBoundAbsoluteRelevance(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
//{
//    pair<Timestamp, Timestamp> p = this->getBestInterval(level, t, md, qa, qb);
//    
//    return min(qb, p.second)-max(qa, p.first)+1;
//}




size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps(const RangeQuery &q)
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
            
            // Handle rest: consider only originals
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsInIds, result);
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsAftIds, result);
            for (auto i = a; i <= b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, result);
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, result);
            }

            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, result);
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, result);
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsInIds, result);
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, result);
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, result);
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
#ifdef WORKLOAD_COUNT
            result++;
#else
            result ^= (*iterI);
#endif
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second))
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }
    
    
    return result;
}


// Theta relevance: bottom-up search using no bounds
inline size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceThreshold(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>& r, const RangeQuery& q))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, theta, computeRelevance, result);
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            
            // Handle rest: consider only originals
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
            for (auto i = a; i <= b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
            }
            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }
    
    
    return result;
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold(q, theta, computeAbsoluteRelevance);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold(q, theta, computeRelativeRelevance);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold(q, theta, computeRecordRelativeRelevance);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold(q, theta, computeQueryRelativeRelevance);
}


// Theta relevance: bottom-up search using lower bounds
inline size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta));
    bool foundzero = false;
    bool foundone = false;


    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th))
                freeResults[2] += this->pRepsInIds[l][a].size();
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th))
                freeResults[3] += this->pRepsAftIds[l][a].size();
#endif

            // Handle rest: consider only originals
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
            else
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
                
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th))
                        freeResults[0] += this->pOrgsInIds[l][i].size();
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                        freeResults[1] += this->pOrgsAftIds[l][i].size();
#endif
                }

                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], q, theta, computeRelevance, result);
                }
#ifdef STATS
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th))
                    freeResults[0] += this->pOrgsInIds[l][b].size();
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                    freeResults[1] += this->pOrgsAftIds[l][b].size();
#endif
            }
#ifdef STATS
            if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                freeResults[0] += this->pOrgsInIds[l][a].size();
            if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                freeResults[1] += this->pOrgsAftIds[l][a].size();
#endif
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }
            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
#ifdef STATS
            if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                freeResults[0] += this->pOrgsInIds[l][a].size();
            if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                freeResults[1] += this->pOrgsAftIds[l][a].size();
#endif

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th))
                freeResults[2] += this->pRepsInIds[l][a].size();
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th))
                freeResults[3] += this->pRepsAftIds[l][a].size();
#endif

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    auto tmp = result;
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th))
                        freeResults[0] += this->pOrgsInIds[l][i].size();
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                        freeResults[1] += this->pOrgsAftIds[l][i].size();
#endif
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
#ifdef STATS
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th))
                    freeResults[0] += this->pOrgsInIds[l][b].size();
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                    freeResults[1] += this->pOrgsAftIds[l][b].size();
#endif
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceLowerBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingLowerBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound);
}


// Theta relevance: bottom-up search using upper bounds
inline size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(Metadata &, const RangeQuery&))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta));
    bool foundzero = false;
    bool foundone = false;

    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsInMetadata[l][a], q) > th))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, theta, computeRelevance, result);
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsAftMetadata[l][a], q) > th))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
#ifdef STATS
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsInMetadata[l][a], q) <= th))
                prunedIntervals[2] += this->pRepsInIds[l][a].size();
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsAftMetadata[l][a], q) <= th))
                prunedIntervals[3] += this->pRepsAftIds[l][a].size();
#endif

            // Handle rest: consider only originals
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
            else
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);

                for (auto i = a+1; i < b; i++)
                {
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][i], q) > th))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][i], q) > th))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
#ifdef STATS
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][i], q) <= th))
                        prunedIntervals[0] += this->pOrgsInIds[l][i].size();
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][i], q) <= th))
                        prunedIntervals[1] += this->pOrgsAftIds[l][i].size();
#endif
                }
                
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][b], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][b], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], q, theta, computeRelevance, result);
#ifdef STATS
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][b], q) <= th))
                    prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][b], q) <= th))
                    prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
#endif
            }
#ifdef STATS
            if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) <= th))
                prunedIntervals[0] += this->pOrgsInIds[l][a].size();
            if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) <= th))
                prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
#endif
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) > th))
                    this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) > th))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
            }

            else
            {
                // Lemma 1
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) > th))
                    this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) > th))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][a], q) <= th))
                prunedIntervals[0] += this->pOrgsInIds[l][a].size();
            if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][a], q) <= th))
                prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
#endif

            // Lemma 1, 3
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsInMetadata[l][a], q) > th))
                this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsAftMetadata[l][a], q) > th))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
#ifdef STATS
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsInMetadata[l][a], q) <= th))
                prunedIntervals[2] += this->pRepsInIds[l][a].size();
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(this->pRepsAftMetadata[l][a], q) <= th))
                prunedIntervals[3] += this->pRepsAftIds[l][a].size();
#endif

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i <= b-1; i++)
                {
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][i], q) > th))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][i], q) > th))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
#ifdef STATS
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][i], q) <= th))
                        prunedIntervals[0] += this->pOrgsInIds[l][a].size();
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][i], q) <= th))
                        prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
#endif
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][b], q) > th))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][b], q) > th))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
#ifdef STATS
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsInMetadata[l][b], q) <= th))
                    prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(this->pOrgsAftMetadata[l][b], q) <= th))
                    prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
#endif
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceUpperBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingUpperBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound);
}


// Theta relevance: bottom-up search using both lower and upper bounds
inline size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
#ifdef LU
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta));
    bool foundzero = false;
    bool foundone = false;


    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    freeResults[2] += this->pRepsInIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) <= th)
                    prunedIntervals[2] += this->pRepsInIds[l][a].size();
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    freeResults[3] += this->pRepsAftIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) <= th)
                    prunedIntervals[3] += this->pRepsAftIds[l][a].size();
            }
#endif

            // Handle rest: consider only originals
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
            else
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }

                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            freeResults[0] += this->pOrgsInIds[l][i].size();
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) <= th)
                            prunedIntervals[0] += this->pOrgsInIds[l][i].size();
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            freeResults[1] += this->pOrgsAftIds[l][i].size();
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) <= th)
                            prunedIntervals[1] += this->pOrgsAftIds[l][i].size();
                    }
#endif
                }
                
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], q, theta, computeRelevance, result);
                }
#ifdef STATS
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][b].size();
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][b].size();
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
                }
#endif
            }
#ifdef STATS
            if (!this->pOrgsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                    freeResults[0] += this->pOrgsInIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) <= th)
                    prunedIntervals[0] += this->pOrgsInIds[l][a].size();
            }
            if (!this->pOrgsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                    freeResults[1] += this->pOrgsAftIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) <= th)
                    prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
            }
#endif
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }
            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
#ifdef STATS
            if (!this->pOrgsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                    freeResults[0] += this->pOrgsInIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) <= th)
                    prunedIntervals[0] += this->pOrgsInIds[l][a].size();
            }
            if (!this->pOrgsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                    freeResults[1] += this->pOrgsAftIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) <= th)
                    prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
            }
#endif

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
            }
#ifdef STATS
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    freeResults[2] += this->pRepsInIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) <= th)
                    prunedIntervals[2] += this->pRepsInIds[l][a].size();
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    freeResults[3] += this->pRepsAftIds[l][a].size();
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) <= th)
                    prunedIntervals[3] += this->pRepsAftIds[l][a].size();
            }
#endif

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i <= b-1; i++)
                {
                    auto tmp = result;
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            freeResults[0] += this->pOrgsInIds[l][i].size();
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) <= th)
                            prunedIntervals[0] += this->pOrgsInIds[l][i].size();
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            freeResults[1] += this->pOrgsAftIds[l][i].size();
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) <= th)
                            prunedIntervals[1] += this->pOrgsAftIds[l][i].size();
                    }
#endif
                }
                
                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
#ifdef STATS
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][b].size();
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][b].size();
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
                }
#endif
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}
#elif defined(UL)
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta));
    bool foundzero = false;
    bool foundone = false;


    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th))
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, theta, computeRelevance, result);
            }
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th))
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) <= th)
                    prunedIntervals[2] += this->pRepsInIds[l][a].size();
                else if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    freeResults[2] += this->pRepsInIds[l][a].size();
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) <= th)
                    prunedIntervals[3] += this->pRepsAftIds[l][a].size();
                else if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    freeResults[3] += this->pRepsAftIds[l][a].size();
            }
#endif

            // Handle rest: consider only originals
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
            else
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
                for (auto i = a+1; i < b; i++)
                {
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th))
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) <= th)
                            prunedIntervals[0] += this->pOrgsInIds[l][i].size();
                        else if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            freeResults[0] += this->pOrgsInIds[l][i].size();
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) <= th)
                            prunedIntervals[1] += this->pOrgsAftIds[l][i].size();
                        else if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            freeResults[1] += this->pOrgsAftIds[l][i].size();
                    }
#endif
                }
                
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], q, theta, computeRelevance, result);
                }
#ifdef STATS
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                    else if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][b].size();
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
                    else if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][b].size();
                }
#endif

            }
#ifdef STATS
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][a].size();
                    else if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][a].size();
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
                    else if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][a].size();
                }
#endif
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }
            else
            {
                // Lemma 1
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], result);
                    else
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                    else
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, theta, computeRelevance, result);
                }
            }
#ifdef STATS
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][a].size();
                    else if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][a].size();
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][a].size();
                    else if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][a].size();
                }
#endif

            // Lemma 1, 3
            if ((!this->pRepsInIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th))
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
                else
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            }
            if ((!this->pRepsAftIds[l][a].empty()) && (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th))
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, theta, computeRelevance, result);
            }
#ifdef STATS
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) <= th)
                    prunedIntervals[2] += this->pRepsInIds[l][a].size();
                else if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    freeResults[2] += this->pRepsInIds[l][a].size();
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) <= th)
                    prunedIntervals[3] += this->pRepsAftIds[l][a].size();
                else if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    freeResults[3] += this->pRepsAftIds[l][a].size();
            }
#endif

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i <= b-1; i++)
                {
                    auto tmp = result;
                    if ((!this->pOrgsInIds[l][i].empty()) && (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th))
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, theta, computeRelevance, result);
                    }
                    if ((!this->pOrgsAftIds[l][i].empty()) && (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                        else
                            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, theta, computeRelevance, result);
                    }
#ifdef STATS
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) <= th)
                            prunedIntervals[0] += this->pOrgsInIds[l][i].size();
                        else if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            freeResults[0] += this->pOrgsInIds[l][i].size();
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) <= th)
                            prunedIntervals[1] += this->pOrgsAftIds[l][i].size();
                        else if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            freeResults[1] += this->pOrgsAftIds[l][i].size();
                    }
#endif
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], result);
                    else
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
#ifdef STATS
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) <= th)
                        prunedIntervals[0] += this->pOrgsInIds[l][b].size();
                    else if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        freeResults[0] += this->pOrgsInIds[l][b].size();
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) <= th)
                        prunedIntervals[1] += this->pOrgsAftIds[l][b].size();
                    else if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        freeResults[1] += this->pOrgsAftIds[l][b].size();
                }
#endif
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}
#endif
/*size_t HINT_M_SubsSort_CM::executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta));
    bool foundzero = false;
    bool foundone = false;


    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
            }

            // Handle rest: consider only originals
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }
            else
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                    }
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                    }
                }
                
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckEnd_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_CheckEnd_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
            }

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i <= b-1; i++)
                {
                    auto tmp = result;
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                    }
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }
//    cout << tmp << endl;

    return result;
}


size_t HINT_M_SubsSort_CM::executeBottomUp_RelativeRecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta,
                                                                                                    float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&),
                                                                                                    float (*computeRelevanceLowerBound_MinStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb),
                                                                                                    float (*computeRelevanceLowerBound_MinStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb),
                                                                                                    float (*computeRelevanceLowerBound_MaxStartMinEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb),
                                                                                                    float (*computeRelevanceLowerBound_MaxStartMaxEnd)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb),
                                                                                                    float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta) >> (this->maxBits-this->numBits));      // When the absolute relevance function is used, we need to convert it first to an int
    bool foundzero = false;
    bool foundone = false;

    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
            {
                if (//(this->pRepsInIds[l][a].size() > SIZE_THRESHOLD) &&
                    (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pRepsInMetadata, qa, qb) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (//(this->pRepsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                    (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
            }

            // Handle rest: consider only originals
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (//(this->pOrgsInIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (//(this->pOrgsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }
            else
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (//(this->pOrgsInIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (//(this->pOrgsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }

                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (//(this->pOrgsInIds[l][i].size() > SIZE_THRESHOLD) &&
                            (computeRelevanceLowerBound_MinStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MinStartMaxEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsInMetadata, qa, qb) > th))
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (//(this->pOrgsAftIds[l][i].size() > SIZE_THRESHOLD) &&
                            (computeRelevanceLowerBound_MinStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MinStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                    }

                }
                
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (//(this->pOrgsInIds[l][b].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (//(this->pOrgsAftIds[l][b].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (//(this->pOrgsInIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (//(this->pOrgsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (//(this->pOrgsInIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckEnd_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (//(this->pOrgsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                }
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
            {
                if (//(this->pRepsInIds[l][a].size() > SIZE_THRESHOLD) &&
                    (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pRepsInMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pRepsInMetadata, qa, qb) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_CheckEnd_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, theta, computeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if (//(this->pRepsAftIds[l][a].size() > SIZE_THRESHOLD) &&
                    (computeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) &&
                    (computeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
            }

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (//(this->pOrgsInIds[l][i].size() > SIZE_THRESHOLD) &&
                            (computeRelevanceLowerBound_MinStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MinStartMaxEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsInMetadata, qa, qb) > th))
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (//(this->pOrgsAftIds[l][i].size() > SIZE_THRESHOLD) &&
                            (computeRelevanceLowerBound_MinStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MinStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th) &&
                            (computeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb) > th))
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRelevance, result);
                    }
                }

                //   Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (//(this->pOrgsInIds[l][b].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (//(this->pOrgsAftIds[l][b].size() > SIZE_THRESHOLD) &&
                        (computeRelevanceLowerBound_MinStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MinStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, result);
                    else if (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRelevance, result);
                }
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}*/


size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound);
//    return this->executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(q, theta, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound);
//    return this->executeBottomUp_RelativeRecordRelativeRelevanceThreshold_UsingBothBounds(q, theta, computeRelativeRelevance, computeRelativeRelevanceLowerBound_MinStartMinEnd, computeRelativeRelevanceLowerBound_MinStartMaxEnd, computeRelativeRelevanceLowerBound_MaxStartMinEnd, computeRelativeRelevanceLowerBound_MaxStartMaxEnd, computeRelativeRelevanceUpperBound);
}

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound);
//    return this->executeBottomUp_RelativeRecordRelativeRelevanceThreshold_UsingBothBounds(q, theta, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound_MinStartMinEnd, computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd, computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd, computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd, computeRecordRelativeRelevanceUpperBound);
}
/*
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    float th = ((int(theta) != theta)? theta: int(theta) >> (this->maxBits-this->numBits));      // When the absolute relevance function is used, we need to convert it first to an int
    bool foundzero = false;
    bool foundone = false;

    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
            {
                if ((a == b) && (min(computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsInMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsInMetadata, qa, qb)) > th) ||
                    (a != b) && (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsInMetadata, qa, qb) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, result);
                else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
            }
            if (!this->pRepsAftIds[l][a].empty())
            {
                if ((a == b) && (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) ||
                    (a > b) && (min(computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb)) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
            }

            // Handle rest: consider only originals
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if ((computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsInMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
//                    if (min(computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb)) > th)
                    if ((computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) && (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
            }
            else
            {
                if (!this->pOrgsInIds[l][a].empty())
                {
                    if (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][a].empty())
                {
//                    if (min(computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb)) > th)
                    if ((computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }

                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRecordRelativeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (min(computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb)) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRecordRelativeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                    }

                }
                
                if (!this->pOrgsInIds[l][b].empty())
                {
                    if (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    if (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th)
//                    if (computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb) > th)
//                    if (min(computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, b, this->pOrgsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, b, this->pOrgsAftMetadata, qa, qb)) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                    this->scanPartition_CheckBothTimestamps_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRecordRelativeRelevance, result);
                if ((!this->pOrgsAftIds[l][a].empty()) && (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                    this->scanPartition_CheckStart_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRecordRelativeRelevance, result);
            }

            else
            {
                // Lemma 1
                if ((!this->pOrgsInIds[l][a].empty()) && (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > th))
                    this->scanPartition_CheckEnd_gOverlaps(l, a, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    if (
                        //(l == 0) && (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) ||
                        //(l > 0) &&
                        (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th) &&
                        (computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, a, this->pOrgsAftMetadata, qa, qb) > th))
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, result);
                    else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > th)
                        this->scanPartition_NoChecks_gOverlaps(l, a, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                }
            }

            // Lemma 1, 3
            if ((!this->pRepsInIds[l][a].empty()) && (computeRecordRelativeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > th))
                this->scanPartition_CheckEnd_gOverlaps(l, a, this->pRepsInIds, this->pRepsInTimestamps, CompareTimestampPairsByEnd, q, theta, computeRecordRelativeRelevance, result);
            if (!this->pRepsAftIds[l][a].empty())
            {
//                if (computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb) > th)
//                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
//                else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
//                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRelevance, result);
                if ((a == b) && (computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb) > th) ||
                    (a > b) && (min(computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(l, a, this->pRepsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(l, a, this->pRepsAftMetadata, qa, qb)) > th))
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, result);
                else if (computeRecordRelativeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > th)
                    this->scanPartition_NoChecks_gOverlaps(l, a, this->pRepsAftIds, this->pRepsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
            }

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        if (computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, result);
                        else if (computeRecordRelativeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsInIds, this->pOrgsInTimestamps, q, theta, computeRecordRelativeRelevance, result);
                    }

                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        if (min(computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(l, i, this->pOrgsAftMetadata, qa, qb), computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(l, i, this->pOrgsAftMetadata, qa, qb)) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, result);
                        else if (computeRecordRelativeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > th)
                            this->scanPartition_NoChecks_gOverlaps(l, i, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, theta, computeRecordRelativeRelevance, result);
                    }
                }

                //   Handle the partition that contains b: consider only originals, comparisons needed
                if ((!this->pOrgsInIds[l][b].empty()) && (computeRecordRelativeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > th))
                    this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsInIds, this->pOrgsInTimestamps, CompareTimestampPairsByStart, q, theta, computeRecordRelativeRelevance, result);
                if ((!this->pOrgsAftIds[l][b].empty()) && (computeRecordRelativeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > th))
                    this->scanPartition_CheckStart_gOverlaps(l, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, CompareTimestampPairsByStart, q, theta, computeRecordRelativeRelevance, result);
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            if (computeRecordRelativeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRecordRelativeRelevance(*iter, q) > theta)
            {
#ifdef WORKLOAD_COUNT
                result++;
#else
                result ^= (*iterI);
#endif
            }
            iterI++;
        }
    }


    return result;
}*/

size_t HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(const RangeQuery &q, const float theta)
{
    return this->executeBottomUp_gOverlaps_RelevanceThreshold_UsingBothBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound);
//    return this->executeBottomUp_AbsoluteQueryRelativeRelevanceThreshold_UsingBothBounds(q, theta, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound);
}



// Top-k relevance: top-down search using no bounds
inline void HINT_M_SubsSort_CM::executeTopDown_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    float qrel = computeRelevance(make_pair(q.start, q.end), q);
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        // Handle the partition that contains a: consider both originals and replicas, comparisons needed
        if (a == b)
        {
            this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
        }
        else
        {
            // Lemma 1
            this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
        }

        // Lemma 1, 3
        this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
//        if ((result.size() == k) && (result.top() == qrel))
//            return;
        this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, k, computeRelevance, result);
//        if ((result.size() == k) && (result.top() == qrel))
//            return;

        if (a < b)
        {
            // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsInIds, this->pOrgsInTimestamps, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            for (auto i = a+1; i < b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }

            // Handle the partition that contains b: consider only originals, comparisons needed
            this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
        }

        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
}

void HINT_M_SubsSort_CM::executeTopDown_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeTopDown_gOverlaps_RelevanceTopK(q, k, computeAbsoluteRelevance, result);
}

void HINT_M_SubsSort_CM::executeTopDown_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeTopDown_gOverlaps_RelevanceTopK(q, k, computeRelativeRelevance, result);
}

void HINT_M_SubsSort_CM::executeTopDown_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeTopDown_gOverlaps_RelevanceTopK(q, k, computeRecordRelativeRelevance, result);
}

void HINT_M_SubsSort_CM::executeTopDown_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeTopDown_gOverlaps_RelevanceTopK(q, k, computeQueryRelativeRelevance, result);
}


// Top-k relevance: bottom-up search using no bounds
inline void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceTopK(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), priority_queue<float, vector<float>, greater<float>> &result)
{
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    bool foundzero = false;
    bool foundone = false;
//    float qrel = computeRelevance(make_pair(0, std::numeric_limits<int>::max()), q);
    float qrel = computeRelevance(make_pair(q.start, q.end), q);


    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            // Handle rest: consider only originals
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsInIds, this->pOrgsInTimestamps, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
//            this->scanPartitions_NoChecks_gOverlaps(l, a, b, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            for (auto i = a; i <= b; i++)
            {
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
            else
            {
                // Lemma 1
                this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }

            // Lemma 1, 3
            this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;
            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsInIds, this->pOrgsInTimestamps, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, this->pOrgsAftTimestamps, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                for (auto i = a+1; i < b; i++)
                {
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
    if (foundone && foundzero)
    {
        // All contents are guaranteed to be results
        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
        {
            updateTopK(result, k, computeRelevance(*iter, q));
            
            iter++;
        }
    }
    else
    {
        // Comparisons needed
        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            if ((iter->first <= q.end) && (q.start <= iter->second))
                updateTopK(result, k, computeRelevance(*iter, q));

            iterI++;
        }
    }
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK(q, k, computeAbsoluteRelevance, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK(q, k, computeRelativeRelevance, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK(q, k, computeRecordRelativeRelevance, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK(q, k, computeQueryRelativeRelevance, result);
}


// Top-k relevance: bottom-up search using upper bounds
inline void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result)
{
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
//    float qrel = computeRelevance(make_pair(0, std::numeric_limits<int>::max()), q);
    float qrel = computeRelevance(make_pair(q.start, q.end), q);
    
//    cout << "Query: [" << qa << "," << qb << "]" << endl;
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
//            cout << "level = " << l << ", p" << a << endl;
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if ((!this->pRepsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > result.top())))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if ((!this->pRepsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > result.top())))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            // Handle rest: consider only originals
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                if ((!this->pOrgsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
            else
            {
                if ((!this->pOrgsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                if ((!this->pOrgsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                for (auto i = a+1; i < b; i++)
                {
//                    cout << "level = " << l << ", p" << i << endl;
                    if ((!this->pOrgsInIds[l][i].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > result.top())))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;
                    
                    if ((!this->pOrgsAftIds[l][i].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > result.top())))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;
                }
                
//                cout << "level = " << l << ", p" << b << endl;
                if ((!this->pOrgsInIds[l][b].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
                
                if ((!this->pOrgsAftIds[l][b].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if ((!this->pOrgsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                if ((!this->pOrgsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
            else
            {
                // Lemma 1
                if ((!this->pOrgsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                if ((!this->pOrgsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }
//            cout << "level = " << l << ", p" << a << endl;

            // Lemma 1, 3
            if ((!this->pRepsInIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) > result.top())))
                this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if ((!this->pRepsAftIds[l][a].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) > result.top())))
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], this->pRepsAftTimestamps[l][a], q, k, computeRelevance, result);
//            if ((result.size() == k) && (result.top() == qrel))
//                return;

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i <= b-1; i++)
                {
//                    cout << "level = " << l << ", p" << i << endl;

                    if ((!this->pOrgsInIds[l][i].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) > result.top())))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], this->pOrgsInTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;

                    if ((!this->pOrgsAftIds[l][i].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) > result.top())))
                        this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], this->pOrgsAftTimestamps[l][i], q, k, computeRelevance, result);
//                    if ((result.size() == k) && (result.top() == qrel))
//                        return;
                }
//                cout << "level = " << l << ", p" << b << endl;

                // Handle the partition that contains b: consider only originals, comparisons needed
                if ((!this->pOrgsInIds[l][b].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) > result.top())))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;

                if ((!this->pOrgsAftIds[l][b].empty()) && ((result.size() < k) || (computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) > result.top())))
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, k, computeRelevance, result);
//                if ((result.size() == k) && (result.top() == qrel))
//                    return;
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
//
//    // Handle root.
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
//        auto iter = this->pOrgsInTimestamps[this->numBits][0].begin();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//            if (computeRelevance(*iter, q) > theta)
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iter++;
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
//        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
//        for (auto iter = iterBegin; iter != iterEnd; iter++)
//        {
//            if ((iter->first <= q.end) && (q.start <= iter->second) && computeRelevance(*iter, q) > theta)
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iterI++;
//        }
//    }
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBottomUp_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound, result);
}


// Top-k relevance: best-first search using upper bounds
inline void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result)
#ifdef PQUEUE
{
//    cout << "PQUEUE" << endl;
    priority_queue<QEntry, vector<QEntry>> Q;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
                Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, false, -1));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));
            
            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
                if (!this->pOrgsInIds[l][i].empty())
                    Q.push(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                if (!this->pOrgsAftIds[l][i].empty())
                    Q.push(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 2));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 0));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
                Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, true, 0));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                        Q.push(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                    if (!this->pOrgsAftIds[l][i].empty())
                        Q.push(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                    Q.push(QEntry(l, b, 0, computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb), true, true, true, 1));
                if (!this->pOrgsAftIds[l][b].empty())
                    Q.push(QEntry(l, b, 0, computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//#ifdef WORKLOAD_COUNT
//            result++;
//#else
//            result ^= (*iterI);
//#endif
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
//        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
//        for (auto iter = iterBegin; iter != iterEnd; iter++)
//        {
//            if ((iter->first <= q.end) && (q.start <= iter->second))
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iterI++;
//        }
//    }
    
    while (!Q.empty())
    {
        QEntry e = Q.top();
        
//        cout << "<l = " << e.level << ",\tp = " << e.pid << ",\tUB = " << e.upperBound << ",\tLB = " << e.lowerBound << "\t" << ((e.isOriginal)? "Orgs": "Reps") << ((e.isIn)? "In": "Aft") << "\ttheta = " << ((result.size() == k)? result.top(): 0) << ">" << endl;
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;

//        if (e.performCheck)
//        {
//            cout << "check ";
//            if (e.checkWhat == 2)
//                cout << "both start and end";
//            else if (e.checkWhat == 1)
//                cout << "both start";
//            else if (e.checkWhat     == 0)
//                cout << "end";
//        }
//        else
//            cout << "no comps" <<  ">";
//        cout << endl;
        Q.pop();
//        continue;
        
        if (e.isOriginal)
        {
            if (e.isIn)
            {
                if (e.performCheck)
                {
                    if (e.checkWhat == 2)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 1)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 0)
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
                }
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
            {
//                cout << "\tpOrgsAft[" << e.level << "][" << e.pid << "]:";
//                auto iterIBegin = this->pOrgsInIds[e.level][e.pid].begin();
//                auto iterIEnd = this->pOrgsInIds[e.level][e.pid].end();
//                auto iter = this->pOrgsInTimestamps[e.level][e.pid].begin();
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    cout << " <" << (*iterI) << "," << computeRelevance(*iter, q) << ">";
//                    iter++;
//                }
//                cout<<endl;

                if (e.performCheck)
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
        }
        else
        {
            if (e.isIn)
            {
                if (e.performCheck)
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[e.level][e.pid], this->pRepsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
        }
    }
}
#else
{
    vector<QEntry> Q;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
                Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, false, -1));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));
            
            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
                if (!this->pOrgsInIds[l][i].empty())
                    Q.push_back(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                if (!this->pOrgsAftIds[l][i].empty())
                    Q.push_back(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 2));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 0));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
                Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, true, 0));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push_back(QEntry(l, a, 0, computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                        Q.push_back(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                    if (!this->pOrgsAftIds[l][i].empty())
                        Q.push_back(QEntry(l, i, 0, computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                    Q.push_back(QEntry(l, b, 0, computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb), true, true, true, 1));
                if (!this->pOrgsAftIds[l][b].empty())
                    Q.push_back(QEntry(l, b, 0, computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//#ifdef WORKLOAD_COUNT
//            result++;
//#else
//            result ^= (*iterI);
//#endif
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
//        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
//        for (auto iter = iterBegin; iter != iterEnd; iter++)
//        {
//            if ((iter->first <= q.end) && (q.start <= iter->second))
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iterI++;
//        }
//    }
    
    sort(Q.begin(), Q.end(), compareQEntries);

    for (const QEntry &e: Q)
    {
//        cout << "<l = " << e.level << ",\tp = " << e.pid << ",\tUB = " << e.upperBound << ",\tLB = " << e.lowerBound << "\t" << ((e.isOriginal)? "Orgs": "Reps") << ((e.isIn)? "In": "Aft") << "\ttheta = " << ((result.size() == k)? result.top(): 0) << ">" << endl;
        
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;

//        if (e.performCheck)
//        {
//            cout << "check ";
//            if (e.checkWhat == 2)
//                cout << "both start and end";
//            else if (e.checkWhat == 1)
//                cout << "both start";
//            else if (e.checkWhat     == 0)
//                cout << "end";
//        }
//        else
//            cout << "no comps" <<  ">";
//        cout << endl;
//        continue;
        
        if (e.isOriginal)
        {
            if (e.isIn)
            {
                if (e.performCheck)
                {
                    if (e.checkWhat == 2)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 1)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 0)
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
                }
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
            {
//                cout << "\tpOrgsAft[" << e.level << "][" << e.pid << "]:";
//                auto iterIBegin = this->pOrgsInIds[e.level][e.pid].begin();
//                auto iterIEnd = this->pOrgsInIds[e.level][e.pid].end();
//                auto iter = this->pOrgsInTimestamps[e.level][e.pid].begin();
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    cout << " <" << (*iterI) << "," << computeRelevance(*iter, q) << ">";
//                    iter++;
//                }
//                cout<<endl;

                if (e.performCheck)
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
        }
        else
        {
            if (e.isIn)
            {
                if (e.performCheck)
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[e.level][e.pid], this->pRepsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
        }
    }
}
#endif

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingUpperBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingUpperBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceUpperBound, result);
}


// Top-k relevance: best-first search using both lower and upper bounds
inline void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), priority_queue<float, vector<float>, greater<float>> &result)
#ifdef PQUEUE
{
    priority_queue<QEntry, vector<QEntry>> Q;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
                Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, false, -1));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));
            
            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
                if (!this->pOrgsInIds[l][i].empty())
                    Q.push(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                if (!this->pOrgsAftIds[l][i].empty())
                    Q.push(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 2));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 0));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
                Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, true, 0));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                        Q.push(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                    if (!this->pOrgsAftIds[l][i].empty())
                        Q.push(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                    Q.push(QEntry(l, b, computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb), true, true, true, 1));
                if (!this->pOrgsAftIds[l][b].empty())
                    Q.push(QEntry(l, b, computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//#ifdef WORKLOAD_COUNT
//            result++;
//#else
//            result ^= (*iterI);
//#endif
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
//        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
//        for (auto iter = iterBegin; iter != iterEnd; iter++)
//        {
//            if ((iter->first <= q.end) && (q.start <= iter->second))
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iterI++;
//        }
//    }
    
    while (!Q.empty())
    {
        QEntry e = Q.top();
        
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;

//        cout << "<l = " << e.level << ",\tp = " << e.pid << ",\t\tLB = " << e.lowerBound << ",\tUB = " << e.upperBound << ",\tp" << ((e.isOriginal)? "Orgs": "Reps") << ((e.isIn)? "In": "Aft") << " ";
//        if (e.performCheck)
//        {
//            cout << "check ";
//            if (e.checkWhat == 2)
//                cout << "both start and end";
//            else if (e.checkWhat == 1)
//                cout << "both start";
//            else if (e.checkWhat     == 0)
//                cout << "end";
//        }
//        else
//            cout << "no comps" <<  ">";
//        cout << endl;
        Q.pop();
//        continue;
        
        if (e.isOriginal)
        {
            if (e.isIn)
            {
                if (e.performCheck)
                {
                    if (e.checkWhat == 2)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 1)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 0)
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
                }
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
            {
//                cout << "\tpOrgsAft[" << e.level << "][" << e.pid << "]:";
//                auto iterIBegin = this->pOrgsInIds[e.level][e.pid].begin();
//                auto iterIEnd = this->pOrgsInIds[e.level][e.pid].end();
//                auto iter = this->pOrgsInTimestamps[e.level][e.pid].begin();
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    cout << " <" << (*iterI) << "," << computeRelevance(*iter, q) << ">";
//                    iter++;
//                }
//                cout<<endl;

                if (e.performCheck)
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
        }
        else
        {
            if (e.isIn)
            {
                if (e.performCheck)
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[e.level][e.pid], this->pRepsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
        }
    }
}
#else
{
    vector<QEntry> Q;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
            if (!this->pRepsInIds[l][a].empty())
                Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, false, -1));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));
            
            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
                if (!this->pOrgsInIds[l][i].empty())
                    Q.push_back(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                if (!this->pOrgsAftIds[l][i].empty())
                    Q.push_back(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 2));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            else
            {
                // Lemma 1
                if (!this->pOrgsInIds[l][a].empty())
                    Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb), true, true, true, 0));
                if (!this->pOrgsAftIds[l][a].empty())
                    Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
            }

            // Lemma 1, 3
            if (!this->pRepsInIds[l][a].empty())
                Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb), false, true, true, 0));
            if (!this->pRepsAftIds[l][a].empty())
                Q.push_back(QEntry(l, a, computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb), computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb), false, false, false, -1));

            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
                for (auto i = a+1; i < b; i++)
                {
                    if (!this->pOrgsInIds[l][i].empty())
                        Q.push_back(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb), true, true, false, -1));
                    if (!this->pOrgsAftIds[l][i].empty())
                        Q.push_back(QEntry(l, i, computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb), true, false, false, -1));
                }

                // Handle the partition that contains b: consider only originals, comparisons needed
                if (!this->pOrgsInIds[l][b].empty())
                    Q.push_back(QEntry(l, b, computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb), computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb), true, true, true, 1));
                if (!this->pOrgsAftIds[l][b].empty())
                    Q.push_back(QEntry(l, b, computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb), computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb), true, false, true, 1));
            }

            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
    
    // Handle root.
//    if (foundone && foundzero)
//    {
//        // All contents are guaranteed to be results
//        auto iterIBegin = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterIEnd = this->pOrgsInIds[this->numBits][0].end();
//        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//        {
//#ifdef WORKLOAD_COUNT
//            result++;
//#else
//            result ^= (*iterI);
//#endif
//        }
//    }
//    else
//    {
//        // Comparisons needed
//        auto iterI = this->pOrgsInIds[this->numBits][0].begin();
//        auto iterBegin = this->pOrgsInTimestamps[this->numBits][0].begin();
//        auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[this->numBits][0].end(), make_pair<Timestamp, Timestamp>(q.end+1, q.end+1), CompareTimestampPairsByStart);
//        for (auto iter = iterBegin; iter != iterEnd; iter++)
//        {
//            if ((iter->first <= q.end) && (q.start <= iter->second))
//            {
//#ifdef WORKLOAD_COUNT
//                result++;
//#else
//                result ^= (*iterI);
//#endif
//            }
//            iterI++;
//        }
//    }
    
    sort(Q.begin(), Q.end(), compareQEntries);
    
    for (const QEntry &e: Q)
    {
        if ((result.size() == k) && (e.upperBound <= result.top()))
            return;

//        cout << "<l = " << e.level << ",\tp = " << e.pid << ",\t\tLB = " << e.lowerBound << ",\tUB = " << e.upperBound << ",\tp" << ((e.isOriginal)? "Orgs": "Reps") << ((e.isIn)? "In": "Aft") << " ";
//        if (e.performCheck)
//        {
//            cout << "check ";
//            if (e.checkWhat == 2)
//                cout << "both start and end";
//            else if (e.checkWhat == 1)
//                cout << "both start";
//            else if (e.checkWhat     == 0)
//                cout << "end";
//        }
//        else
//            cout << "no comps" <<  ">";
//        cout << endl;
//        continue;
        
        if (e.isOriginal)
        {
            if (e.isIn)
            {
                if (e.performCheck)
                {
                    if (e.checkWhat == 2)
                        this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 1)
                        this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                    else if (e.checkWhat == 0)
                        this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
                }
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[e.level][e.pid], this->pOrgsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
            {
//                cout << "\tpOrgsAft[" << e.level << "][" << e.pid << "]:";
//                auto iterIBegin = this->pOrgsInIds[e.level][e.pid].begin();
//                auto iterIEnd = this->pOrgsInIds[e.level][e.pid].end();
//                auto iter = this->pOrgsInTimestamps[e.level][e.pid].begin();
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    cout << " <" << (*iterI) << "," << computeRelevance(*iter, q) << ">";
//                    iter++;
//                }
//                cout<<endl;

                if (e.performCheck)
                    this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], CompareTimestampPairsByStart, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[e.level][e.pid], this->pOrgsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
        }
        else
        {
            if (e.isIn)
            {
                if (e.performCheck)
                    this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], CompareTimestampPairsByEnd, q, k, computeRelevance, result);
                else
                    this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[e.level][e.pid], this->pRepsInTimestamps[e.level][e.pid], q, k, computeRelevance, result);
            }
            else
                this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[e.level][e.pid], this->pRepsAftTimestamps[e.level][e.pid], q, k, computeRelevance, result);
        }
    }
}
#endif

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_AbsoluteRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_RecordRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound, result);
}

void HINT_M_SubsSort_CM::executeBestFirst_gOverlaps_QueryRelativeRelevanceTopK_UsingBothBounds(const RangeQuery &q, const unsigned int k, priority_queue<float, vector<float>, greater<float>> &result)
{
    this->executeBestFirst_gOverlaps_RelevanceTopK_UsingBothBounds(q, k, computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound, result);
}



//inline void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
//{
//    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
//    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
//    Timestamp qa = q.start;
//    Timestamp qb = q.end;
//
//
//    for (auto l = 0; l < this->numBits; l++)
//    {
//        // For each relevant partition
//        for (auto i = a; i <= b; i++)
//        {
//            // OrgsIn
//            if (!this->pOrgsInIds[l][i].empty())
//            {
//                auto iterIBegin = this->pOrgsInIds[l][i].begin();
//                auto iterIEnd = this->pOrgsInIds[l][i].end();
//                auto iter = this->pOrgsInTimestamps[l][i].begin();
//                Timestamp LB = qb-qa+1;
//                Timestamp UB = 0;
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    auto rel = computeRelevance(*iter, q);
//                    
////                    if (rel < 0)
////                        cout << "r" << *iterI << " [" << iter->first << ".." << iter->second << "] Vs q[" << qa << ".." << qb << "]" << endl;
//
//                    if (rel < LB)
//                        LB = rel;
//                    if (rel > UB)
//                        UB = rel;
//                }
////                cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << endl;
//                cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb)) << endl;
//            }
//            
//            // OrgsAft
//            if (!this->pOrgsAftIds[l][i].empty())
//            {
//                auto iterIBegin = this->pOrgsAftIds[l][i].begin();
//                auto iterIEnd = this->pOrgsAftIds[l][i].end();
//                auto iter = this->pOrgsAftTimestamps[l][i].begin();
//                Timestamp LB = qb-qa+1;
//                Timestamp UB = 0;
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    auto rel = computeRelevance(*iter, q);
//                    
//                    if (rel < LB)
//                        LB = rel;
//                    if (rel > UB)
//                        UB = rel;
//                }
////                cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) << endl;
//                cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb)) << endl;
//            }
//            
//            // RepsIn
//            if (!this->pRepsInIds[l][i].empty())
//            {
//                auto iterIBegin = this->pRepsInIds[l][i].begin();
//                auto iterIEnd = this->pRepsInIds[l][i].end();
//                auto iter = this->pRepsInTimestamps[l][i].begin();
//                Timestamp LB = qb-qa+1;
//                Timestamp UB = 0;
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    auto rel = computeRelevance(*iter, q);
//                    
//                    if (rel < LB)
//                        LB = rel;
//                    if (rel > UB)
//                        UB = rel;
//                }
////                cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << endl;
//                cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pRepsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pRepsInMetadata, qa, qb)) << endl;
//            }
//            
//            // RepsAft
//            if (!this->pRepsAftIds[l][i].empty())
//            {
//                auto iterIBegin = this->pRepsAftIds[l][i].begin();
//                auto iterIEnd = this->pRepsAftIds[l][i].end();
//                auto iter = this->pRepsAftTimestamps[l][i].begin();
//                Timestamp LB = qb-qa+1;
//                Timestamp UB = 0;
//                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
//                {
//                    auto rel = computeRelevance(*iter, q);
//                    
//                    if (rel < LB)
//                        LB = rel;
//                    if (rel > UB)
//                        UB = rel;
//                }
////                cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << endl;
//                cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pRepsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pRepsAftMetadata, qa, qb)) << endl;
//            }
//        }
//        a >>= 1; // a = a div 2
//        b >>= 1; // b = b div 2
//    }
//}


inline void HINT_M_SubsSort_CM::executeBottomUp_gOverlaps_MeasureAccuracy(const RangeQuery &q, float (*computeRelevance)(const pair<Timestamp, Timestamp>&, const RangeQuery&), float (*computeRelevanceLowerBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb), float (*computeRelevanceUpperBound)(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb))
{
    size_t result = 0;
    Timestamp a = q.start >> (this->maxBits-this->numBits); // prefix
    Timestamp b = q.end   >> (this->maxBits-this->numBits); // prefix
    Timestamp qa = q.start;
    Timestamp qb = q.end;
    bool foundzero = false;
    bool foundone = false;
    
    
    for (auto l = 0; l < this->numBits; l++)
    {
        if (foundone && foundzero)
        {
            // Partition totally covers lowest-level partition range that includes query range
            // all contents are guaranteed to be results
            
            // Handle the partition that contains a: consider both originals and replicas
//            this->scanPartition_NoChecks_gOverlaps(this->pRepsInIds[l][a], result);
            if (!this->pRepsInIds[l][a].empty())
            {
                auto iterIBegin = this->pRepsInIds[l][a].begin();
                auto iterIEnd = this->pRepsInIds[l][a].end();
                auto iter = this->pRepsInTimestamps[l][a].begin();
                Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                Timestamp UB = 0;
                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                {
                    auto rel = computeRelevance(*iter, q);

                    if (rel < LB)
                        LB = rel;
                    if (rel > UB)
                        UB = rel;
                }
//                cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) << endl;
//                cout << l << "\t" << a << "\t";
//                cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb))/LB << "\t";
//                cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb))/UB << endl;
                cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb)) << endl;
            }
//            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
            if (!this->pRepsAftIds[l][a].empty())
            {
                auto iterIBegin = this->pRepsAftIds[l][a].begin();
                auto iterIEnd = this->pRepsAftIds[l][a].end();
                auto iter = this->pRepsAftTimestamps[l][a].begin();
                Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                Timestamp UB = 0;
                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                {
                    auto rel = computeRelevance(*iter, q);

                    if (rel < LB)
                        LB = rel;
                    if (rel > UB)
                        UB = rel;
                }
//                cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) << endl;
//                cout << l << "\t" << a << "\t";
//                cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb))/LB << "\t";
//                cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb))/UB << endl;
                cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb)) << endl;
            }

            // Handle rest: consider only originals
            for (auto i = a; i <= b; i++)
            {
//                this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                if (!this->pOrgsInIds[l][i].empty())
                {
                    auto iterIBegin = this->pOrgsInIds[l][i].begin();
                    auto iterIEnd = this->pOrgsInIds[l][i].end();
                    auto iter = this->pOrgsInTimestamps[l][i].begin();
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                    }
//                    cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << endl;
//                    cout << l << "\t" << i << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb)) << endl;
                }
//                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                if (!this->pOrgsAftIds[l][i].empty())
                {
                    auto iterIBegin = this->pOrgsAftIds[l][i].begin();
                    auto iterIEnd = this->pOrgsAftIds[l][i].end();
                    auto iter = this->pOrgsAftTimestamps[l][i].begin();
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                    }
//                    cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) << endl;
//                    cout << l << "\t" << i << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb)) << endl;
                }
            }
        }
        else
        {
            // Comparisons needed
            
            // Handle the partition that contains a: consider both originals and replicas, comparisons needed
            if (a == b)
            {
//                this->scanPartition_CheckBothTimestamps_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], CompareTimestampPairsByStart, q, result);
                if (!this->pOrgsInIds[l][a].empty())
                {
                    auto iterI = this->pOrgsInIds[l][a].begin();
                    auto iterBegin = this->pOrgsInTimestamps[l][a].begin();
                    auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[l][a].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    bool check = false;
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        if (q.start <= iter->second)
                        {
                            auto rel = computeRelevance(*iter, q);

                            if (rel < LB)
                                LB = rel;
                            if (rel > UB)
                                UB = rel;
                            
                            check = true;
                        }
                        iterI++;
                    }
                    if (!check)
                        LB = UB = 0;
                    
//                    cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) << endl;
//                    cout << l << "\t" << a << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb))/UB << endl;
                      cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb)) << endl;
                }

//                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][a], this->pOrgsAftTimestamps[l][a], CompareTimestampPairsByStart, q, result);
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    auto iterI = this->pOrgsInIds[l][a].begin();
                    auto iterBegin = this->pOrgsAftTimestamps[l][a].begin();
                    auto iterEnd = lower_bound(iterBegin, this->pOrgsAftTimestamps[l][a].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                        
                        iterI++;
                    }
                    if (iterBegin == iterEnd)
                        LB = UB = 0;
                    
//                    cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) << endl;
//                    cout << l << "\t" << a << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb)) << endl;
                }
            }
            
            else
            {
                // Lemma 1
//                this->scanPartition_CheckEnd_gOverlaps(this->pOrgsInIds[l][a], this->pOrgsInTimestamps[l][a], q, result);
                if (!this->pOrgsInIds[l][a].empty())
                {
                    auto iterI = pOrgsInIds[l][a].begin();
                    auto iterBegin = this->pOrgsInTimestamps[l][a].begin();
                    auto iterEnd = this->pOrgsInTimestamps[l][a].end();
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    bool check = false;
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        if (q.start <= iter->second)
                        {
                            auto rel = computeRelevance(*iter, q);

                            if (rel < LB)
                                LB = rel;
                            if (rel > UB)
                                UB = rel;
                            
                            check = true;
                        }
                        iterI++;
                    }
                    if (!check)
                        LB = UB = 0;

//                  cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) << endl;
//                  cout << l << "\t" << a << "\t";
//                  cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb))/LB << "\t";
//                  cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsInMetadata, qa, qb)) << endl;
                }

//                this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][a], result);
                if (!this->pOrgsAftIds[l][a].empty())
                {
                    auto iterIBegin = this->pOrgsAftIds[l][a].begin();
                    auto iterIEnd = this->pOrgsAftIds[l][a].end();
                    auto iter = this->pOrgsAftTimestamps[l][a].begin();
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                    }
//                    cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) << endl;
//                    cout << l << "\t" << a << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pOrgsAftMetadata, qa, qb)) << endl;
                }
            }
            
            // Lemma 1, 3
//            this->scanPartition_CheckEnd_gOverlaps(this->pRepsInIds[l][a], this->pRepsInTimestamps[l][a], CompareTimestampPairsByEnd, q, result);
            if (!this->pRepsInIds[l][a].empty())
            {
                auto iterI = this->pRepsInIds[l][a].begin();
                auto iterBegin = this->pRepsInTimestamps[l][a].begin();
                auto iterEnd = this->pRepsInTimestamps[l][a].end();
                auto iter = lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), CompareTimestampPairsByEnd);
                Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                Timestamp UB = 0;

                iterI += iter-iterBegin;
                while (iter != iterEnd)
                {
                    auto rel = computeRelevance(*iter, q);

                    if (rel < LB)
                        LB = rel;
                    if (rel > UB)
                        UB = rel;

                    iter++;
                    iterI++;
                }
                if (iterEnd == lower_bound(iterBegin, iterEnd, make_pair(q.start, q.start), CompareTimestampPairsByEnd))
                    LB = UB = 0;
                
//                cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) << endl;
//                cout << l << "\t" << a << "\t";
//                cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb))/LB << "\t";
//                cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb))/UB << endl;
                cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsInMetadata, qa, qb)) << endl;
            }

//            this->scanPartition_NoChecks_gOverlaps(this->pRepsAftIds[l][a], result);
            if (!this->pRepsAftIds[l][a].empty())
            {
                auto iterIBegin = this->pRepsAftIds[l][a].begin();
                auto iterIEnd = this->pRepsAftIds[l][a].end();
                auto iter = this->pRepsAftTimestamps[l][a].begin();
                Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                Timestamp UB = 0;
                for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                {
                    auto rel = computeRelevance(*iter, q);

                    if (rel < LB)
                        LB = rel;
                    if (rel > UB)
                        UB = rel;
                }
//                cout << l << "\t" << a << "\t" << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) << endl;
//                cout << l << "\t" << a << "\t";
//                cout << LB << "\t" << computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb))/LB << "\t";
//                cout << UB << "\t" << computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb))/UB << endl;
                cout << l << "\t" << a << "\t" << abs(LB-computeRelevanceLowerBound(l, a, this->pRepsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, a, this->pRepsAftMetadata, qa, qb)) << endl;
            }
            
            if (a < b)
            {
                // Handle the rest before the partition that contains b: consider only originals, no comparisons needed
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsInIds, result);
//                this->scanPartitions_NoChecks_gOverlaps(l, a+1, b-1, this->pOrgsAftIds, result);
                for (auto i = a+1; i < b; i++)
                {
//                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsInIds[l][i], result);
                    if (!this->pOrgsInIds[l][i].empty())
                    {
                        auto iterIBegin = this->pOrgsInIds[l][i].begin();
                        auto iterIEnd = this->pOrgsInIds[l][i].end();
                        auto iter = this->pOrgsInTimestamps[l][i].begin();
                        Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                        Timestamp UB = 0;
                        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                        {
                            auto rel = computeRelevance(*iter, q);

                            if (rel < LB)
                                LB = rel;
                            if (rel > UB)
                                UB = rel;
                        }
//                        cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << endl;
//                        cout << l << "\t" << i << "\t";
//                        cout << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb))/LB << "\t";
//                        cout << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb))/UB << endl;
                        cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsInMetadata, qa, qb)) << endl;
                    }
//                    this->scanPartition_NoChecks_gOverlaps(this->pOrgsAftIds[l][i], result);
                    if (!this->pOrgsAftIds[l][i].empty())
                    {
                        auto iterIBegin = this->pOrgsAftIds[l][i].begin();
                        auto iterIEnd = this->pOrgsAftIds[l][i].end();
                        auto iter = this->pOrgsAftTimestamps[l][i].begin();
                        Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                        Timestamp UB = 0;
                        for (auto iterI = iterIBegin; iterI != iterIEnd; iterI++)
                        {
                            auto rel = computeRelevance(*iter, q);

                            if (rel < LB)
                                LB = rel;
                            if (rel > UB)
                                UB = rel;
                        }
//                        cout << l << "\t" << i << "\t" << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) << endl;
//                        cout << l << "\t" << i << "\t";
//                        cout << LB << "\t" << computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb))/LB << "\t";
//                        cout << UB << "\t" << computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb))/UB << endl;
                        cout << l << "\t" << i << "\t" << abs(LB-computeRelevanceLowerBound(l, i, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, i, this->pOrgsAftMetadata, qa, qb)) << endl;
                    }
                }
                
                // Handle the partition that contains b: consider only originals, comparisons needed
//                this->scanPartition_CheckStart_gOverlaps(this->pOrgsInIds[l][b], this->pOrgsInTimestamps[l][b], CompareTimestampPairsByStart, q, result);
                if (!this->pOrgsInIds[l][b].empty())
                {
                    auto iterI = this->pOrgsInIds[l][b].begin();
                    auto iterBegin = this->pOrgsInTimestamps[l][b].begin();
                    auto iterEnd = lower_bound(iterBegin, this->pOrgsInTimestamps[l][b].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                        
                        iterI++;
                    }
                    if (iterBegin == iterEnd)
                        LB = UB = 0;
                    
//                    cout << l << "\t" << b << "\t" << LB << "\t" << computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) << endl;
//                    cout << l << "\t" << b << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << b << "\t" << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsInMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsInMetadata, qa, qb)) << endl;
                }

//                this->scanPartition_CheckStart_gOverlaps(this->pOrgsAftIds[l][b], this->pOrgsAftTimestamps[l][b], CompareTimestampPairsByStart, q, result);
                if (!this->pOrgsAftIds[l][b].empty())
                {
                    auto iterI = this->pOrgsInIds[l][b].begin();
                    auto iterBegin = this->pOrgsAftTimestamps[l][b].begin();
                    auto iterEnd = lower_bound(iterBegin, this->pOrgsAftTimestamps[l][b].end(), pair<Timestamp, Timestamp>(q.end+1, q.end+1));
                    Timestamp LB = computeRelevance(make_pair(qa, qb), q);
                    Timestamp UB = 0;
                    for (auto iter = iterBegin; iter != iterEnd; iter++)
                    {
                        auto rel = computeRelevance(*iter, q);

                        if (rel < LB)
                            LB = rel;
                        if (rel > UB)
                            UB = rel;
                        
                        iterI++;
                    }
                    if (iterBegin == iterEnd)
                        LB = UB = 0;
                    
//                    cout << l << "\t" << b << "\t" << LB << "\t" << computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) << "\t" << UB << "\t" << computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) << endl;
//                    cout << l << "\t" << b << "\t";
//                    cout << LB << "\t" << computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb)) << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb))/LB << "\t";
//                    cout << UB << "\t" << computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb))/UB << endl;
                    cout << l << "\t" << b << "\t" << abs(LB-computeRelevanceLowerBound(l, b, this->pOrgsAftMetadata, qa, qb)) << "\t" << abs(UB-computeRelevanceUpperBound(l, b, this->pOrgsAftMetadata, qa, qb)) << endl;
                }
            }
            
            if (b%2) //last bit of b is 1
                foundone = 1;
            if (!(a%2)) //last bit of a is 0
                foundzero = 1;
        }
        a >>= 1; // a = a div 2
        b >>= 1; // b = b div 2
    }
}
