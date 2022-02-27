//
//  main.cpp
//  DataMiningHW1
//
//  Created by jeffrey_MAC on 2016/10/14.
//  Copyright © 2016年 jeffrey_MAC. All rights reserved.
//
// Implementation Plan :
// Need to performance list intersections on transaction id list of each item, in order to get the item set ids
//
// From WIKI :
// Eclat[9] (alt. ECLAT, stands for Equivalence Class Transformation) is a depth-first search algorithm using set intersection. It is a naturally elegant algorithm suitable for both sequential as well as parallel execution with locality enhancing properties. It was first introduced by Zaki, Parthasarathy, Li and Ogihara in a series of papers written in 1997.


#include <vector>
#include <set>
#include <queue>
#include <map>
#include <utility>


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <sys/time.h>

using namespace std;


typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}


//Global Parameters
map<int, set<int>> verticle_tid_sets;
map<vector<int>, set<int>> frequent_item_sets;
int support_count = 0 ;

//Functions
template<typename T>
set<T> setIntersection(set<T> A, set<T> B) {
    set<T> C;
    set_intersection(A.begin(), A.end(), B.begin(), B.end(), insert_iterator<set<T>>(C,C.begin()));
    return C;
}

template<typename T>
vector<T> vectorInstersection(vector<T> &v1, vector<T> &v2)
{
    
    vector<T> v3;
    set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
    return v3;
}


template<typename T, int size>
int len(T(&)[size]){
    return size;
}
template<typename T,int size>
vector<T> array2vector(T(&A)[size]){
    vector<T> v(A, A + sizeof(A)/sizeof(A[0]));
    return v;
}

vector<bool> intersectBooleanLists(vector<bool> A, vector<bool> B) {
    std::vector<bool> intersection;
    int n1 = (int) A.size();
    for (int i = 0; i<n1; i++) {
        intersection.push_back(A[i]&B[i]);
    }
    return intersection;
}

void enumerateItemSets(vector<int> focused_sets,queue<int> unfocused_sets){
    // there are 2^n - 1 lattices for the support sets with size n
    while (unfocused_sets.size()!=0) {
        vector<int> tmp_focused_sets = focused_sets;
        int combined_item = unfocused_sets.front();
        tmp_focused_sets.push_back(combined_item);
        unfocused_sets.pop();
        set<int> intersect_tid_sets;
        if (tmp_focused_sets.size()==1) {
            intersect_tid_sets = verticle_tid_sets[combined_item];
            
        }else{
            intersect_tid_sets = setIntersection(frequent_item_sets[focused_sets],verticle_tid_sets[combined_item]);
        }
        //printf("support = %d",(int) intersect_tid_sets.size());
        if (intersect_tid_sets.size()>=support_count) {
            pair<vector<int>,set<int>> item_set_tid_set_pair(tmp_focused_sets,intersect_tid_sets);
            frequent_item_sets.insert(item_set_tid_set_pair);
            enumerateItemSets(tmp_focused_sets,unfocused_sets);
        }
    }
}


int main(int argc, const char * argv[]){
    
    
    
    // Reading in all transaction from the dataset
    ifstream file("/Users/jeffrey_mac/Desktop/HW1/data.txt");
    string   line;
    int user_id=0;
    string time_id;
    map<pair<int,string>,int> transaction_tids;
    int location_id;
    int data_line_count = 0;
    int tid = 0;
    int focused_tid = 0;
    cout<<"Start Reading from Dataset ... "<<endl;
    
    //Start Timer
    timestamp_t t0 = get_timestamp();
    
    if(file.is_open()){
        while(!file.eof()){
            // input the transaction information : user_id + time_id
            getline(file,line,',');
            if(line.size()==0) break;
            user_id = stoi(line);
            getline(file,line,',');
            time_id = line;
            pair<int,string> transaction(user_id,time_id);
            // getting tid for every transaction information
            if(transaction_tids.find(transaction)==transaction_tids.end()){
                //the transaction is not in the transactions tid set
                tid++;//increase the tid number
                focused_tid = tid;
                //adding another transaction into the set
                pair<pair<int,string>,int> transaction_key_id_pair(transaction,tid);
                transaction_tids.insert(transaction_key_id_pair);
            }else{
                //the transaction is in the transactions tid set
                focused_tid = transaction_tids[transaction];
            }
            getline(file,line,'\n');
            location_id = stoi(line);
            if(verticle_tid_sets.find(location_id)==verticle_tid_sets.end()){
                //we cannot find the item in the keys of verticl tid sets , create a new verticle tid set
                set<int> verticle_tid_set;
                verticle_tid_set.insert(focused_tid);
                pair<int,set<int>> item_id_transaction_set_pair(location_id,verticle_tid_set);
                verticle_tid_sets.insert(item_id_transaction_set_pair);
            }else{
                //we can find the item in the keys of verticl tid sets
                verticle_tid_sets[location_id].insert(focused_tid);
            }
            data_line_count++;
        }
    }
    file.close();
    
    //Times up
    timestamp_t t1 = get_timestamp();
    cout<<" - Data Loading Time : "<<(t1 - t0) / 1000000.0L<<endl;
    
    cout<<"Some Simple Statistics : "<<data_line_count<<endl;
    cout<<" - number of data line : "<<data_line_count<<endl;
    cout<<" - number of transactions : "<<transaction_tids.size()<<endl;
    cout<<" - number of tids : "<<tid<<endl;
    cout<<" - number of location ids : "<<verticle_tid_sets.size()<<endl;
    float support_percentage = 0.3;
    support_count = (int) (((float) transaction_tids.size())*support_percentage*0.01);
    cout<<"Minimum Support Count : "<<support_count<<endl;
    
    // Intersecting Two Verticle Transaction ID Set
    //t0 = get_timestamp();
    //vector<int> v1,v2;
    //no need to sort since they are already converted in order
    //v1.assign(verticle_tid_sets[0].begin(),verticle_tid_sets[0].end());
    //v2.assign(verticle_tid_sets[1].begin(),verticle_tid_sets[1].end());
    //vector<int> v3 = vectorInstersection(v1,v2);
    //set<int> intersection = setIntersection(verticle_tid_sets[0], verticle_tid_sets[1]);
    //t1 = get_timestamp();
    //cout<<"TID Set Intersecting Time : "<<(t1 - t0) / 1000000.0L<<endl;
    
    // Generating Items Sets Queue
    t0 = get_timestamp();
    vector<int> empty_v;
    queue<int> items_set;
    for (auto item_tid_sets_pair:verticle_tid_sets) {
        items_set.push(item_tid_sets_pair.first);
    }
    cout<<"Finish Generating Items Sets Queue"<<endl;
    enumerateItemSets(empty_v, items_set);
    t1 = get_timestamp();
    
    cout<<"Finish Frequent Item Sets Mining"<<endl;
    cout<<" - Total Mining Time : "<<(t1 - t0)/1000000.0L<<endl;
    
    // Convert the pairs of frequect items sets with corresponding verticle tid sets to frequent items with support count
    vector<pair<int,vector<int>>> frequent_item_set_vector;
    for(auto frequent_item_set:frequent_item_sets){
        pair<int, vector<int>> p(frequent_item_set.second.size(),frequent_item_set.first); //support count , item set
        frequent_item_set_vector.push_back(p);
    }
    // Sorting the frequent item sets by their support count
    t0 = get_timestamp();
    sort(frequent_item_set_vector.begin(),frequent_item_set_vector.end());
    t1 = get_timestamp();
    cout<<" Mining Result Sorting Time : "<<(t1 - t0)/1000000.0L<<endl;
    
    // Printing the frequet item sets with decreasing support count
    for (int j=(int) frequent_item_set_vector.size()-1;j>=0;j--){
        cout<<"<{";
        for (int i = 0; i<frequent_item_set_vector[j].second.size(); i++) {
            if (i!=frequent_item_set_vector[j].second.size()-1) {
                cout<<frequent_item_set_vector[j].second[i]<<" ";
            }else{
                cout<<frequent_item_set_vector[j].second[i];
            }
        }
        cout<<"},"<<frequent_item_set_vector[j].first<<">"<<endl;
    }
    
    return 0;
}
