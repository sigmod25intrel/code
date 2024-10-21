#include "getopt.h"
#include "def_global.h"
#include "./containers/relation.h"
#include "./indices/hint_m.h"
#include "utils_inlines.h"



void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       Relevance queries for interval data" << endl;
    cerr << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./measure_accuracy_hint.exec [OPTION]... [DATA] [QUERIES]" << endl;
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
    cerr << endl;
    cerr << "EXAMPLES" << endl;
    cerr << "       ./measure_accuracy_hint.exec-q gOVERLAPS -b UO -f RELATIVE inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << "       ./measure_accuracy_hint.exec-q gOVERLAPS -b IO -f RELATIVE inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
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
    while ((c = getopt(argc, argv, "?hm:o:a:q:f:v")) != -1)
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
        cerr << endl << "Error - unknown type of metadata \"" << strMetadata << "\"" << endl << endl;
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

    while (fQ >> qstart >> qend)
    {
        switch (settings.relevanceFunction)
        {
            case RELEVANCE_FUNCTION_ABSOLUTE:
                idxR->executeBottomUp_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound);
                break;
                
            case RELEVANCE_FUNCTION_RELATIVE:
                idxR->executeBottomUp_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound);
                break;
                
            case RELEVANCE_FUNCTION_DRELATIVE:
                idxR->executeBottomUp_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound);
                break;
                
            case RELEVANCE_FUNCTION_QRELATIVE:
                idxR->executeBottomUp_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound);
                break;
        }
    }
    fQ.close();
    
    delete idxR;
    
    
    return 0;
}
