#!/bin/bash

if [ "$#" -lt 4 ] || [ "$#" -eq 5 ]
then
        echo
        echo "Usage: $0 DATASET_PREFIX INDEX RELEVANCE_FUNCTION [METADATA] [BOUNDS] NUM_RUNS"
        echo
        exit
fi

ifile="../inputs/$1.txt"
th=0.5
#th=0.01
#th=0.001

#for qe in 0.1 0.5 1 5 10
for qe in 0.01 0.05 0.1 0.5 1
do
        qfile="../queries/$1_qe${qe}%_qn10000.txt"
        
        if [ $2 = "hint" ] || [ $2 = "HINT" ]
        then
            if [ "$#" -eq 4 ]
            then
                ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_th${th}_f${3}.txt"
                echo "../query_relevance-theta_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -t $th -f $3 -r ${!#} $ifile $qfile > $ofile"
                ../query_relevance-theta_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -t $th -f $3 -r ${!#} $ifile $qfile > $ofile
            else
                ofile="../outputs/$1_qe${qe}%_qn10000_HINT_qGOVERLAPS_th${th}_f${3}_a${4}_b${5}.txt"
                echo "../query_relevance-theta_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -t $th -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                ../query_relevance-theta_hint.exec -q GOVERLAPS -o SUBS+SORT+CM -t $th -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
            fi
            echo
        elif [ $2 = "itree" ] || [ $2 = "ITREE" ] || [ $2 = "intervaltree" ] || [ $2 = "INTERVALTREE" ]
        then
            if [ "$#" -eq 4 ]
            then
                ofile="../outputs/$1_qe${qe}%_qn10000_INTERVALTREE_qGOVERLAPS_th${th}_f${3}.txt"
                echo "../query_relevance-theta_intervaltree.exec -q GOVERLAPS -t $th -f $3 -r ${!#} $ifile $qfile > $ofile"
                ../query_relevance-theta_intervaltree.exec -q GOVERLAPS -t $th -f $3 -r ${!#} $ifile $qfile > $ofile
            else
                ofile="../outputs/$1_qe${qe}%_qn10000_INTERVALTREE_qGOVERLAPS_th${th}_f${3}_a${4}_b${5}.txt"
                echo "../query_relevance-theta_intervaltree.exec -q GOVERLAPS -t $th -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile"
                ../query_relevance-theta_intervaltree.exec -q GOVERLAPS -t $th -f $3 -r ${!#} -a $4 -b $5 $ifile $qfile > $ofile
            fi
            echo
        fi
done
