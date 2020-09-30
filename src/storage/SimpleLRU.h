#ifndef AFINA_STORAGE_SIMPLE_LRU_H
#define AFINA_STORAGE_SIMPLE_LRU_H

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>


#include <afina/Storage.h>

namespace Afina {
namespace Backend {

/**
 * # Map based implementation
 * That is NOT thread safe implementaiton!!
 */
class SimpleLRU : public Afina::Storage {
private:
using lru_node = struct lru_node {
       const  std::string key;
        std::string value;
        lru_node* prev;
        std::unique_ptr<lru_node> next;
    };

    // Maximum number of bytes could be stored in this cache.                                                                                                                                                                                    // i.e all (keys+values) must be less the _max_size
    std::size_t _max_size;
	std::size_t current_;

    // Main storage of lru_nodes, elements in this list ordered descending by "freshness": in the head
    // element that wasn't used for longest time.
    //
    // List owns all nodes
    std::unique_ptr<lru_node> _lru_head;

    // Index of nodes from list above, allows fast random access to elements by lru_node#key
    std::map<std::reference_wrapper<const std::string>, std::reference_wrapper<lru_node>, std::less<std::string>> _lru_index;
public:
    SimpleLRU(size_t max_size = 1024) : _max_size(max_size),current_(0) {}

    ~SimpleLRU() {
        _lru_index.clear();
        auto tmp = std::move(_lru_head);
		while (tmp != nullptr)
		{
			tmp->prev = nullptr;
			tmp = std::move(tmp->next);
		}
    }

    // Implements Afina::Storage interface
    bool Put(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool PutIfAbsent(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Set(const std::string &key, const std::string &value) override;

    // Implements Afina::Storage interface
    bool Delete(const std::string &key) override;

    // Implements Afina::Storage interface
    bool Get(const std::string &key, std::string &value) override;

private:
void append(lru_node* l);
void print();
void print_map();
void check(const std::string& key);
void Rem(lru_node* l);
bool isp(const std::string& key);
void del(lru_node* l);
bool f_del(const std::string key);
bool Osv();
};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STORAGE_SIMPLE_LRU_H
