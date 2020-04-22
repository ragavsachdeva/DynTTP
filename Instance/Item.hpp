#ifndef ITEM_HPP
#define ITEM_HPP

class Item {
    public:
    Item() {}
    Item(int i, long long v, long long w, int c) {
        index = i;
        value = v;
        weight = w;
        city = c;
        available = true;
    }
    int index;
    long long value;
    long long weight;
    int city;
    bool available;
};

#endif