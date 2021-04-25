#include "provided.h"
#include "Trie.h"
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
    int minLen;
    vector<Genome> m_gene;
    Trie<pair<int, int>> m_trie;
    //the first int represents this gene's location in m_gene
    //the second int represents the starting position of this gene
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    minLen=minSearchLength;
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    m_gene.push_back(genome);
    string fragment;
    for(int i=0; i<genome.length();i++){
        if(genome.extract(i, minLen, fragment)){
            pair<int, int> value(m_gene.size()-1,i);
            m_trie.insert(fragment, value);}
    }
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    return minLen;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    for(int i=0; i<matches.size();i++)
        matches.pop_back();
    if(minimumLength<minLen)
        return false;
    if(m_trie.find(fragment.substr(0,minLen), exactMatchOnly).empty())
        return false;
    vector<pair<int,int>> prefix;
    for(int i=0; i<m_trie.find(fragment.substr(0,minLen), exactMatchOnly).size();i++)
        prefix.push_back(m_trie.find(fragment.substr(0,minLen), exactMatchOnly)[i]);
    int length = 0;
    string thisOne;
    DNAMatch start;
    start.length=0;
    start.position=-1;
    //create a result vector with m_gene's size and default DNAMatch object
    vector<DNAMatch> result(m_gene.size(),start);
    bool exact=exactMatchOnly;
    for(int i=0; i<prefix.size();i++){
        for(int j=0;j<fragment.size();j++){
            m_gene[prefix[i].first].extract(prefix[i].second+j, 1, thisOne);
            if(thisOne==fragment.substr(j,1))
                length++;
            else if(!exact){
                length++;
                exact=true;}
            else
                break;}
        if(length>=minimumLength){
            //compare this matching segment with the one recorded in result
            if(length>result[prefix[i].first].length ||
               ((length==result[prefix[i].first].length) && prefix[i].second<result[prefix[i].first].position)){
                DNAMatch temp;
                temp.genomeName=m_gene[prefix[i].first].name();
                temp.position=prefix[i].second;
                temp.length=length;
                result[prefix[i].first]=temp;}}
        length=0;
        exact=exactMatchOnly;
    }
    for(int i=0; i<result.size(); i++){
        //push back all the modified DNAMatch objects
        if(result[i].length>0)
            matches.push_back(result[i]);}
    if(matches.empty())
        return false;
    return true;
}

bool customCompare(GenomeMatch first, GenomeMatch second){
    if(first.percentMatch>second.percentMatch)
        return true;
    if(first.percentMatch<second.percentMatch)
        return false;
    return (first.genomeName<second.genomeName);
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    for(int i=0; i<results.size();i++)
        results.pop_back();
    if(fragmentMatchLength<minLen)
        return false;
    vector<DNAMatch> temp;
    vector<DNAMatch> matches;
    string gene;
    int totalMatch=query.length()/fragmentMatchLength;
    for(int i=0;i<totalMatch;i++){
        query.extract(i*fragmentMatchLength, fragmentMatchLength, gene);
        //if can find this DNA in m_gene
        if(findGenomesWithThisDNA(gene, fragmentMatchLength, exactMatchOnly, temp)){
            for(int j=0; j<temp.size();j++)
                matches.push_back(temp[j]);}
    }
    int countMatch=0;
    for(int i=0; i<matches.size(); i++){
        //compare matches[i] with all other vectors in matches
        int j=i+1;
        countMatch++;
        while(j<matches.size()){
            //if they are from the same gene
            if(matches[i].genomeName==matches[j].genomeName){
                countMatch++;
                //delete this one to avoid repetitive counting
                matches[j]=matches[matches.size()-1];
                matches.pop_back();
            }
            else
                j++;
        }
        if((double)countMatch/totalMatch >= matchPercentThreshold*0.01){
            GenomeMatch m;
            m.genomeName=matches[i].genomeName;
            m.percentMatch=((double)countMatch/totalMatch)*100;
            results.push_back(m);
        }
        countMatch=0;
    }
    sort(results.begin(),results.end(),&customCompare);
    if(results.empty())
        return false;
    return true;
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
