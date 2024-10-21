#include "getopt.h"
#include "def_global.h"
#include "./containers/relation.h"
#include "./indices/intervaltree.h"
#include "utils_inlines.h"



void usage()
{
    cerr << endl;
    cerr << "PROJECT" << endl;
    cerr << "       Relevance queries for interval data" << endl;
    cerr << endl;
    cerr << "USAGE" << endl;
    cerr << "       ./measure_accuracy_intervaltree.exec [OPTION]... [DATA] [QUERIES]" << endl;
    cerr << endl;
    cerr << "DESCRIPTION" << endl;
    cerr << "       -? or -h" << endl;
    cerr << "              display this help message and exit" << endl;
    cerr << endl;
    cerr << "       -a min/max" << endl;
    cerr << "              set the type of min/max endpoint bounds: \"UO\" uninformed or \"IO\" informed; option always used in combination with '-b'" << endl;
    cerr << endl;
    cerr << "       -q predicate" << endl;
    cerr << "              set the predicate type: currently only \"GOVERLAPS\" is supported" << endl;
    cerr << "       -f function" << endl;
    cerr << "              set the relevance function:  \"ABSOLUTE\" or \"RELATIVE\" or \"DRELATIVE\" or \"QRELATIVE\"" << endl;
    cerr << endl;
    cerr << "EXAMPLES" << endl;
    cerr << "       ./measure_accuracy_intervaltree.exec -q gOVERLAPS -a IO -f RELATIVE inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt" << endl;
    cerr << endl;
}


int main(int argc, char **argv)
{
    Timer tim;
    Relation R;
    IntervalTree *idxR;
    size_t totalResult = 0, queryresult = 0, numQueries = 0;
    double totalIndexTime = 0, totalQueryTime = 0, querytime = 0, avgQueryTime = 0;
    Timestamp qstart, qend;
    RunSettings settings;
    char c;
    string strQuery = "", strPredicate = "GOVERLAPS", strRelevanceFunction = "", strMetadata = "", strBounds = "", strStrategy = "DEPTH-FIRST";
    bool useBounds = false;

    
    // Parse command line input
    settings.init();
    settings.method = "interval tree";
    while ((c = getopt(argc, argv, "?ha:q:f:v")) != -1)
    {
        switch (c)
        {
            case '?':
            case 'h':
                usage();
                return 0;
                
            case 'a':
                strMetadata = toUpperCase((char*)optarg);
                break;

            case 'q':
                strPredicate = toUpperCase((char*)optarg);
                break;

            case 'f':
                strRelevanceFunction = toUpperCase((char*)optarg);
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
    tim.start();
    R.sortByStart();
    idxR = new IntervalTree(R, settings.typeMetadata);
    totalIndexTime = tim.stop();


    // Execute queries
    while (fQ >> qstart >> qend)
    {
        switch (settings.relevanceFunction)
        {
            case RELEVANCE_FUNCTION_ABSOLUTE:
                idxR->executeDepthFirst_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeAbsoluteRelevance, computeAbsoluteRelevanceLowerBound, computeAbsoluteRelevanceUpperBound);
                break;

            case RELEVANCE_FUNCTION_RELATIVE:
                idxR->executeDepthFirst_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeRelativeRelevance, computeRelativeRelevanceLowerBound, computeRelativeRelevanceUpperBound);
                break;
                    
            case RELEVANCE_FUNCTION_DRELATIVE:
                idxR->executeDepthFirst_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeRecordRelativeRelevance, computeRecordRelativeRelevanceLowerBound, computeRecordRelativeRelevanceUpperBound);
                break;
                    
            case RELEVANCE_FUNCTION_QRELATIVE:
                idxR->executeDepthFirst_gOverlaps_MeasureAccuracy(RangeQuery(numQueries, qstart, qend), computeQueryRelativeRelevance, computeQueryRelativeRelevanceLowerBound, computeQueryRelativeRelevanceUpperBound);
                break;
        }
    }
    fQ.close();

    delete idxR;
    
    
    return 0;
}
