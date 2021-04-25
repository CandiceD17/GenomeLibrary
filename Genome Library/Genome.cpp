#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const string& nm, const string& sequence);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
private:
    string m_name;
    string m_seq;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
{
    m_name = nm;
    m_seq = sequence;
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) 
{
    for(int i=0; i<genomes.size(); i++)
        genomes.pop_back();
    string line;
    if(!getline(genomeSource,line))
        return false;
    if(line[0]!='>'||line.length()<=1)
        //if the first line does not start with '<'
        //or it only includes '<'
        return false;
    string name=line.substr(1);
    string sequence="";
    while(getline(genomeSource,line)){
        if(line.length()==0)
            return false;
        if(line[0]=='>'){
            if(sequence==""||line.length()<=1)
                //if the previous sequence is empty
                //or this line only includes '<'
                return false;
            genomes.push_back(Genome(name, sequence));
            name=line.substr(1);
            sequence="";
        }
        else{
            for(int i=0;i<line.size();i++){
                toupper(line[i]);
                if(line[i]!='A'&& line[i]!='T'&& line[i]!='C'&& line[i]!='G'&& line[i]!='N')
                    return false;}
            sequence+=line;
        }
    }
    if(name==""||sequence=="")
        return false;
    //push back the last genome
    genomes.push_back(Genome(name, sequence));
    return true;
}

int GenomeImpl::length() const
{
    return m_seq.length();
}

string GenomeImpl::name() const
{
    return m_name;
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
    if (position>=m_seq.size() || position+length-1>=m_seq.size())
       return false;
    fragment=m_seq.substr(position,length);
    return true;
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
