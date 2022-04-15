/*
Name : Atishay Jain
Roll No. : 20CS30008
*/

#include <bits/stdc++.h>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using namespace rapidxml;
using namespace std;

class node
{
public:
    long long int id;
    long double lat, lng;
};

class nodeWithName : public node
{
public:
    string name = "";
    string sname = ""; // short name
};

map<long long int, node> map1;                           // global variable
map<long long int, nodeWithName> map2;                   // global variable
map<long long int, unordered_set<long long int>> adList; // adjacency List

long double distance(long long int i1, long long int i2)
{
    long double lt1, lt2, ln1, ln2;
    // converting to radians
    lt1 = map1.at(i1).lat * M_PI / 180;
    lt2 = map1.at(i2).lat * M_PI / 180;
    ln1 = map1.at(i1).lng * M_PI / 180;
    ln2 = map1.at(i2).lng * M_PI / 180;

    long double dlt = lt2 - lt1; // difference calculation
    long double dln = ln2 - ln1;

    // Haversine Formula
    long double d = pow(sin(dlt / 2), 2) + cos(lt1) * cos(lt2) * pow(sin(dln / 2), 2);
    d = 2 * asin(sqrt(d)) * 6378; // Radius of earth = 6378km

    return d;
}

void search()
{
    cout << "Search : ";
    string s; // input string
    fflush(stdin);
    getline(cin, s);

    size_t f;     // for finding the substring
    int flag = 0; // to store wheather match is found or not

    for (auto i : map2) // only check with those which have name
    {
        // cout << i << "  " << map2.size() << endl;
        f = i.second.name.find(s); // finding the substring
        if (f != string::npos)     // if substring found
        {
            // printing the information
            cout << "\nName : " << i.second.name << endl;
            cout << "ID : " << i.second.id << endl;
            if (i.second.sname != "")
                cout << "Short Name : " << i.second.sname << endl;
            cout << "Longitude : " << i.second.lng << endl;
            cout << "Lattitude : " << i.second.lat << endl;
            flag = 1; // to check that there exist some node
        }
    }

    if (flag == 0) // if no match is found
    {
        cout << "No such node exist !!!" << endl;
    }
}

struct comp
{
    bool operator()(pair<long double, long long int> a, pair<long double, long long int> b)
    {
        if (a.first < b.first) // making min heap by this comparision
        {
            return false;
        }
        return true;
    }
};

void k_nearest()
{
    long long int id;
    int k;
    cout << "\nEnter id of the node : ";
    fflush(stdin); // clearing the buffer
    cin >> id;

    while (map1.find(id) == map1.end()) // if the entered id is wrong
    {
        cout << "Enter again : ";
        cin >> id;
    }

    cout << "Enter k : ";
    cin >> k;

    vector<pair<long double, long long int>> v; // vector containing all the nodes with their distance from the given node

    pair<long double, long long int> p; // temporary pair variable

    for (auto i : map1)
    {
        if (i.first == id) // same node
            continue;
        p.first = distance(i.first, id); // calculating distance
        p.second = i.first;
        v.push_back(p);
    }

    int size = v.size();

    make_heap(v.begin(), v.end(), comp()); // creating a min heap

    cout << "\tID\t:\tDistance(in km)" << endl;

    for (int i = 1; i <= k; i++)
    {
        p = v.front(); // finding the minimum element
        pop_heap(v.begin(), v.end(), comp());
        v.pop_back();
        cout << "  " << p.second << "\t:\t" << p.first << endl;
    }
}

void path()
{
    long long int i1, i2;
    cout << "\nEnter ID of first node : ";
    cin >> i1;

    while (map1.find(i1) == map1.end()) // if the entered id is wrong
    {
        cout << "Enter again : ";
        cin >> i1;
    }

    cout << "Enter ID of second node : ";
    cin >> i2;

    while (map1.find(i2) == map1.end()) // if the entered id is wrong
    {
        cout << "Enter again : ";
        cin >> i2;
    }

    if (adList.find(i1) == adList.end())
    {
        cout << "No path possible!!!" << endl;
        return;
    }

    else if (adList.find(i2) == adList.end())
    {
        cout << "No path possible!!!" << endl;
        return;
    }

    vector<long long int> vis;           // shortest distance found nodes
    map<long long int, long double> dis; // shortest distance at a step from i1

    // adding the first node
    dis.insert({i1, 0});
    vis.push_back(i1);

    long long int v2; // minimum distance at a step
    v2 = i1;          // initially

    while (v2 != i2)
    {
        unordered_set<long long int> al = adList.find(v2)->second; // adjacency list

        for (auto it : al) // looping over adjacency list
        {
            if (find(vis.begin(), vis.end(), it) != vis.end())
            {
                continue;
            }

            long double d = dis.find(v2)->second + distance(v2, it);

            if (dis.find(it) == dis.end())
            {
                dis.insert({it, d});
            }

            else if (dis.find(it)->second > d)
            {
                dis.find(it)->second = d; // setting the distance to mimimum
            }
        }

        dis.erase(v2);

        if (dis.size() == 0)
        {
            cout << "No path exist!!!" << endl;
            return;
        }

        long double min = DBL_MAX;
        for (auto it : dis)
        {
            if (it.second < min)
            {
                min = it.second;
                v2 = it.first;
            }
        }

        vis.push_back(v2);
    }
    cout << "Shortest Distance : " << dis.find(v2)->second << " km" << endl;
}

int main()
{
    file<> xmlFile("map.osm");    // openinig the file
    xml_document<> doc;           // creating a doc variable
    doc.parse<0>(xmlFile.data()); // parsing the file

    xml_node<> *n1 = doc.first_node("osm"); // parent node
    n1 = n1->first_node("node");
    int c1 = 0; // node count

    node temp;
    nodeWithName temp2;
    while (n1) // storing all the nodes information
    {
        temp.id = atoll(n1->first_attribute("id")->value());           // char* to long long int
        temp.lat = strtold(n1->first_attribute("lat")->value(), NULL); // char* to long double
        temp.lng = strtold(n1->first_attribute("lon")->value(), NULL);
        map1.insert({temp.id, temp}); // adding to the map
        c1++;                         // increasing the node count

        int flag = 0;
        xml_node<> *t = n1->first_node("tag"); // t = tag nodes
        while (t)
        {
            if (strcmp(t->first_attribute("k")->value(), "name") == 0) // if name attribute is present
            {
                temp2.id = temp.id; // id, lattitude and longitude is same
                temp2.lat = temp.lat;
                temp2.lng = temp.lng;
                temp2.name = t->first_attribute("v")->value(); // copying the name
                flag = 1;
            }
            else if (strcmp(t->first_attribute("k")->value(), "short_name") == 0)
            {
                temp2.sname = t->first_attribute("v")->value(); // copying the short name
            }
            t = t->next_sibling(); // next tag
        }
        if (flag == 1) // if name was found
        {
            map2.insert({temp2.id, temp2}); // adding to the seperate map
        }

        n1 = n1->next_sibling("node");
    }
    cout << "Number of nodes : " << c1 << endl;

    xml_node<> *n2 = doc.first_node("osm");
    n2 = n2->first_node("way");
    int c2 = 0; // count of number of ways

    while (n2) // storing the ways as adjacency list
    {
        xml_node<> *t1 = n2->first_node("nd");
        xml_node<> *t2 = t1->next_sibling("nd");
        while (t1 && t2)
        {
            long long int l1 = atoll(t1->first_attribute("ref")->value());
            long long int l2 = atoll(t2->first_attribute("ref")->value());

            // there's a way between t1 and t2
            // adding them to each others list

            if (adList.find(l1) != adList.end())
            {
                adList.find(l1)->second.insert(l2);
            }

            else
            {
                unordered_set<long long int> st = {l2};
                adList.insert({l1, st});
            }

            if (adList.find(l2) != adList.end())
            {
                adList.find(l2)->second.insert(l1);
            }

            else
            {
                unordered_set<long long int> st = {l1};
                adList.insert({l2, st});
            }

            t1 = t2;
            t2 = t2->next_sibling("nd");
        }

        c2++; // increament in the number of ways
        n2 = n2->next_sibling("way");
    }
    cout << "Number of ways : " << c2 << endl;

    int in;
    do
    {
        // main interface
        cout << "\nMenu : " << endl;
        cout << "1) Search" << endl;
        cout << "2) Find k Nearest node" << endl;
        cout << "3) Find Shortest distance" << endl;
        cout << "4) Exit!" << endl;
        cout << "Choose any one : ";

        cin >> in;
        while (in < 1 || in > 4)
        {
            cout << "Enter again : ";
            cin >> in;
        }
        switch (in) // calling the respective function
        {
        case 1:
            search();
            break;

        case 2:
            k_nearest();
            break;

        case 3:
            path();
            break;

        default:
            break;
        }
    } while (in != 4);

    return 0;
}