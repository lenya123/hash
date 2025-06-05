#include "HashTable.h"
#include <functional>
#include <stdexcept>

HashTable::HashTable(size_t size) noexcept : _capacity(size), _filled(0), table(size) {}

HashTable::~HashTable() = default;

size_t HashTable::hash_function(const KeyType &key) const
{
    int base = 17;
    size_t hash = 0;
    for (size_t i = 0; i < key.size(); ++i)
    {
        hash += key[i] * pow(base, key.size() - 1 - i);
    }
    return _capacity != 0 ? hash % _capacity : 0;
}

double HashTable::getLoadFactor() {
    return static_cast<double>(_filled) / _capacity;
}

void HashTable::insert(const KeyType &key, const ValueType &value) {
    size_t index = hash_function(key);
    for (auto &pair : table[index]) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }

    table[index].emplace_back(key, value);
    _filled++;

    if (getLoadFactor() > 0.75) {
        size_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);

        for (const auto &bucket : table) {
            for (const auto &pair : bucket) {
                size_t new_index = std::hash<KeyType>{}(pair.first) % new_capacity;
                new_table[new_index].push_back(pair);
            }
        }

        table = std::move(new_table);
        _capacity = new_capacity;
    }
}

bool HashTable::find(const KeyType &key, ValueType &value) const {
    size_t index = hash_function(key);
    for (const auto &pair : table[index]) {
        if (pair.first == key) {
            value = pair.second;
            return true;
        }
    }
    return false;
}

void HashTable::remove(const KeyType &key) {
    size_t index = hash_function(key);
    auto &bucket = table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            _filled--;
            return;
        }
    }
}

ValueType& HashTable::operator[](const KeyType &key) {
    size_t index = hash_function(key);
    for (auto &pair : table[index]) {
        if (pair.first == key) {
            return pair.second;
        }
    }

    table[index].emplace_back(key, ValueType{});
    _filled++;

    if (getLoadFactor() > 0.75) {
        size_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);

        for (const auto &bucket : table) {
            for (const auto &pair : bucket) {
                size_t new_index = std::hash<KeyType>{}(pair.first) % new_capacity;
                new_table[new_index].push_back(pair);
            }
        }

        table = std::move(new_table);
        _capacity = new_capacity;
    }

    index = hash_function(key);
    for (auto &pair : table[index]) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    throw std::runtime_error("error in operator");
}
