#!/bin/bash

if [ "$#" -lt 4 ] || [ "$#" -eq 5 ]
then
        echo
        echo "Usage: $0 DATASET_PREFIX INDEX RELEVANCE_FUNCTION [METADATA] [BOUNDS] NUM_RUNS"
        echo
        exit
fi

ifile="../inputs/$1_Sven.txt"
k=10

#for qe in 0.1 0.5 1 5 10
for qe in 0.01 0.05 0.1 0.5 1
do
        qfile="../queries/$1_qe${qe}%_qn10000.txt"
        
        if [ $2 = "hint" ] || [ $2 = "HINT" ]
        then
            if [ "$#" -eq 4 ]
            then                            
#                ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_sTOP-DOWN_k${k}_f${3}.txt"
#                echo "../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s TOP-DOWN -k $k -f $3 -r ${!#} $ifile $qfile > $ofile"
#                ../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s TOP-DOWN -k $k -f $3 -r ${!#} $ifile $qfile > $ofile

                ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_sBOTTOM-UP_k${k}_f${3}.txt"
                echo "../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BOTTOM-UP -k $k -f $3 -r ${!#} $ifile $qfile > $ofile"
                ../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BOTTOM-UP -k $k -f $3 -r ${!#} $ifile $qfile > $ofile
            else
                if [ $5 = "upper" ] || [ $5 = "UPPER" ]
                then
                    ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_sBOTTOM-UP_k${k}_f${3}_a${4}_b${5}.txt"
                    echo "../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BOTTOM-UP -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                    ../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BOTTOM-UP -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
                fi

                ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_sBEST-FIRST_k${k}_f${3}_a${4}_b${5}.txt"
                echo "../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BEST-FIRST -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                ../query_relevance-topk_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -s BEST-FIRST -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
            fi
            echo
        elif [ $2 = "itree" ] || [ $2 = "ITREE" ] || [ $2 = "intervaltree" ] || [ $2 = "INTERVALTREE" ]
        then
            if [ "$#" -eq 4 ]
            then
                ofile="../outputs/$1_qe${qe}%_qn10000_INTERVALTREE_qGOVERLAPS_sTOP-DOWN_k${k}_f${3}.txt"
                echo "../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s TOP-DOWN -k $k -f $3 -r ${!#} $ifile $qfile > $ofile"
                ../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s TOP-DOWN -k $k -f $3 -r ${!#} $ifile $qfile > $ofile
            else
                if [ $5 = "upper" ] || [ $5 = "UPPER" ]
                then
                    ofile="../outputs/$1_qe${qe}%_qn10000_INTERVALTREE_qGOVERLAPS_sTOP-DOWN_k${k}_f${3}_a${4}_b${5}.txt"
                    echo "../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s TOP-DOWN -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                    ../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s TOP-DOWN -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
                fi

                ofile="../outputs/$1_qe${qe}%_qn10000_INTERVALTREE_qGOVERLAPS_sBEST-FIRST_k${k}_f${3}_a${4}_b${5}.txt"
                echo "../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s BEST-FIRST -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                ../query_relevance-topk_intervaltree.exec -q GOVERLAPS -s BEST-FIRST -k $k -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
            fi
            echo
        fi
done
