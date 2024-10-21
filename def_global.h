#pragma once
#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <tuple>
#include <queue>
using namespace std;

// Comment out the following for XOR workload
#define WORKLOAD_COUNT

// Basic predicates of Allen's algebra
#define PREDICATE_EQUALS     1
#define PREDICATE_STARTS     2
#define PREDICATE_STARTED    3
#define PREDICATE_FINISHES   4
#define PREDICATE_FINISHED   5
#define PREDICATE_MEETS      6
#define PREDICATE_MET        7
#define PREDICATE_OVERLAPS   8
#define PREDICATE_OVERLAPPED 9
#define PREDICATE_CONTAINS   10
#define PREDICATE_CONTAINED  11
#define PREDICATE_PRECEDES   12
#define PREDICATE_PRECEDED   13

// Generalized predicates, ACM SIGMOD'22 gOverlaps
#define PREDICATE_GOVERLAPS  14

#define HINT_OPTIMIZATIONS_NO          0
#define HINT_OPTIMIZATIONS_SS          1

#define HINT_M_OPTIMIZATIONS_NO                   0
#define HINT_M_OPTIMIZATIONS_SUBS                 1
#define HINT_M_OPTIMIZATIONS_SUBS_SORT            2
#define HINT_M_OPTIMIZATIONS_SUBS_SOPT            3
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT       4
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_SS    5
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_CM         6
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_CM    7
#define HINT_M_OPTIMIZATIONS_SUBS_SORT_SS_CM      8
#define HINT_M_OPTIMIZATIONS_ALL                  9

#define RELEVANCE_FUNCTION_ABSOLUTE   0
#define RELEVANCE_FUNCTION_RELATIVE   1
#define RELEVANCE_FUNCTION_DRELATIVE  2
#define RELEVANCE_FUNCTION_QRELATIVE  3

#define METADATA_NO                          0
#define METADATA_UNINFORMED_RESCALED_DOMAIN  1
#define METADATA_UNINFORMED_ORIGINAL_DOMAIN  2
#define METADATA_INFORMED_ORIGINAL_DOMAIN    3

#define TRAVERSING_STRATEGY_DEPTHFIRST 0
#define TRAVERSING_STRATEGY_TOPDOWN    1
#define TRAVERSING_STRATEGY_BOTTOMUP   2
#define TRAVERSING_STRATEGY_BESTFIRST  3

#define BOUNDS_NO     0
#define BOUNDS_LOWER  1
#define BOUNDS_UPPER  2
#define BOUNDS_BOTH   3


#define SIZE_THRESHOLD   10000

//#define STATS

//#define LU
#define UL

typedef int PartitionId;
typedef int RecordId;
typedef int Timestamp;

class Record;


struct RunSettings
{
	string       method;
	const char   *dataFile;
	const char   *queryFile;
	bool         verbose;
    unsigned short int typeQuery;
    unsigned short int typePredicate;
	unsigned short int numPartitions;
	unsigned short int numBits;
	unsigned short int maxBits;
	unsigned short int traversingStrategy;
	unsigned short int numRuns;
    unsigned short int typeOptimizations;
    unsigned short int relevanceFunction;
    float theta;
    unsigned short int k;
    unsigned short int typeMetadata;
    unsigned short int typeBounds;
    unsigned int numLevels;
    
	
	void init()
	{
		verbose	           = false;
        traversingStrategy = TRAVERSING_STRATEGY_BOTTOMUP;
        numBits            = 0;
		numRuns            = 1;
        typeOptimizations  = 0;
        theta              = 0;
        k                  = 0;
        typeMetadata       = METADATA_NO;
        typeBounds         = BOUNDS_NO;
        numLevels          = 16;
	};
};


struct StabbingQuery
{
	size_t id;
	Timestamp point;
    
    StabbingQuery()
    {
        
    };
    StabbingQuery(size_t i, Timestamp p)
    {
        id = i;
        point = p;
    };
};

struct RangeQuery
{
	size_t id;
	Timestamp start, end;

    RangeQuery()
    {
        
    };
    RangeQuery(size_t i, Timestamp s, Timestamp e)
    {
        id = i;
        start = s;
        end = e;
    };
};



class Timer
{
private:
	using Clock = std::chrono::high_resolution_clock;
	Clock::time_point start_time, stop_time;
	
public:
	Timer()
	{
		start();
	}
	
	void start()
	{
		start_time = Clock::now();
	}
	
	
	double getElapsedTimeInSeconds()
	{
		return std::chrono::duration<double>(stop_time - start_time).count();
	}
	
	
	double stop()
	{
		stop_time = Clock::now();
		return getElapsedTimeInSeconds();
	}
};

class Relation;


struct Metadata
{
    Timestamp minStart;
    Timestamp maxStart;
    Timestamp minEnd;
    Timestamp maxEnd;
    
    Metadata()
    {
        minStart = numeric_limits<Timestamp>::max();
        maxStart = 0;
        minEnd   = numeric_limits<Timestamp>::max();
        maxEnd   = 0;
    };
    Metadata(Timestamp mins, Timestamp maxs, Timestamp mine, Timestamp maxe)
    {
        minStart = mins;
        maxStart = maxs;
        minEnd   = mine;
        maxEnd   = maxe;
    };
};


// Imports from utils
string toUpperCase(char *buf);
string toLowerCase(char *buf);
bool checkOptimizations(string strOptimizations, RunSettings &settings);
bool checkMetadata(string strMetadata, RunSettings &settings);
bool checkPredicate(string strPredicate, RunSettings &settings);
bool checkRelevanceFunction(string relevanceFunction, RunSettings &settings);
bool checkStrategy(string strStrategy, RunSettings &settings);
bool checkBounds(string strBounds, RunSettings &settings);
void process_mem_usage(double& vm_usage, double& resident_set);
unsigned int determineOptimalNumBitsForHINT_M(const Relation &R, const float qe_precentage);

////// Compute relevance
////float computeAbsoluteRelevance(const Record& r, const RangeQuery& q);
////float computeRelativeRelevance(const Record& r, const RangeQuery& q);
////float computeRecordRelativeRelevance(const Record& r, const RangeQuery& q);
////float computeQueryRelativeRelevance(const Record& r, const RangeQuery& q);
////
////float computeAbsoluteRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q);
////float computeRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q);
////float computeRecordRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q);
////float computeQueryRelativeRelevance(const pair<Timestamp, Timestamp>& r, const RangeQuery& q);
//
//// Compute relevance lower bound
//float computeAbsoluteRelevanceLowerBound(Metadata &md, const RangeQuery &q);
//float computeRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q);
//float computeRecordRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q);
//float computeQueryRelativeRelevanceLowerBound(Metadata &md, const RangeQuery &q);
//
//// Compute Relevance upper bound
//float computeAbsoluteRelevanceUpperBound(Metadata &md, const RangeQuery &q);
//float computeRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q);
//float computeRecordRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q);
//float computeQueryRelativeRelevanceUpperBound(Metadata &md, const RangeQuery &q);
//
//float computeAbsoluteRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeQueryRelativeRelevanceUpperBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//
//float computeAbsoluteRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceLowerBound_MinStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceLowerBound_MinStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceLowerBound_MaxStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceLowerBound_MaxStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceLowerBound_MinStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceLowerBound_MinStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceLowerBound_MaxStartMinEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceLowerBound_MaxStartMaxEnd(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeRecordRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//float computeQueryRelativeRelevanceLowerBound(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
//
//void updateTopK(priority_queue<float, vector<float>, greater<float>> &result, const unsigned int k, const float relevance);
//
////float computeUpperBoundAbsoluteRelevance_FirstLastOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_FirstLastOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_FirstOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_FirstOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_FirstRepsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_FirstRepsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_MiddleOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_MiddleOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_LastOrgsIn(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
////float computeUpperBoundAbsoluteRelevance_LastOrgsAft(unsigned int level, Timestamp t, Metadata **md, Timestamp qa, Timestamp qb);
#endif // _GLOBAL_DEF_H_
