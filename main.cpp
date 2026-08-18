#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <set>
#include <functional>

using namespace std;

struct Entry {
    char index[64];
    int value;

    bool operator<(const Entry& other) const {
        int cmp = memcmp(index, other.index, 64);
        if (cmp != 0) return cmp < 0;
        return value < other.value;
    }
    bool operator==(const Entry& other) const {
        return memcmp(index, other.index, 64) == 0 && value == other.value;
    }
};

size_t get_bucket(const string& index) {
    return hash<string>{}(index) % 10;
}

string get_bucket_filename(size_t b) {
    return "bucket_" + to_string(b) + ".bin";
}

void load_bucket(size_t b, set<Entry>& entries) {
    string filename = get_bucket_filename(b);
    ifstream is(filename, ios::binary);
    if (!is) return;
    Entry e;
    while (is.read((char*)&e, sizeof(Entry))) {
        entries.insert(e);
    }
}

void save_bucket(size_t b, const set<Entry>& entries) {
    string filename = get_bucket_filename(b);
    ofstream os(filename, ios::binary | ios::trunc);
    for (const auto& e : entries) {
        os.write((char*)&e, sizeof(Entry));
    }
}

size_t current_bucket = 11;
set<Entry> current_entries;
bool bucket_modified = false;

void switch_bucket(size_t b) {
    if (current_bucket == b) return;
    if (current_bucket != 11 && bucket_modified) {
        save_bucket(current_bucket, current_entries);
    }
    current_bucket = b;
    current_entries.clear();
    load_bucket(b, current_entries);
    bucket_modified = false;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (!(cin >> n)) return 0;

    while (n--) {
        string cmd;
        if (!(cin >> cmd)) break;
        if (cmd == "insert") {
            string index_str;
            int value;
            cin >> index_str >> value;
            size_t b = get_bucket(index_str);
            switch_bucket(b);
            
            Entry e;
            memset(e.index, 0, 64);
            size_t len = index_str.length();
            if (len > 64) len = 64;
            memcpy(e.index, index_str.c_str(), len);
            e.value = value;
            
            if (current_entries.insert(e).second) {
                bucket_modified = true;
            }
        } else if (cmd == "delete") {
            string index_str;
            int value;
            cin >> index_str >> value;
            size_t b = get_bucket(index_str);
            switch_bucket(b);
            
            Entry e;
            memset(e.index, 0, 64);
            size_t len = index_str.length();
            if (len > 64) len = 64;
            memcpy(e.index, index_str.c_str(), len);
            e.value = value;
            
            if (current_entries.erase(e)) {
                bucket_modified = true;
            }
        } else if (cmd == "find") {
            string index_str;
            cin >> index_str;
            size_t b = get_bucket(index_str);
            switch_bucket(b);
            
            char index_buf[64];
            memset(index_buf, 0, 64);
            size_t len = index_str.length();
            if (len > 64) len = 64;
            memcpy(index_buf, index_str.c_str(), len);
            
            bool found = false;
            bool first = true;
            
            Entry e_start;
            memset(e_start.index, 0, 64);
            memcpy(e_start.index, index_buf, 64);
            e_start.value = -1;
            
            auto it = current_entries.lower_bound(e_start);
            while (it != current_entries.end() && memcmp(it->index, index_buf, 64) == 0) {
                if (!first) cout << " ";
                cout << it->value;
                first = false;
                found = true;
                ++it;
            }
            
            if (!found) {
                cout << "null";
            }
            cout << "\n";
        }
    }
    
    if (current_bucket != 257 && bucket_modified) {
        save_bucket(current_bucket, current_entries);
    }
    
    return 0;
}
