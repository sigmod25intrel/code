#include "getopt.h"
#include "def_global.h"
#include "./containers/relation.h"
#include "./indices/hint_m.h"



// externs
#ifdef STATS
extern size_t prunedIntervals[4];
extern size_t freeResults[4];
#endif


void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       Relevance queries for interval data" << endl;
    cerr << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./query_relevance-theta.exec [OPTION]... [DATA] [QUERIES]" << endl;
    cerr << endl;
    cerr << "DESCRIPTION" << endl;
    cerr << "       -? or -h" << endl;
    cerr << "              display this help message and exit" << endl;
    cerr << endl;
    cerr << "       -o optimizations" << endl;
    cerr << "              set the HINT^m optimizations: currently only \"SUBS+SORT+CM\" is supported" << endl;
    cerr << "       -m bits" << endl;
    cerr << "              set the number of bits for HINT^m; omit option to automatically set a value using the cost model in Christodoulou et al. @ ACM SIGMOD 2022" << endl;
    cerr << "       -a min/max" << endl;
    cerr << "              set the type of min/max endpoint bounds: \"UO\" uninformed or \"IO\" informed; option always used in combination with '-b'" << endl;
    cerr << endl;
    cerr << "       -q predicate" << endl;
    cerr << "              set the predicate type: currently only \"GOVERLAPS\" is supported" << endl;
    cerr << "       -f function" << endl;
    cerr << "              set the relevance function:  \"ABSOLUTE\" or \"RELATIVE\" or \"DRELATIVE\" or \"QRELATIVE\"" << endl;
    cerr << "       -t theta" << endl;
    cerr << "              set the relevance threshold: a value inside [0,1]; if set to 0 all overlapping intervals are returned" << endl;
    cerr << "       -s strategy" << endl;
    cerr << "              set the traversing strategy: currently only \"BOTTOM-UP\" is supported; omit option for default \"BOTTOM-UP\"" << endl;
    cerr << "       -b bounds" << endl;
    cerr << "              set the type of bounds: \"lower\", \"upper\" or \"both\"; omit option to deactivate bounds" << endl;
    cerr << "       -r runs" << endl;
    cerr << "              set the number of runs per query; omit option for default 1" << endl;
    cerr << "       -v" << endl;
    cerr << "              activate verbose mode; print the trace for every query; otherwise only the final report" << endl;
    cerr << endl;
    cerr << "EXAMPLES" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b LOWER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b LOWER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << endl;
}


int main(int argc, char **argv)
{
    Timer tim;
    Relation R;
    HierarchicalIndex *idxR;
    size_t totalResult = 0, queryresult = 0, numQueries = 0;
    double totalIndexTime = 0, totalQueryTime = 0, querytime = 0, avgQueryTime = 0;
    Timestamp qstart, qend;
    RunSettings settings;
    char c;
    string strQuery = "", strPredicate = "GOVERLAPS", strOptimizations = "SUBS+SORT+CM", strRelevanceFunction = "", strMetadata = "", strBounds = "", strStrategy = "BOTTOM-UP";
    bool useBounds = false;

    
    // Parse command line input
    settings.init();
    settings.method = "hint_m";
    while ((c = getopt(argc, argv, "?hm:o:a:q:f:t:s:b:r:v")) != -1)
    {
        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;
                
            case 'o':
                strOptimizations = toUpperCase((char*)optarg);
                break;
                
            case 'm':
                settings.numBits = atoi(optarg);
                break;
                
            case 'a':
                strMetadata = toUpperCase((char*)optarg);
                break;

            case 'q':
                strPredicate = toUpperCase((char*)optarg);
                break;

            case 'f':
                strRelevanceFunction = toUpperCase((char*)optarg);
                break;

            case 't':
                settings.theta = atof(optarg);
                break;
                
            case 's':
                strStrategy = toUpperCase((char*)optarg);
                break;

            case 'b':
                strBounds = toUpperCase((char*)optarg);
                break;

            case 'r':
                settings.numRuns = atoi(optarg);
                break;
                
            case 'v':
                settings.verbose = true;
                break;
                
            default:
                cerr << endl << "Error - unknown option '" << c << "'" << endl << endl;
                usage();
                return 1;
        }
    }
    
    
    // Sanity check
    if (argc-optind != 2)
    {
        usage();
        return 1;
    }
    if (!checkOptimizations(strOptimizations, settings))
    {
        cerr << endl << "Error - unknown optimizations type \"" << strOptimizations << "\"" << endl << endl;
        usage();
        return 1;
    }
    if (!checkMetadata(strMetadata, settings))
    {
        cerr << endl << "Error - unknown type of min/max bounds \"" << strMetadata << "\"" << endl << endl;
        usage();
        return 1;
    }
    if (!checkPredicate(strPredicate, settings))
    {
        if (strPredicate == "")
            cerr << endl << "Error - predicate type not defined" << endl << endl;
        else
            cerr << endl << "Error - unknown predicate type \"" << strPredicate << "\"" << endl << endl;
        usage();
        return 1;
    }
    if (!checkRelevanceFunction(strRelevanceFunction, settings))
    {
        if (strRelevanceFunction == "")
            cerr << endl << "Error - relevance function not defined" << endl << endl;
        else
            cerr << endl << "Error - unknown relevance function \"" << strRelevanceFunction << "\"" << endl << endl;
        usage();
        return 1;
    }
    if (!checkStrategy(strStrategy, settings))
    {
        cerr << endl << "Error - unknown traversing straetgy \"" << strStrategy << "\"" << endl << endl;
        usage();
        return 1;
    }
    if (!checkBounds(strBounds, settings))
    {
        cerr << endl << "Error - unknown type of bounds \"" << strBounds << "\"" << endl << endl;
        usage();
        return 1;
    }
    if ((settings.typeBounds != BOUNDS_NO) && (settings.typeMetadata == METADATA_NO))
    {
        cerr << endl << "Error - must set the type of metadata in order to use bounds" << endl << endl;
        usage();
        return 1;
    }
    if (settings.typeBounds == BOUNDS_NO)
        settings.typeMetadata = METADATA_NO;
    if ((settings.theta < 0) || (settings.theta > 1))
    {
        cerr << endl << "Error - relevance threshold must be a value inside [0,1] \"" << settings.theta << "\"" << endl << endl;
        usage();
        return 1;
    }
    settings.dataFile = argv[optind];
    settings.queryFile = argv[optind+1];
    
    // Load data and queries
    R.load(settings.dataFile);
    settings.maxBits = int(log2(R.gend-R.gstart)+1);
    
    ifstream fQ(settings.queryFile);
    if (!fQ)
    {
        cerr << endl << "Error - cannot open query file \"" << settings.queryFile << "\"" << endl << endl;
        return 1;
    }

    
    // Build index
    if (settings.numBits == 0)
        settings.numBits = determineOptimalNumBitsForHINT_M(R, 0.1);    // Use 0.1% of the domain as the query extent

    switch (settings.typeOptimizations)
    {
        // Base HINT^m, no optimizations activated
//        case HINT_M_OPTIMIZATIONS_NO:
//            tim.start();
//            idxR = new HINT_M(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;
//
//        // HINT^m with subs+sort optimization activated
//        case HINT_M_OPTIMIZATIONS_SUBS_SORT:
//            tim.start();
//            idxR = new HINT_M_SubsSort(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;
//
//        // HINT^m with subs+sopt optimization activated
//        case HINT_M_OPTIMIZATIONS_SUBS_SOPT:
//            tim.start();
//            idxR = new HINT_M_SubsSopt(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;
//
//        // HINT^m with subs+sort+sopt optimization activated
//        case HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT:
//            tim.start();
//            idxR = new HINT_M_SubsSortSopt(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;
//
//        // HINT^m with subs+sort+sopt and skewness & sparsity optimizations activated
//        case HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_SS:
//            tim.start();
//            idxR = new HINT_M_SubsSortSopt_SS(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;

        // HINT^m with subs+sort+sopt and cash misses optimizations activated
        case HINT_M_OPTIMIZATIONS_SUBS_SORT_CM:
            tim.start();
            idxR = new HINT_M_SubsSort_CM(R, settings.numBits, settings.maxBits, settings.typeMetadata);
            totalIndexTime = tim.stop();
            break;

//        // HINT^m with subs+sort+sopt and cash misses optimizations activated
//        case HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_CM:
//            tim.start();
//            idxR = new HINT_M_SubsSortSopt_CM(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;

        // HINT^m with subs+sort and cash misses optimizations activated, from VLDB Journal
//        case HINT_M_OPTIMIZATIONS_SUBS_SORT_SS_CM:
//            tim.start();
//            idxR = new HINT_M_SubsSort_SS_CM(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;

        // HINT^m with all optimizations activated, from ACM SIGMOD'22
//        case HINT_M_OPTIMIZATIONS_ALL:
//            tim.start();
//            idxR = new HINT_M_ALL(R, settings.numBits, settings.maxBits);
//            totalIndexTime = tim.stop();
//            break;
    }
    
//    if (settings.typeMetadata == METADATA_UNINFORMED_RESCALED_DOMAIN)
//        idxR->computeUninformedBoundsMetadata();
//    else 
    if (settings.typeMetadata == METADATA_UNINFORMED_ORIGINAL_DOMAIN)
        idxR->computeUninformedBoundsMetadataOriginalSpace();

    
    // Execute queries
#ifdef STATS
    memset(prunedIntervals, 0, sizeof(size_t));
    memset(freeResults, 0, sizeof(size_t));
#endif
    size_t sumQ = 0;
    if (settings.verbose)
        cout << "Query\tPredicate\tRelevance\tTheta\tMethod\tBits\tOptimizations\tMin/Max\tStrategy\tBounds\tResult\tTime" << endl;
    while (fQ >> qstart >> qend)
    {
        sumQ += qend-qstart;
        numQueries++;
        
        double sumT = 0;
        for (auto r = 0; r < settings.numRuns; r++)
        {
            switch (settings.relevanceFunction)
            {
                case RELEVANCE_FUNCTION_ABSOLUTE:
                    if (settings.typeBounds == BOUNDS_NO)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold(RangeQuery(numQueries, qstart, qend), int(settings.theta*(R.gend-R.gstart+1)));
//                        queryresult = idxR->executeBottomUp_gOverlaps_gOverlaps(RangeQuery(numQueries, qstart, qend));
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_LOWER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingLowerBounds(RangeQuery(numQueries, qstart, qend), int(settings.theta*(R.gend-R.gstart+1)));
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_UPPER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingUpperBounds(RangeQuery(numQueries, qstart, qend), int(settings.theta*(R.gend-R.gstart+1)));
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_BOTH)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_AbsoluteRelevanceThreshold_UsingBothBounds(RangeQuery(numQueries, qstart, qend), int(settings.theta*(R.gend-R.gstart+1)));
                        querytime = tim.stop();
                    }
                    break;
                    
                case RELEVANCE_FUNCTION_RELATIVE:
                    if (settings.typeBounds == BOUNDS_NO)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RelativeRelevanceThreshold(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_LOWER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingLowerBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_UPPER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingUpperBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_BOTH)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RelativeRelevanceThreshold_UsingBothBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    break;

                case RELEVANCE_FUNCTION_DRELATIVE:
                    if (settings.typeBounds == BOUNDS_NO)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_LOWER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingLowerBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_UPPER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingUpperBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_BOTH)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_RecordRelativeRelevanceThreshold_UsingBothBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    break;

                case RELEVANCE_FUNCTION_QRELATIVE:
                    if (settings.typeBounds == BOUNDS_NO)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_LOWER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingLowerBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_UPPER)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingUpperBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    else if (settings.typeBounds == BOUNDS_BOTH)
                    {
                        tim.start();
                        queryresult = idxR->executeBottomUp_gOverlaps_QueryRelativeRelevanceThreshold_UsingBothBounds(RangeQuery(numQueries, qstart, qend), settings.theta);
                        querytime = tim.stop();
                    }
                    break;
            }

            sumT += querytime;
            totalQueryTime += querytime;
            
            if (settings.verbose)
                cout << "[" << qstart << "," << qend << "]\t" << strPredicate << "\t" << strRelevanceFunction << "\t" << settings.theta << "\t" << settings.method << "\t" << settings.numBits << "\t" << strOptimizations << "\t" << ((settings.typeMetadata == METADATA_NO)? "No": strMetadata) << "\t" << toLowerCase((char*)strStrategy.c_str()) << "\t" << ((settings.typeMetadata == METADATA_NO)? "No": strBounds) << "\t" << queryresult << "\t" << querytime << endl;
        }
        totalResult += queryresult;
        avgQueryTime += sumT/settings.numRuns;
    }
    fQ.close();
    
    
    // Report
    idxR->getStats();
    cout << endl;
    cout << "HINT^m" << endl;
    cout << "======" << endl;
    cout << "Input" << endl;
    cout << "  Num of intervals            : " << R.size() << endl;
    cout << "  Domain size                 : " << (R.gend-R.gstart) << endl;
    cout << "  Avg interval extent [%]     : "; printf("%f\n", R.avgRecordExtent*100/(R.gend-R.gstart));
    cout << endl;
    cout << "Index" << endl;
    cout << "  Optimizations               : " << ((settings.typeOptimizations == HINT_M_OPTIMIZATIONS_NO)? "no": strOptimizations) << endl;
    cout << "  Num of bits                 : " << settings.numBits << endl;
    cout << "  Num of partitions           : " << idxR->numPartitions << endl;
    if (settings.typeOptimizations == HINT_M_OPTIMIZATIONS_NO)
    {
        cout << "  Num of Originals            : " << idxR->numOriginals << endl;
        cout << "  Num of replicas             : " << idxR->numReplicas << endl;
    }
    else
    {
        cout << "  Num of Originals (In)       : " << idxR->numOriginalsIn << endl;
        cout << "  Num of Originals (Aft)      : " << idxR->numOriginalsAft << endl;
        cout << "  Num of replicas (In)        : " << idxR->numReplicasIn << endl;
        cout << "  Num of replicas (Aft)       : " << idxR->numReplicasAft << endl;
    }
    cout << "  Num of empty partitions     : " << idxR->numEmptyPartitions << endl;
    printf( "  Avg partition size          : %f\n", idxR->avgPartitionSize);
    switch (settings.typeMetadata)
    {
        case METADATA_NO:
            strMetadata = "No";
            break;

//        case METADATA_UNINFORMED_RESCALED_DOMAIN:
//            strMetadata = "Uninformed on rescaled domain";
//            break;
//
        case METADATA_UNINFORMED_ORIGINAL_DOMAIN:
            strMetadata = "Uninformed";
            break;

        case METADATA_INFORMED_ORIGINAL_DOMAIN:
            strMetadata = "Informed";
            break;
    }
    cout << "  Min/Max bounds type         : " << strMetadata << endl;
    printf( "  Size [Bytes]                : %ld\n", idxR->getSize());
    printf( "  Indexing time [secs]        : %f\n", totalIndexTime);
    cout << endl;
    cout << "Queries" << endl;
    cout << "  Predicate type              : " << strPredicate << endl;
    cout << "  Traversing strategy         : " << toLowerCase((char*)strStrategy.c_str()) << endl;
    cout << "  Num of queries              : " << numQueries << endl;
    cout << "  Avg query extent [%]        : "; printf("%f\n", (((float)sumQ/numQueries)*100)/(R.gend-R.gstart));
    cout << "  Relevance function          : " << strRelevanceFunction << endl;
    cout << "  Relevance threshold         : ";
    if (settings.relevanceFunction == RELEVANCE_FUNCTION_ABSOLUTE)
        printf("%f * DOMAIN_EXTENT = %ld\n", settings.theta, int(settings.theta*(R.gend-R.gstart)));
    else
        cout << 100*settings.theta << "%" << endl;
    switch (settings.typeBounds)
    {
        case BOUNDS_NO:
            strBounds = "No";
            break;

        case BOUNDS_LOWER:
            strBounds = "Lower";
            break;

        case BOUNDS_UPPER:
            strBounds = "Upper";
            break;

        case BOUNDS_BOTH:
            strBounds = "Lower and upper";
            break;
    }
    cout << "  Bounds type                 : " << strBounds << endl;
    cout << "  Num of runs per query       : " << settings.numRuns << endl;
    cout << "  Size threshold              : " << SIZE_THRESHOLD << endl;
    cout << endl;
    cout << "Output" << endl;
    cout << "  Total result [";
#ifdef WORKLOAD_COUNT
    cout << "COUNT]        : ";
#else
    cout << "XOR]          : ";
#endif
    cout << totalResult << endl;
#ifdef STATS
    cout << "  Total free results (all)    : " << (freeResults[0]+freeResults[1]+freeResults[2]+freeResults[3]) << endl;
    cout << "    Originals (In)            : " << freeResults[0] << endl;
    cout << "    Originals (Aft)           : " << freeResults[1] << endl;
    cout << "    Replicas (In)             : " << freeResults[2] << endl;
    cout << "    Replicas (Aft)            : " << freeResults[3] << endl;
    cout << "  Total pruned intervals (all): " << (prunedIntervals[0]+prunedIntervals[1]+prunedIntervals[2]+prunedIntervals[3]) << endl;
    cout << "    Originals (In)            : " << prunedIntervals[0] << endl;
    cout << "    Originals (Aft)           : " << prunedIntervals[1] << endl;
    cout << "    Replicas (In)             : " << prunedIntervals[2] << endl;
    cout << "    Replicas (Aft)            : " << prunedIntervals[3] << endl;
#endif
    printf( "  Total querying time [secs]  : %f\n", totalQueryTime/settings.numRuns);
    printf( "  Avg querying time [secs]    : %f\n", avgQueryTime/numQueries);
    printf( "  Throughput [queries/sec]    : %f\n\n", numQueries/(totalQueryTime/settings.numRuns));

    delete idxR;
    
    
    return 0;
}
