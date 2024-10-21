CC      = g++
CFLAGS  = -O3 -mavx -std=c++14 -w
LDFLAGS =


SOURCES = utils.cpp containers/relation.cpp indices/hierarchicalindex.cpp indices/hint_m_subs+sort+cm.cpp indices/intervaltree.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: theta_h theta_i topk_h topk_i accuracy_h accuracy_i


theta_h: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/hierarchicalindex.o indices/hint_m_subs+sort+cm.o main_relevance-theta_hint.cpp -o query_relevance-theta_hint.exec $(LDADD)

theta_i: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/intervaltree.o main_relevance-theta_intervaltree.cpp -o query_relevance-theta_intervaltree.exec $(LDADD)

#theta_i_old: $(OBJECTS)
#	$(CC) $(CFLAGS) $(LDFLAGS) utils.o main_relevance-theta_intervaltree.old.cpp -o query_relevance-theta_intervaltree.old.exec $(LDADD)


topk_h: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/hierarchicalindex.o indices/hint_m_subs+sort+cm.o main_relevance-topk_hint.cpp -o query_relevance-topk_hint.exec $(LDADD)

topk_i: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/intervaltree.o  main_relevance-topk_intervaltree.cpp -o query_relevance-topk_intervaltree.exec $(LDADD)

#topk_i_old: $(OBJECTS)
#	$(CC) $(CFLAGS) $(LDFLAGS) utils.o main_relevance-topk_intervaltree.old.cpp -o query_relevance-topk_intervaltree.old.exec $(LDADD)


accuracy_h: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/hierarchicalindex.o indices/hint_m_subs+sort+cm.o main_accuracy_hint.cpp -o measure_accuracy_hint.exec $(LDADD)

accuracy_i: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) utils.o containers/relation.o indices/intervaltree.o main_accuracy_intervaltree.cpp -o measure_accuracy_intervaltree.exec $(LDADD)


.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf utils.o
	rm -rf containers/*.o
	rm -rf indices/*.o
	rm -rf query_relevance-theta_hint.exec
	rm -rf query_relevance-theta_intervaltree.exec
	rm -rf query_relevance-topk_hint.exec
	rm -rf query_relevance-topk_intervaltree.exec
	rm -rf measure_accuracy_hint.exec
	rm -rf measure_accuracy_intervaltree.exec
