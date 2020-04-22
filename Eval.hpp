#ifndef EVAL_HPP
#define EVAL_HPP

#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

const int maxsize = 100000 + 5;
vector<string> split(const string &s, char delim);
template <typename T>
bool parseField(string const &match, string input, T &output);

// template from Mike Seymour
// (http://stackoverflow.com/questions/14590410/stoi-and-stoll-in-c)
template <typename T>
T ston(string const &s)
{
    stringstream ss(s);
    T result;
    ss >> result;
    return result;
}

class EvalItem
{
  public:
    EvalItem()
    {
        selected = false;
    }

    int index;        // EvalItem's index
    long long value;  // EvalItem's value
    long long weight; // EvalItem's weight
    bool selected;    // False if the item is not
                      //picked in the packing plan, and true otherwise
};

ostream &operator<<(ostream &os, const EvalItem &item);

class EvalCity
{
  public:
    int index;          // Index of the city
    vector<EvalItem> items; // Set of items assigned to the city
    int distance;       // Distance to the next city in the tour.
                        // REMARK: distance for Tour[n-1] corresponds to the
                        // distance between cities (n-1) and 0 of solution Tour
    int m;              // Number of items in the city
    double positionX;   // Position X of the city
    double positionY;   // Position Y of the city
};

ostream &operator<<(ostream &os, const EvalCity &city);

class ProblemTTP
{
  public:
    static vector<EvalCity> cities;    // Set of cities in the instance
    static size_t n;               // Number of cities in the instance
    static size_t mItems;          // Total number of items in the instance
    static map<int, int> itemsAll; // indexed map of pointers to cities containing items
    static double r;               // Renting rate
    static double minSpeed;        // Minimal speed
    static double maxSpeed;        // Maximal speed
    static long long maxWeight;    // Maximal knapsack's weight

    // function to read in TPP problem specificaiton from a file
    static bool readTTPInstance(string fileName)
    {
        string input;
        ifstream sr(fileName.c_str());
        if (!sr.is_open())
            return false;

        bool status = getline(sr, input) ? true : false;
        while ((status) && (!(input.find("NODE_COORD_SECTION") != string::npos)))
        {
            string match;    // holds string to match
            string stripped; // holds processed string stripped of spaces.

            // match on keyword read in number, assign to var and continue;
            if (parseField<size_t>("DIMENSION:", input, n))
            {
                //cout << "n: " << n << endl;
                ;
            }
            else if (parseField<size_t>("NUMBER OF ITEMS:", input, mItems))
            {
                //cout << "mItems: " << mItems << endl;
                ;
            }
            else if (parseField<double>("RENTING RATIO:", input, r))
            {
                //cout << "r: " << r << endl;
                ;
            }
            else if (parseField<double>("MIN SPEED:", input, minSpeed))
            {
                //cout << "minSpeed: " << minSpeed << endl;
                ;
            }
            else if (parseField<double>("MAX SPEED:", input, maxSpeed))
            {
                //cout << "maxSpeed: " << maxSpeed << endl;
                ;
            }
            else if (parseField<long long>("CAPACITY OF KNAPSACK:", input, maxWeight))
            {
                //cout << "maxWeight: " << maxWeight << endl;
                ;
            }
            // read new line
            status = getline(sr, input) ? true : false;
        }

        // now we are at the map section of the file.
        // ******************************************
        // read the cities.
        status = getline(sr, input) ? true : false; // read the first line
        // then subsequent lines
        // read the cities -- one value per line
        for (size_t i = 0; i < n; i++)
        {
            cities.push_back(EvalCity()); // add a new city object
            if (readEvalCity(removeDoubleSpace(replaceGeneric(input, "\t", " ")),
                         cities[i]))
            {
                status = getline(sr, input) ? true : false; // read next line
            }
            else
            {
                // there was a problem reading... report and return
                cerr << "inconsistent city data in file" << endl;
                return false;
            }
        }

        // Now we are at the items section of the file.
        // ********************************************

        status = getline(sr, input) ? true : false; // read the first line
        // reading the list of available values at nodes.
        for (size_t i = 0; i < mItems; i++)
        {
            int nodeIndex = 0; // the node index of the item
            EvalItem item;         // a new item.
            if (readEvalItem(removeDoubleSpace(replaceGeneric(input, "\t", " ")),
                         item,
                         nodeIndex))
            {

                cities[nodeIndex - 1].items.push_back(item); // add the item to the
                                                             // city specified by the
                                                             // item's entry
                itemsAll[item.index] = nodeIndex;            // record the city in which the item
                                                             // resides

                status = getline(sr, input) ? true : false; // read next line
            }
            else
            {
                // there was a problem reading... report and return
                cerr << "inconsistent item data in file" << endl;
                return false;
            }
        }
        return true;
    }

    // Function to compute a distance between two cities
    static int getDistance(EvalCity a, EvalCity b)
    {
        return (int)ceil(sqrt(pow(a.positionX - b.positionX, 2) +
                              pow(a.positionY - b.positionY, 2)));
    }

    static string replaceGeneric(string s,
                                 const string &search,
                                 const string &replace)
    {
        size_t pos;
        size_t len;
        string result = s;
        len = search.length();
        pos = result.find(search);
        while (pos != string::npos)
        {
            result.replace(pos, len, replace);
            pos = result.find(search);
        }
        return result;
    }

    static string removeDoubleSpace(string s)
    {
        return replaceGeneric(s, "  ", " ");
    }

    static bool readEvalItem(string input, EvalItem &item, int &nodeIndex)
    {

        vector<string> tokens;
        try
        {
            // split on tokens and check size
            tokens = split(input, ' ');
            if (tokens.size() != 4)
                return false;

            // do conversions
            item.index = ston<int>(tokens[0]);
            item.value = ston<long long>(tokens[1]);
            item.weight = ston<long long>(tokens[2]);
            nodeIndex = ston<int>(tokens[3]);
            return true;
        }
        catch (int e)
        {
            // exception in processing return false
            return false;
        }
    }

    static bool readEvalCity(string input, EvalCity &city)
    {
        vector<string> tokens;
        try
        {
            // split on tokens and check size
            tokens = split(input, ' ');
            if (tokens.size() != 3)
                return false;

            // do conversions
            city.index = ston<int>(tokens[0]);
            city.positionX = ston<double>(tokens[1]);
            city.positionY = ston<double>(tokens[2]);
            return true;
        }
        catch (int e)
        {
            // exception in processing return false
            return false;
        }
    }
};

class EvalSolution
{
  public:
    static vector<EvalCity *> tour; // tour of cities in solution

    // calculate fitness of the solution..
    // this is the last thing that is called after reading in the files.
    static double calculateObjectiveValue(double minSpeed,
                                          double maxSpeed,
                                          long long maxWeight,
                                          double r)
    {

        long collectedWeight = 0;  // collected weight while travelling
        double objectiveValue = 0; // return value

        // calculate speed of thief
        double speedCoef = (maxSpeed - minSpeed) / maxWeight;

        //go through all the cities in items.
        for (size_t i = 0; i < tour.size(); i++)
        {
            EvalCity city = *(tour[i]); // current city

            // for each item in city
            for (size_t j = 0; j < city.items.size(); j++)
            {
                EvalItem item = city.items[j];

                // if the item is marked on the packing list
                if (item.selected)
                {
                    // add it to the knapsack
                    collectedWeight += item.weight;

                    // if we exceed the capacity of the knapsack return
                    // a really bad fitness value
                    if (collectedWeight > ProblemTTP::maxWeight)
                    {
                        cerr << "max weight exceeded" << endl;
                        return INT_MIN;
                    }
                    // add the item's value to the objective value
                    objectiveValue += item.value;
                }
            }
            objectiveValue -= city.distance * r /
                              (maxSpeed - speedCoef * collectedWeight);
        }
        return objectiveValue;
    }

    static bool readEvalSolution(string fileName)
    {

        // input file format has two lines
        // the first line is a comma separated array of city indexes (length n)
        // the second line is a comma separated array of locations of each items
        // length mItems.

        // read a line

        string input;                  // input string to hold line
        ifstream sr(fileName.c_str()); // open the file

        if (!sr.is_open())
            return false; // file can't open then exit

        getline(sr, input); // file can open grab the first line
        string commaSep = ProblemTTP::removeDoubleSpace(
            ProblemTTP::replaceGeneric(
                ProblemTTP::replaceGeneric(input, "[", " "), "]", " "));

        // now read in cities.
        // ******************
        vector<string> tokens; // container for numbers on the line
        set<int> citySet;      // set of cities for sanity check.

        try
        {
            // split on tokens
            tokens = split(commaSep, ',');
            // sanity check the size of the tour
            if (tokens.size() != ProblemTTP::n)
            {
                cerr << "tour is incorrect size " << endl;
                return false;
            }
            // iterate over tokens
            for (size_t i = 0; i < tokens.size(); i++)
            {

                int index = ston<int>(tokens[i]); // parse token

                // check to see if tour starts at city 1
                if ((i == 0) && (index != 1))
                {
                    cerr << "error: tour must start at city 1" << endl;
                    return false;
                }

                tour.push_back(&(ProblemTTP::cities[index - 1])); // index into cities

                citySet.insert(index); // add the index to the set of cities

                // calculate distance to next city
                // if we aren't at the last index
                EvalCity next;
                if (i < (ProblemTTP::n - 1))
                {
                    // calc distance from city i to i+1;
                    next = ProblemTTP::cities[ston<int>(tokens[i + 1]) - 1];
                }
                else
                {
                    // otherwise calculate distance back to node 0
                    next = ProblemTTP::cities[ston<int>(tokens[0]) - 1];
                }
                tour[i]->distance = ProblemTTP::getDistance(*(tour[i]), next);
            }

            // was the set of cities are unique
            if (citySet.size() != ProblemTTP::n)
            {
                cerr << "might be some duplicate cities" << endl;
                return false;
            }

            // now to read the allocation of items
            // ***********************************
            getline(sr, input); // grab the line of items
            // remove everything except the commas and split
            commaSep = ProblemTTP::removeDoubleSpace(
                ProblemTTP::replaceGeneric(
                    ProblemTTP::replaceGeneric(input, "[", " "), "]", " "));
            // split on tokens
            tokens = split(commaSep, ',');
            // now iterate through all the items in the vector
            // the items are numbered from 1 so we have to subtract 1 from the number

            for (size_t i = 0; i < tokens.size(); i++)
            {
                // read in the index.. subtract one and mark as selected.
                int itemIndex = ston<int>(tokens[i]);
                if (itemIndex < 1)
                {
                    // if the list is empty or contains values less than one
                    // quit
                    return false;
                }

                // set selected=true for the item .. go to the city and scan

                //Mark EvalItem as selected
                //Find the city and then scan the list of items for that city
                int cityIndex = ProblemTTP::itemsAll[itemIndex]; // get the city index        // get handle on items
                vector<EvalItem> *itemsPtr = &(ProblemTTP::cities[cityIndex - 1].items);
                // scan through the list... break when you find it
                for (size_t i = 0; i < itemsPtr->size(); i++)
                {

                    // is the item index found?
                    if ((*itemsPtr)[i].index == itemIndex)
                    {
                        // yes mark as selected  and break
                        (*itemsPtr)[i].selected = true;
                        break;
                    }
                }
            }
            // that's it
        }
        catch (int e)
        {
            // exception in processing return false
            return false;
        }
        return true;
    } // end read solution.
};

// template to parse a input field (labelled with match)
// into an output reference type
// if there is no match then the output is left untouched.
// returns true if there is a match.
template <typename T>
bool parseField(string const &match,
                string input, T &output)
{
    size_t pos;
    string stripped;
    bool res = false;
    if ((pos = input.find(match)) != string::npos)
    {
        res = true;
        input.replace(pos, match.length(), "");          // get rid of label
        stripped = ProblemTTP::removeDoubleSpace(input); // strip to space
        output = ston<T>(stripped);                      // convert to num
    }
    return res;
}

// derived from split function from Evan Teran
vector<string> split(const string &s, char delim)
{
    vector<string> res;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        res.push_back(item);
    }
    return res;
}

// overloading of operator <<  derived from Zorawa
// http://stackoverflow.com/questions/10750057/c-printing-out-the-contents-of-a-vector/11335634#11335634
// not standard in C++
template <typename T>
ostream &operator<<(ostream &out, const vector<T> &v)
{
    if (!v.empty())
    {
        out << '[';
        for (size_t i = 0; i < v.size(); i++)
        {
            out << v[i] << " , " << endl;
        }
        out << "\b\b]";
    }
    return out;
}

ostream &operator<<(ostream &os, const EvalItem &item)
{
    os << "index: " << item.index;
    os << " value: " << item.value;
    os << " weight: " << item.weight;
    os << " selected: " << item.selected;
    return os;
}

ostream &operator<<(ostream &os, const EvalCity &city)
{
    os << "index: " << city.index;
    os << " items: " << city.items;
    os << " distance: " << city.distance;
    os << " m: " << city.m;
    os << " positionX: " << city.positionX;
    os << " positionY: " << city.positionY;
    return os;
}

#endif
