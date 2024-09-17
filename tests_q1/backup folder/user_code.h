#ifndef USER_CODE_H
#define USER_CODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>
#include "fileIterator.h"
#include "fileWriter.h"

using namespace std;



//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices 
 */

// Custom hash function for vector<string>
struct VectorStringHash {
    size_t operator()(const vector<string>& vec) const {
        size_t seed = vec.size();
        for (const auto& str : vec) {
            seed ^= hash<string>()(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};





void groupCustomersByHashtags(fileIterator& hashtags, fileIterator& purchases,fileIterator& prices, int k, string outputFilePath)
{
    //Use this to log compute time    
    auto start = high_resolution_clock::now();

    //cout <<"------------------------------------------------------" << endl;
    //  Write your code here
    // Map product ID to its list of hashtags
    unordered_map<int, vector<string>> productHashtags;

    while (hashtags.hasNext()) {
        string line = hashtags.next();
        if(line.size() > 0){
            stringstream ss(line);
            int product;
            string temp;
            getline(ss, temp, ',');  // Read up to the first comma
            product = stoi(temp);    // Convert to intger
            while (getline(ss, temp, ',')) {
                productHashtags[product].push_back(temp);   
            }

            // for(const string& temp : productHashtags[product]){
            //     cout << temp << " ";
            // }
            // cout << endl;
        }
    }
    
    // Map customer ID to their hashtag frequency (i.e., interests)
    unordered_map<int, unordered_map<string, int>> customerInterests;

    // Step 2: Read the purchases and build each customer's interest list based on purchased products' hashtags
    while (purchases.hasNext()) {
        string line = purchases.next();
        if (line.size() > 0){
            stringstream ss(line);
            int customer, product;
            string temp;
            getline(ss, temp, ',');
            customer = stoi(temp);
            //cout << "Customer: " << customer << endl;
            getline(ss, temp, ',');
            product = stoi(temp);
            //cout << "Product: " << product << endl;

            for (const auto& hashtag : productHashtags[product]) {
                customerInterests[customer][hashtag]++;
           }
        }
    }
    //print the customerInterests
    // for(const auto& temp : customerInterests){
    // for(const auto& customerInterestPair : customerInterests) {
    //     int customer = customerInterestPair.first;
    //     auto& interestMap = customerInterestPair.second;
    //     cout << "Customer: " << customer << endl;
    //     for(const auto& hashtagCountPair : interestMap) {
    //         cout << hashtagCountPair.first << "-" << hashtagCountPair.second <<"    ";
    //     }
    //     cout<<endl;
    // }

    // Step 3: For each customer, extract the top-k most frequent hashtags
    unordered_map<vector<string>, vector<int>, VectorStringHash> groupedCustomers;
    for (auto& customerInterestPair : customerInterests) {
        int customer = customerInterestPair.first;
        auto& interestMap = customerInterestPair.second;
        // Sort hashtags by frequency (descending order) and then lexicographically
        vector<pair<string, int>> freqHashtags(interestMap.begin(), interestMap.end());

        //cout <<customer << ":" << freqHashtags[0].first << " " << freqHashtags[0].second << endl;
        sort(freqHashtags.begin(), freqHashtags.end(), [](const auto& a, const auto& b) {
            if (a.second == b.second) return a.first < b.first;
            return a.second > b.second;
        });

        //cout << "Customer: " << customer << endl;
        // cout <<customer;
        // for(const auto& temp : freqHashtags){
        //     //cout  << ":" << temp.first << " " << temp.second;;
        // }
        

        // Extract the top-k hashtags
        vector<string> topKHashtags;
        for (int i = 0; i < min(k, (int)freqHashtags.size()); ++i) {
            topKHashtags.push_back(freqHashtags[i].first);
        }
        //sort the topKHashtags
        sort(topKHashtags.begin(), topKHashtags.end());

        // Group customers who share the same top-k hashtags
        groupedCustomers[topKHashtags].push_back(customer);
        cout << endl;
    }
    
     // cout << "Grouped Customers: ";
    for(const auto& temp : groupedCustomers){
        cout << "Group: ";
        for(const auto& temp2 : temp.first){
            cout << temp2 << " ";
        }
        cout <<"this is question 1" <<endl;
        vector<int> group;
        for(const auto& temp2 : temp.second){
            group.push_back(temp2);
            cout << temp2 << " ";

        }
        writeOutputToFile(group, outputFilePath);
        cout << endl;
    }

    // Step 4: Write the grouped customers in csv output file each group in a new line and customers in the group separated by commas
    



    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by compute part of the function: "<< duration.count() << " microseconds" << endl;

    // Use the below utility function to write the output to a file
    // Call this function for every group as a vector of integers
    
    //cout << endl << "hello world: " << group.size() << endl;
    return ;
}

//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param customerList 
 * @param purchases 
 * @param prices
 */

float calculateGroupAverageExpense(vector<int> customerList, fileIterator& purchases, fileIterator& prices){
    //Use this to log compute time    
    auto start = high_resolution_clock::now();

    unordered_map<int, vector<int>> customerPurchases;  // Maps customer ID to the list of product IDs
    unordered_map<int, float> productPrices;            // Maps product ID to price

    // Step 1: Load purchase data into a map
    while (purchases.hasNext()) {
        
        string line = purchases.next();
        if (line.size() != 0){
            stringstream ss(line);
            int customer, product;
            string temp;
            getline(ss, temp, ',');
            customer = stoi(temp);
            while (getline(ss, temp, ',')) {
                product = stoi(temp);
                customerPurchases[customer].push_back(product);   
            }
        }
    }
    // Step 2: Load price data into a map
    while (prices.hasNext()) {

        string line = prices.next();
        if (line.size() != 0){
            stringstream ss(line);
            string temp;
            int product;
            float price;
            getline(ss, temp, ',');
            product = stoi(temp);
            getline(ss, temp, ',');
            price = stof(temp);
            productPrices[product] = price; // Map product to price
        }
    }

    // Step 3: Calculate the total expense for the group
    float totalExpense = 0.0;
    int totalProducts = 0;

    for (int customer : customerList) {
        if (customerPurchases.find(customer) != customerPurchases.end()) {
            for (int product : customerPurchases[customer]) {
                if (productPrices.find(product) != productPrices.end()) {
                    totalExpense += productPrices[product];
                    totalProducts++;
                }
            }
        }
    }

    // Step 4: Calculate the average expense per customer
    float avgExpense = totalProducts > 0 ? totalExpense / customerList.size() : 0.0;    

    //write avgExpense to the file
    cout << "Average Expense: " << avgExpense << endl;


    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by this function: "<< duration.count() << " microseconds" << endl;

    return avgExpense;
}


//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices
 * @param newHashtags
 * @param k
 * @param outputFilePath
 */
void groupCustomersByHashtagsForDynamicInserts(fileIterator& hashtags, fileIterator& purchases,fileIterator& prices,vector<string> newHashtags, int k, string outputFilePath){
    //Use this to log compute time    
    auto start = high_resolution_clock::now();
    //  Write your code here
    unordered_map<int, vector<string>> productHashtags;

    cout << "size of newHashtags: " << newHashtags.size() << endl;

    while (hashtags.hasNext()) {
        string line = hashtags.next();
        if(line.size() != 0){
            stringstream ss(line);
            int product;
            string temp;
            getline(ss, temp, ',');  // Read up to the first comma
            product = stoi(temp);    // Convert to intger
            while (getline(ss, temp, ',')) {
                productHashtags[product].push_back(temp);   
            }
        }
    }
    

    // Update the productHashtags with new hashtags
    for(const auto& newHashtag : newHashtags) {
        if(newHashtag.size() > 0){
            stringstream ss(newHashtag);
            int product;
            string temp;
            getline(ss, temp, ',');  // Read up to the first comma
            product = stoi(temp);    // Convert to intger
            while (getline(ss, temp, ',')) {
                productHashtags[product].push_back(temp);  // Update the mapping with new hashtags

            }
        }
    }


    // Map customer ID to their hashtag frequency (i.e., interests)
    unordered_map<int, unordered_map<string, int>> customerInterests;

    // Step 2: Read the purchases and build each customer's interest list based on purchased products' hashtags
    while (purchases.hasNext()) {
        string line = purchases.next();
        if (line.size() != 0){
            stringstream ss(line);
            int customer, product;
            string temp;
            getline(ss, temp, ',');
            customer = stoi(temp);
            //cout << "Customer: " << customer << endl;
            getline(ss, temp, ',');
            product = stoi(temp);
            //cout << "Product: " << product << endl;
            for (const auto& hashtag : productHashtags[product]) {
                customerInterests[customer][hashtag]++;
           }
        }
    }

    // Step 3: For each customer, extract the top-k most frequent hashtags
    unordered_map<vector<string>, vector<int>, VectorStringHash> groupedCustomers;
    for (auto& customerInterestPair : customerInterests) {
        int customer = customerInterestPair.first;
        auto& interestMap = customerInterestPair.second;
        // Sort hashtags by frequency (descending order) and then lexicographically
        vector<pair<string, int>> freqHashtags(interestMap.begin(), interestMap.end());

        //cout <<customer << ":" << freqHashtags[0].first << " " << freqHashtags[0].second << endl;
        sort(freqHashtags.begin(), freqHashtags.end(), [](const auto& a, const auto& b) {
            if (a.second == b.second) return a.first < b.first;
            return a.second > b.second;
        });

        cout << "Customer: " << customer << endl;
        // cout <<customer;
        for(const auto& temp : freqHashtags){
             cout  << ":" << temp.first << " " << temp.second;;
        }

        // Extract the top-k hashtags
        vector<string> topKHashtags;
        for (int i = 0; i < min(k, (int)freqHashtags.size()); ++i) {
            topKHashtags.push_back(freqHashtags[i].first);
        }
        //sort the topKHashtags
        sort(topKHashtags.begin(), topKHashtags.end());

        // Group customers who share the same top-k hashtags
        groupedCustomers[topKHashtags].push_back(customer);
        cout << endl;
    }


    // cout << "Grouped Customers: ";
    for(const auto& temp : groupedCustomers){
        cout << "Group: ";
        for(const auto& temp2 : temp.first){
            cout << temp2 << " ";
        }
        vector<int> group;
        for(const auto& temp2 : temp.second){
            group.push_back(temp2);
            cout << temp2 << " hi hi hi ";
        }
        writeOutputToFile(group, outputFilePath);
        cout << endl;
    }

    /*groupByHashtags can be used from now on. in doing so we need to to write a csv
    files which consists of updated values of the hashtages. but  we will do it by again
    coding */

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by compute part of the function: "<< duration.count() << " microseconds" << endl;

    // Use the below utility function to write the output to a file
    // Call this function for every group as a vector of integers
    return;
    
}



#endif // USER_CODE_H