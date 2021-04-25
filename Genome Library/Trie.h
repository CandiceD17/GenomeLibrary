#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
using namespace std;

template<typename ValueType>
class Trie
{
public:
    Trie() {root=new Node;}
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;
    
    // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
    struct Node{
    public:
        std::vector<ValueType> m_value;
        std::vector<Node*> m_ptr;
        std::vector<char> label;
    };
    Node* root;
    void findMatch(const std::string& key, Node* cur, bool exactMatchOnly, std::vector<ValueType>& result) const;
    void helpReset(Node* cur);
};

#endif // TRIE_INCLUDED

template<typename ValueType>
void Trie<ValueType>::helpReset(Node* cur){
    //if the node is a leaf, delete it
    if(cur->m_ptr.empty()){
        Node* temp = cur;
        delete temp;
        return;
    }
    //traverse node's children
    for(int i=0;i<cur->m_ptr.size();i++){
        helpReset(cur->m_ptr[i]);
    }
    delete cur;
}

template<typename ValueType>
void Trie<ValueType>::reset(){
    helpReset(root);
    root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie(){
    helpReset(root);
}

template<typename ValueType>
void Trie<ValueType>::insert(const std::string& key, const ValueType& value){
    Node* cur = root;
    bool toNext = false;
    for(int i=0; i<=key.size();i++){
        //if the node is a leaf,
        //create a child with key[i]'s label
        if(cur->m_ptr.empty()){
            cur->m_ptr.push_back(new Node);
            cur->label.push_back(key[i]);
            cur = cur->m_ptr[0];
            toNext = true;
        }
        else{
        //traverse the children to find if any matches with key[i]
            for(int j=0;j<cur->label.size();j++){
                if(cur->label[j]==key[i]){
                    cur=cur->m_ptr[j];
                    toNext = true;
                    break;}}
        }
        //if no matching children for key[i], create one
        if(!toNext){
            cur->m_ptr.push_back(new Node);
            cur->label.push_back(key[i]);
            cur = cur->m_ptr[cur->m_ptr.size()-1];
        }
        //if go through all chars in key, push back the value
        if(i==key.size()-1){
            cur->m_value.push_back(value);
        }
        toNext=false;
    }
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const{
    std::vector<ValueType> result;
    //if no child, return an empty vector
    if(root->m_ptr.empty())
        return result;
    for(int i=0; i<root->m_ptr.size(); i++){
        //the the first character in key matches with any label,
        //traverse the following characters
        //else, return an empty vector
        if(key[0]==root->label[i])
            findMatch(key.substr(1), root->m_ptr[i], exactMatchOnly, result);
    }
    return result;
}

template<typename ValueType>
void Trie<ValueType>::findMatch(const std::string& key, Node* cur, bool exactMatchOnly, std::vector<ValueType>& result) const{
    if(cur->m_ptr.empty())
        return;
    if(key.length()==0){
        //if reaches the end of key, record this value
        for(int i=0; i<cur->m_value.size(); i++)
            result.push_back(cur->m_value[i]);
        return;
    }
    for(int i=0; i<cur->m_ptr.size(); i++){
        if(key[0]==cur->label[i])
            findMatch(key.substr(1), cur->m_ptr[i], exactMatchOnly, result);
        //if no matching characters & not an exact match
        //ignore this mismatch and do not allow any mismatch after this
        else if(!exactMatchOnly)
            findMatch(key.substr(1), cur->m_ptr[i], !exactMatchOnly, result);
    }
}

