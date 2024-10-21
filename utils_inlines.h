// Compute relevance
inline float computeAbsoluteRelevance(const Record& r, const RangeQuery& q)
{
    auto rel = min(r.end, q.end) - max(r.start, q.start) + 1;
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevance(const Record& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.end, q.end) - max(r.start, q.start) + 1)/(max(r.end, q.end) - min(r.start, q.start) + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRecordRelativeRelevance(const Record& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.end, q.end) - max(r.start, q.start) + 1)/(r.end - r.start + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeQueryRelativeRelevance(const Record& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.end, q.end) - max(r.start, q.start) + 1)/(q.end - q.start + 1);
    
    return ((rel > 0)? rel: 0);
}


inline float computeAbsoluteRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q)
{
    auto rel = min(r.second, q.end) - max(r.first, q.start) + 1;
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.second, q.end) - max(r.first, q.start) + 1)/(max(r.second, q.end) - min(r.first, q.start) + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRecordRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.second, q.end) - max(r.first, q.start) + 1)/(r.second - r.first + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeQueryRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q)
{
    auto rel = (float)(min(r.second, q.end) - max(r.first, q.start) + 1)/(q.end - q.start + 1);
    
    return ((rel > 0)? rel: 0);
}


// Compute relevance lower bound
inline float computeAbsoluteRelevanceLowerBound(Metadata &md, const RangeQuery &q)
{
    auto rel = (float)(min(q.end,md.minEnd) - max(q.start,md.maxStart) + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q)
{
    float rel = (float)(min(q.end,md.minEnd) - max(q.start,md.minStart) + 1)/(max(q.end, md.minEnd)-min(q.start, md.minStart)+1);
    float result = rel;

    rel = (float)(min(q.end,md.maxEnd) - max(q.start,md.minStart) + 1)/(max(q.end, md.maxEnd)-min(q.start, md.minStart)+1);
    if (rel < result)
        result = rel;
    
    if (md.minEnd >= md.maxStart)
    {
        rel = (float)(min(q.end,md.minEnd) - max(q.start,md.maxStart) + 1)/(max(q.end, md.minEnd)-min(q.start, md.maxStart)+1);
        if (rel < result)
            result = rel;
    }
    
    rel = (float)(min(q.end,md.maxEnd) - max(q.start,md.maxStart) + 1)/(max(q.end, md.maxEnd)-min(q.start, md.maxStart)+1);
    if (rel < result)
        result = rel;

    return ((result > 0)? result: 0);
}

inline float computeRecordRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q)
{
    float rel = (float)(min(q.end,md.minEnd) - max(q.start,md.minStart) + 1)/(md.minEnd-md.minStart+1);
    float result = rel;

    rel = (float)(min(q.end,md.maxEnd) - max(q.start,md.minStart) + 1)/(md.maxEnd-md.minStart+1);
    if (rel < result)
        result = rel;
    
    if (md.minEnd >= md.maxStart)
    {
        rel = (float)(min(q.end,md.minEnd) - max(q.start,md.maxStart) + 1)/(md.minEnd-md.maxStart+1);
        if (rel < result)
            result = rel;
    }
    
    rel = (float)(min(q.end,md.maxEnd) - max(q.start,md.maxStart) + 1)/(md.maxEnd-md.maxStart+1);
    if (rel < result)
        result = rel;

    return ((result > 0)? result: 0);
}

inline float computeQueryRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q)
{
    auto rel = (float)(min(q.end,md.minEnd) - max(q.start,md.maxEnd) + 1)/(q.end-q.start+1);
    
    return ((rel > 0)? rel: 0);
}


inline pair<Timestamp, Timestamp>  getWorstInterval_MinStartMinEnd(Metadata &md, const RangeQuery& q)
{
    auto wstart = md.minStart;
    auto wend   = md.minEnd;
    
    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MinStartMaxEnd(Metadata &md, const RangeQuery& q)
{
    auto wstart = md.minStart;
    auto wend   = md.maxEnd;
    
    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MaxStartMinEnd(Metadata &md, const RangeQuery& q)
{
    auto wstart = md.maxStart;
    auto wend   = md.minEnd;

    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MaxStartMaxEnd(Metadata &md, const RangeQuery& q)
{
    auto wstart = md.maxStart;
    auto wend   = md.maxEnd;

    return make_pair(wstart, wend);
}



inline pair<Timestamp, Timestamp>  getBestInterval(Metadata &md, const RangeQuery &q)
{
    auto bstart = min(max(q.start, md.minStart), md.maxStart);
    auto bend   = max(min(q.end, md.maxEnd), md.minEnd);

    return make_pair(bstart, bend);
}

inline float computeAbsoluteRelevanceUpperBound(Metadata &md, const RangeQuery &q)
{
    pair<Timestamp, Timestamp> p = getBestInterval(md, q);
    auto rel = min(q.end, p.second)-max(q.start, p.first)+1;
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q)
{
    pair<Timestamp, Timestamp> p = getBestInterval(md, q);
    auto rel = (float)(min(q.end, p.second)-max(q.start, p.first)+1)/(max(q.end, p.second)-min(q.start, p.first)+1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRecordRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q)
{
    pair<Timestamp, Timestamp> p = getBestInterval(md, q);
    auto rel = (float)(min(q.end, p.second)-max(q.start, p.first)+1)/(p.second-p.first+1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeQueryRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q)
{
    pair<Timestamp, Timestamp> p = getBestInterval(md, q);
    auto rel = (float)(min(q.end, p.second)-max(q.start, p.first)+1)/(q.end-q.start+1);
    
    return ((rel > 0)? rel: 0);
}




inline pair<Timestamp, Timestamp>  getBestInterval(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto bstart = min(max(qa, md[level][t].minStart), md[level][t].maxStart);
    auto bend   = max(min(qb, md[level][t].maxEnd), md[level][t].minEnd);

    return make_pair(bstart, bend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MinStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto wstart = md[level][t].minStart;
    auto wend   = md[level][t].minEnd;
//    auto wstart = max(qa,md[level][t].minStart);
//    auto wend   = min(qb,md[level][t].maxEnd);

//    cout << "\t\tminStart = " << md[level][t].minStart << endl;
//    cout << "\t\tmaxStart = " << md[level][t].maxStart << endl;
//    cout << "\t\tminEnd = " << md[level][t].minEnd << endl;
//    cout << "\t\tmaxEnd = " << md[level][t].maxEnd << endl;
//    cout << "\t\tworst = [" << wstart <<"," << wend << "]"<<endl;
    
    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MinStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto wstart = md[level][t].minStart;
//    auto wstart = max(qa, md[level][t].minStart);
    auto wend   = md[level][t].maxEnd;
    
//    cout << "\t\tminStart = " << md[level][t].minStart << endl;
//    cout << "\t\tmaxStart = " << md[level][t].maxStart << endl;
//    cout << "\t\tminEnd = " << md[level][t].minEnd << endl;
//    cout << "\t\tmaxEnd = " << md[level][t].maxEnd << endl;
//    cout << "\t\tworst = [" << wstart << ","<<wend<<"]"<<endl;

    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MaxStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto wstart = md[level][t].maxStart;
    auto wend   = md[level][t].minEnd;

//    cout << "\t\tminStart = " << md[level][t].minStart << endl;
//    cout << "\t\tmaxStart = " << md[level][t].maxStart << endl;
//    cout << "\t\tminEnd = " << md[level][t].minEnd << endl;
//    cout << "\t\tmaxEnd = " << md[level][t].maxEnd << endl;
//    cout << "\t\tworst = [" << wstart <<"," << wend << "]"<<endl;
    
    return make_pair(wstart, wend);
}

inline pair<Timestamp, Timestamp>  getWorstInterval_MaxStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto wstart = md[level][t].maxStart;
    auto wend   = md[level][t].maxEnd;

//    cout << "\t\tminStart = " << md[level][t].minStart << endl;
//    cout << "\t\tmaxStart = " << md[level][t].maxStart << endl;
//    cout << "\t\tminEnd = " << md[level][t].minEnd << endl;
//    cout << "\t\tmaxEnd = " << md[level][t].maxEnd << endl;
//    cout << "\t\tworst = [" << wstart << ","<<wend<<"]"<<endl;

    return make_pair(wstart, wend);
}



inline float computeAbsoluteRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getBestInterval(level, t, md, qa, qb);
    auto rel = min(qb, p.second)-max(qa, p.first)+1;
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getBestInterval(level, t, md, qa, qb);
    auto rel = (float)(min(qb, p.second)-max(qa, p.first)+1)/(max(qb, p.second)-min(qa, p.first)+1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRecordRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getBestInterval(level, t, md, qa, qb);
    auto rel = (float)(min(qb, p.second)-max(qa, p.first)+1)/(p.second-p.first+1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeQueryRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getBestInterval(level, t, md, qa, qb);
    auto rel =  (float)(min(qb, p.second)-max(qa, p.first)+1)/(qb-qa+1);
    
    return ((rel > 0)? rel: 0);
}


inline float computeAbsoluteRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMinEnd(level, t, md, qa, qb);
    auto rel = (float)(min(qb,p.second) - max(qa,p.first) + 1);
    
    return ((rel > 0)? rel: 0);
}

inline float computeRelativeRelevanceLowerBound_MinStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MinStartMinEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MinStartMinEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(max(qb,p.second) - min(qa,p.first) + 1);
}

inline float computeRelativeRelevanceLowerBound_MinStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MinStartMaxEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MinStartMaxEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(max(qb,p.second) - min(qa,p.first) + 1);
}

inline float computeRelativeRelevanceLowerBound_MaxStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MaxStartMinEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMinEnd(level, t, md, qa, qb);

//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(max(qb,p.second) - min(qa,p.first) + 1);
}

inline float computeRelativeRelevanceLowerBound_MaxStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MaxStartMaxEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMaxEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;

    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(max(qb,p.second) - min(qa,p.first) + 1);
}

inline float computeRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto rel = min(min(computeRelativeRelevanceLowerBound_MinStartMinEnd(level, t, md, qa, qb), computeRelativeRelevanceLowerBound_MinStartMaxEnd(level, t, md, qa, qb)),
               min(computeRelativeRelevanceLowerBound_MaxStartMinEnd(level, t, md, qa, qb), computeRelativeRelevanceLowerBound_MaxStartMaxEnd(level, t, md, qa, qb)));
    
    return ((rel > 0)? rel: 0);
}

inline float computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MinStartMinEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MinStartMinEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1);
}

inline float computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MinStartMaxEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MinStartMaxEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1);
}

inline float computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MaxStartMinEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMinEnd(level, t, md, qa, qb);

//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;
    
    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1);
}

inline float computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
//    cout << "\tcomputerRelevanceLowerBound_MaxStartMaxEnd with [" << qa << ","<<qb<<"]" << endl;
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMaxEnd(level, t, md, qa, qb);
    
//    cout <<"\tLB["<<level<<"]["<<t<<"] = ("<<min(qb,p.second)<<"-"<<max(qa,p.first)<<"+1) / ("<<p.second<<"-"<<p.first<<"+1) = "<<(float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1)<<endl;

    return (float)(min(qb,p.second) - max(qa,p.first) + 1)/(p.second-p.first+1);
}

inline float computeRecordRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    auto rel = min(min(computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(level, t, md, qa, qb), computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(level, t, md, qa, qb)),
               min(computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(level, t, md, qa, qb), computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(level, t, md, qa, qb)));
    
    return ((rel > 0)? rel: 0);
}

inline float computeQueryRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    pair<Timestamp, Timestamp> p = getWorstInterval_MaxStartMinEnd(level, t, md, qa, qb);
    auto rel = (float)(min(qb,p.second) - max(qa,p.first) + 1)/(qb-qa+1);
    
    return ((rel > 0)? rel: 0);
}




// Computing relevance upper bound
inline float computeUpperBoundAbsoluteRelevance_FirstLastOrgsIn(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return q.end-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_FirstOrgsIn(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return (a+1)*partition_extent-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_FirstOrgsAft(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return q.end-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_FirstRepsIn(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return (a+1)*partition_extent-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_FirstRepsAft(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return q.end-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_MiddleOrgsIn(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return partition_extent;
}

inline float computeUpperBoundAbsoluteRelevance_MiddleOrgsAft(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return (a+1)*partition_extent-q.start;
}

inline float computeUpperBoundAbsoluteRelevance_LastOrgsIn(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return q.end-a*partition_extent;
}

inline float computeUpperBoundAbsoluteRelevance_LastOrgsAft(unsigned int level, Timestamp a, Timestamp partition_extent, const RangeQuery& q)
{
    return q.end-a*partition_extent;
}



inline float computeUpperBoundAbsoluteRelevance_FirstLastOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_a=b: [max(q.st,minst), min(qb,maxend)]
    auto bstart = max(qa, md[level][t].minStart);
    auto bend   = min(qb, md[level][t].maxEnd);
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_FirstLastOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oaft_a=b: [max(q.st,minst), qb]
    auto bstart = max(qa, md[level][t].minStart);
    auto bend   = qb;
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_FirstOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_a: [max(q.st,minst), maxend]
    auto bstart = max(qa, md[level][t].minStart);
    auto bend   = md[level][t].maxEnd;
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_FirstOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_a: [max(q.st,minst), min(qb,maxend)]
    auto bstart = max(qa, md[level][t].minStart);
    auto bend   = min(qb, md[level][t].maxEnd);
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_FirstRepsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Rin_a: [q.st, min(qb,maxend)]
    auto bstart = qa;
    auto bend   = min(qb, md[level][t].maxEnd);
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_FirstRepsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Raft_a: [q.st, qb]
    auto bstart = qa;
    auto bend   = qb;
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_MiddleOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_a<i<b: [minst, maxend]
    auto bstart = md[level][t].minStart;
    auto bend   = md[level][t].maxEnd;
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_MiddleOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oaft_a<i<b: [minst, min(qb,maxend)]
    auto bstart = md[level][t].minStart;
    auto bend   = min(qb, md[level][t].maxEnd);
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_LastOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_b: [minst, min(qb,maxend)]
    auto bstart = md[level][t].minStart;
    auto bend   = min(qb, md[level][t].maxEnd);
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline float computeUpperBoundAbsoluteRelevance_LastOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb)
{
    // Oin_b: [minst, min(qb,maxend)]
    auto bstart = md[level][t].minStart;
    auto bend   = qb;
    
//    cout<<"\t<"<<bstart<<","<<bend<<">"<<endl;
    return bend-bstart+1;
}

inline void updateTopK(priority_queue<float, vector<float>, greater<float>> &result, const unsigned int k, const float relevance)
{
//    if (result.size() > 0)
//        cout << "top-k (" << result.size() << "): " << result.top() << " Vs " << relevance << endl;
//    if (relevance-0.760807 == 0)
//        cout << "found: " << 0.760807 << endl;
    if (result.size() < k)
        result.push(relevance);
    else if (result.top() < relevance)
    {
        result.pop();
        result.push(relevance);
    }
}
