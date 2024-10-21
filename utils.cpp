#include "def_global.h"
#include "containers/relation.h"



string toUpperCase(char *buf)
{
    auto i = 0;
    while (buf[i])
    {
        buf[i] = toupper(buf[i]);
        i++;
    }
    
    return string(buf);
}

string toLowerCase(char *buf)
{
    auto i = 0;
    while (buf[i])
    {
        buf[i] = tolower(buf[i]);
        i++;
    }
    
    return string(buf);
}


bool checkOptimizations(string strOptimizations, RunSettings &settings)
{
    if (strOptimizations == "")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_NO;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT;
        return true;
    }
    else if (strOptimizations == "SUBS+SOPT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SOPT;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT+SS")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_SS;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+CM")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_CM;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SOPT+CM")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SOPT_CM;
        return true;
    }
    else if (strOptimizations == "SUBS+SORT+SS+CM")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_SUBS_SORT_SS_CM;
        return true;
    }
    else if (strOptimizations == "ALL")
    {
        settings.typeOptimizations = HINT_M_OPTIMIZATIONS_ALL;
        return true;
    }
    else if (strOptimizations == "SS")
    {
        settings.typeOptimizations = HINT_OPTIMIZATIONS_SS;
        return true;
    }


    return false;
}


bool checkMetadata(string strMetadata, RunSettings &settings)
{
    if (strMetadata == "")
    {
        settings.typeMetadata = METADATA_NO;
        return true;
    }
//    else if (strMetadata == "UR")
//    {
//        settings.typeMetadata = METADATA_UNINFORMED_RESCALED_DOMAIN;
//        return true;
//    }
    else if (strMetadata == "UO")
    {
        settings.typeMetadata = METADATA_UNINFORMED_ORIGINAL_DOMAIN;
        return true;
    }
    else if (strMetadata == "IO")
    {
        settings.typeMetadata = METADATA_INFORMED_ORIGINAL_DOMAIN;
        return true;
    }

    return false;
}


bool checkPredicate(string strPredicate, RunSettings &settings)
{
//    if (strPredicate == "EQUALS")
//    {
//        settings.typePredicate = PREDICATE_EQUALS;
//        return true;
//    }
//    else if (strPredicate == "STARTS")
//    {
//        settings.typePredicate = PREDICATE_STARTS;
//        return true;
//    }
//    else if (strPredicate == "STARTED")
//    {
//        settings.typePredicate = PREDICATE_STARTED;
//        return true;
//    }
//    else if (strPredicate == "FINISHES")
//    {
//        settings.typePredicate = PREDICATE_FINISHES;
//        return true;
//    }
//    else if (strPredicate == "FINISHED")
//    {
//        settings.typePredicate = PREDICATE_FINISHED;
//        return true;
//    }
//    else if (strPredicate == "MEETS")
//    {
//        settings.typePredicate = PREDICATE_MEETS;
//        return true;
//    }
//    else if (strPredicate == "MET")
//    {
//        settings.typePredicate = PREDICATE_MET;
//        return true;
//    }
//    else if (strPredicate == "OVERLAPS")
//    {
//        settings.typePredicate = PREDICATE_OVERLAPS;
//        return true;
//    }
//    else if (strPredicate == "OVERLAPPED")
//    {
//        settings.typePredicate = PREDICATE_OVERLAPPED;
//        return true;
//    }
//    else if (strPredicate == "CONTAINS")
//    {
//        settings.typePredicate = PREDICATE_CONTAINS;
//        return true;
//    }
//    else if (strPredicate == "CONTAINED")
//    {
//        settings.typePredicate = PREDICATE_CONTAINED;
//        return true;
//    }
//    else if (strPredicate == "BEFORE")
//    {
//        settings.typePredicate = PREDICATE_PRECEDES;
//        return true;
//    }
//    else if (strPredicate == "AFTER")
//    {
//        settings.typePredicate = PREDICATE_PRECEDED;
//        return true;
//    }
//    else
    if (strPredicate == "GOVERLAPS")
    {
        settings.typePredicate = PREDICATE_GOVERLAPS;
        return true;
    }

    return false;
}


bool checkRelevanceFunction(string strRelevanceFunction, RunSettings &settings)
{
    if (strRelevanceFunction == "ABSOLUTE")
    {
        settings.relevanceFunction = RELEVANCE_FUNCTION_ABSOLUTE;
        return true;
    }
    else if (strRelevanceFunction == "RELATIVE")
    {
        settings.relevanceFunction = RELEVANCE_FUNCTION_RELATIVE;
        return true;
    }
    else if ((strRelevanceFunction == "Q-RELATIVE") || (strRelevanceFunction == "QRELATIVE"))
    {
        settings.relevanceFunction = RELEVANCE_FUNCTION_QRELATIVE;
        return true;
    }
    else if ((strRelevanceFunction == "D-RELATIVE") || (strRelevanceFunction == "DRELATIVE"))
    {
        settings.relevanceFunction = RELEVANCE_FUNCTION_DRELATIVE;
        return true;
    }

    return false;
}

bool checkStrategy(string strStrategy, RunSettings &settings)
{
    if ((strStrategy == "DEPTH-FIRST") || (strStrategy == "DEPTHFIRST"))
    {
        settings.traversingStrategy = TRAVERSING_STRATEGY_DEPTHFIRST;
        return true;
    }
    else if ((strStrategy == "TOP-DOWN") || (strStrategy == "TOPDOWN"))
    {
        settings.traversingStrategy = TRAVERSING_STRATEGY_TOPDOWN;
        return true;
    }
    else if ((strStrategy == "BOTTOM-UP") || (strStrategy == "BOTTOMUP"))
    {
        settings.traversingStrategy = TRAVERSING_STRATEGY_BOTTOMUP;
        return true;
    }
    else if ((strStrategy == "BEST-FIRST") || (strStrategy == "BESTFIRST"))
    {
        settings.traversingStrategy = TRAVERSING_STRATEGY_BESTFIRST;
        return true;
    }

    return false;
}

bool checkBounds(string strBounds, RunSettings &settings)
{
    if (strBounds == "")
    {
        settings.typeBounds = BOUNDS_NO;
        return true;
    }
    else if (strBounds == "LOWER")
    {
        settings.typeBounds = BOUNDS_LOWER;
        return true;
    }
    else if (strBounds == "UPPER")
    {
        settings.typeBounds = BOUNDS_UPPER;
        return true;
    }
    else if (strBounds == "BOTH")
    {
        settings.typeBounds = BOUNDS_BOTH;
        return true;
    }

    return false;
}

void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;
    
    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
        >> ignore >> ignore >> vsize >> rss;
    }
    
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}


#define C_cmp(n,m,b)    (b*n)/pow(2,m)
#define C_acc(n,q,m,b)    b*(q - (2*n)/pow(2,m))
unsigned int determineOptimalNumBitsForHINT_M(const Relation &R, const float qe_precentage)
{
    const double beta_cmp = 0.0000025245;
    const double beta_acc = 0.0000006846;
    const float threshold = 0.03;
    size_t n = R.size();
    const Timestamp Lambda = R.gend-R.gstart;
    const double lambda_s = R.avgRecordExtent;
    const double lambda_q = (Lambda*qe_precentage/100);
    const double Q = n * (lambda_s + lambda_q) / Lambda;
    unsigned int m = 1, maxBits = int(log2(Lambda)+1);

    // Compute total cost for m_max
    const double C_min = C_cmp(n, maxBits, beta_cmp) + C_acc(n, Q, maxBits, beta_acc);
    
//    printf("C_cmp(%d) =\t%.10lf\n", maxBits, C_cmp(n, maxBits, beta_cmp));
//    printf("C_acc(%d) =\t%.10lf\n", maxBits, C_acc(n, Q, maxBits, beta_acc));
//    printf("C(%d) = C_cmp(%d) + C_acc(%d) =\t%.10lf\n", maxBits, maxBits, maxBits, C_min);
//    cout << endl;
    
    for (m = 1; m < maxBits; m++)
    {
        // Compute total cost for m
        double C = C_cmp(n, m, beta_cmp) + C_acc(n, Q, m, beta_acc);
    
//        printf("C_cmp(%d) =\t%.10lf\n", m, C_cmp(n, m, beta_cmp));
//        printf("C_acc(%d) =\t%.10lf\n", m, C_acc(n, Q, m, beta_acc));
//        printf("C(%d) = C_cmp(%d) + C_acc(%d) =\t%.10lf\t%f\n", m, m, m, C, (C-C_min)/C_min);
        if ((C > 0) && ((C-C_min)/C_min <= threshold))
            break;
    }
        

    return m;
}
