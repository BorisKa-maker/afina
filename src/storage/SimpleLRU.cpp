#include "SimpleLRU.h"

namespace Afina {
namespace Backend {

bool SimpleLRU::Put(const std::string& key, const std::string& value) 
	{
		if (sizeof(key) + sizeof(value) > _max_size)
		{
			return false;
		}
		auto it = _lru_index.find(key);
		if (it != _lru_index.end())
		{
			while (current_ - sizeof(it->second.get().value) + sizeof(value) > _max_size)
			{
				Osv();
			}
			return Set(key, value);
			
		}
		else
		{
			while (current_ + sizeof(key) + sizeof(value) > _max_size)
			{
				Osv();
			}

			append(new lru_node{ key, value,nullptr,nullptr });
			current_ = current_ + sizeof(key) + sizeof(value);
			return true;
		}
	}
bool SimpleLRU::Delete(const std::string& key) 
	{
		return f_del(key);
	}
	bool SimpleLRU::PutIfAbsent(const std::string& key, const std::string& value)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return Put(key, value);
		}
		else
		{
			return false;
		}
	}
	bool SimpleLRU::Set(const std::string& key, const std::string& value)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return false;
		}
		else
		{
			it->second.get().value = value;
			isp(key);
			current_ = current_ - sizeof(it->second.get().value) + sizeof(value);
			return true;
		}
	}
	bool SimpleLRU::Get(const std::string& key, std::string& value)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return false;
		}
		else
		{	value = it->second.get().value;
			isp(key);
			return true;
		}
	}
	void SimpleLRU::append(lru_node* l)
	{

		if (_lru_head == nullptr)
		{
			_lru_index.insert({std::reference_wrapper<const std::string>(l->key), std::reference_wrapper<lru_node>(*l)});
			_lru_head.reset(l);
			l->prev = nullptr;
			
		}
		else
		{
			_lru_head->prev = l;
			l->next = std::move(_lru_head);
			_lru_index.insert({std::reference_wrapper<const std::string>(l->key), std::reference_wrapper<lru_node>(*l)});
			_lru_head.reset(l);
			l->prev = nullptr;
			
		}
	}
	void SimpleLRU::print()
	{
		//std::cout << "Hello";
		if (_lru_head == nullptr)
		{
			return;
		}
		auto tmn = _lru_head.get();
		while (tmn != nullptr)
		{
			//std::cout << "Hello";
			std::cout << tmn->value<<"   ";
			tmn = tmn->next.get(); 
		}
		std::cout<<std::endl;
	}
	void SimpleLRU::print_map()
	{
		for (auto i : _lru_index)
		{
			std::cout << i.first.get() << "  " << i.second.get().value << std::endl;
		}
	}
	void SimpleLRU::check(const std::string& key)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return;
		}
		else
		{
			std::cout << it->second.get().value<<std::endl;
		}
	}
	void SimpleLRU::Rem(lru_node* node)
	{
if (!node->next.get()) 
{
_lru_head.get()->prev = node;
node->next = std::move(_lru_head);
_lru_head = std::move(node->prev->next);

} else {
auto tmp = std::move(_lru_head);
_lru_head = std::move(node->prev->next);
node->prev->next = std::move(node->next);
tmp.get()->prev = node;
node->next = std::move(tmp);
node->prev->next->prev = node->prev;
} 
	}
	bool SimpleLRU::isp(const std::string& key)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return false;
		}
		else
		{
			
			lru_node* node = &it->second.get();
			//std::cout << "hello";
			if (_lru_head.get() == node)
			{
				//std::cout << "gggg";
				return true;
			}
			else
			{
				//std::cout << "hello";
				Rem(node);
				//std::cout << "Hello_d";
				return true;
			}
		}
	}
	void SimpleLRU::del(lru_node* l)
	{
		//l->prev_->next_ = std::move(l->next_);
		//std::cout << "TEST:prev == " << l->prev_->data_;
		//std::cout << "h_d" << std::endl;
		current_ -= sizeof(l->value) + sizeof(l->key);
		if (l->next != nullptr)
		{
			l->next->prev = l->prev;
		}
		else
		{
			l->prev->next = nullptr;
			return;
		}
		l->prev->next = std::move(l->next);
		//std::cout << "WTF";
	}

	bool SimpleLRU::f_del(const std::string key)
	{
		auto it = _lru_index.find(key);
		if (it == _lru_index.end())
		{
			return false;
		}
		else
		{
			lru_node* node = &it->second.get();
			if (_lru_head.get() == node)
			{
				current_ -= sizeof(node->value) + sizeof(node->key);

				if (_lru_head->next == nullptr)
				{
					_lru_head = nullptr;
					_lru_index.erase(it);
					return true;
				}
				node->next->prev = nullptr;
				_lru_head = std::move(node->next);
				_lru_index.erase(it);
			}
			else
			{
				del(node);
				_lru_index.erase(it);
			}
		return true;
		}
	}
	bool SimpleLRU::Osv()
	{
		//std::cout << "oSv" << std::endl;
		if (_lru_head == nullptr)
		{
			return false;
		}
		lru_node* tmp = _lru_head.get();
		while (tmp->next != nullptr)
		{
			tmp = tmp->next.get();
		}
		//std::cout << "tmp.data == " << tmp->key_ << std::endl;
		return f_del(tmp->key);
		
	}
} // namespace Backend
} // namespace Afina
