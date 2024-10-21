bash run_real_relevance_topk_vary-qe.sh $1 HINT $2 $3
bash run_real_relevance_topk_vary-qe.sh $1 HINT $2 UO UPPER $3
bash run_real_relevance_topk_vary-qe.sh $1 HINT $2 UO BOTH $3
bash run_real_relevance_topk_vary-qe.sh $1 HINT $2 IO UPPER $3
bash run_real_relevance_topk_vary-qe.sh $1 HINT $2 IO BOTH $3

bash run_real_relevance_topk_vary-k.sh $1 HINT $2 $3
bash run_real_relevance_topk_vary-k.sh $1 HINT $2 UO UPPER $3
bash run_real_relevance_topk_vary-k.sh $1 HINT $2 UO BOTH $3
bash run_real_relevance_topk_vary-k.sh $1 HINT $2 IO UPPER $3
bash run_real_relevance_topk_vary-k.sh $1 HINT $2 IO BOTH $3
#exit

bash run_real_relevance_topk_vary-qe.sh $1 INTERVALTREE $2 $3
bash run_real_relevance_topk_vary-qe.sh $1 INTERVALTREE $2 IO UPPER $3
bash run_real_relevance_topk_vary-qe.sh $1 INTERVALTREE $2 IO BOTH $3

bash run_real_relevance_topk_vary-k.sh $1 INTERVALTREE $2 $3
bash run_real_relevance_topk_vary-k.sh $1 INTERVALTREE $2 IO UPPER $3
bash run_real_relevance_topk_vary-k.sh $1 INTERVALTREE $2 IO BOTH $3
