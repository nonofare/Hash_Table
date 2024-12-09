#pragma once
#include <string>
#include <cmath>
#include "DLL.h"
#include "DA.h"

// rehash
// find

namespace HT {

	template <typename T>
	class HashTable {

		struct Node;

		const double FACTOR = 0.75;
		DA::DynArr<DLL::DoubLinList<Node*>*> _array;
		size_t _lists;
		size_t _size;

		size_t Hash(std::string key) const {
			size_t index = 0;

			for (int i = 0; i < key.length(); i++) {
				index += key[i] * pow(31, key.length() - (i + 1));
			}
			index = index % _array.Capacity();

			return index;
		}

		void ReHash() {
			DLL::DoubLinList<Node*> temp;

			for (int i = 0; i < _array.Capacity(); i++) {
				if (_array[i]) {
					for (int j = 0; j < _array[i]->Size(); j++) {
						try {
							temp.Push((*_array[i])[j]);
						}
						catch (const std::exception& ex) {
							throw std::runtime_error("HT::ReHash() -> " + std::string(ex.what()));
						}
					}
					_array[i] = nullptr;
					_lists--;
				}
			}

			try {
				size_t new_capacity = _array.Capacity() * _array.Factor();
				while (_array.Size() < new_capacity) {
					_array.Push(nullptr);
				}
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::ReHash() -> " + std::string(ex.what()));
			}

			for (int i = 0; i < temp.Size(); i++) {
				size_t index = Hash(temp[i]->key);
				try {
					if (!_array[index]) {
						try {
							_array[index] = new DLL::DoubLinList<Node*>();
						}
						catch (const std::exception& ex) {
							throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
						}
						_lists++;
					}
					_array[index]->Push(temp[i]);
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::ReHash() -> " + std::string(ex.what()));
				}
				temp[i] = nullptr;
			}
		}

		int CalculateListElementMinCount() const {
			int min = 0;

			for (int i = 0; i < _array.Capacity(); i++) {
				if (_array[i]) {
					if (!min) {
						min = int(_array[i]->Size());
					}
					if (_array[i]->Size() < min && _array[i]->Size() != 0) {
						min = int(_array[i]->Size());
					}
				}
			}

			return min;
		}

		int CalculateListElementMaxCount() const {
			int max = 0;

			for (int i = 0; i < _array.Capacity(); i++) {
				if (_array[i] && _array[i]->Size() > max) {
					max = int(_array[i]->Size());
				}
			}

			return max;
		}

		int CalculateListElementAvgCount() const {
			if (_lists) {
				return int(_size) / int(_lists);
			}

			return 0;
		}

	public:
		struct Node {
			std::string key;
			T value;

			Node(std::string in_key) : key(in_key) {}
			Node(std::string in_key, T in_value) : key(in_key), value(in_value) {}
		};

		HashTable() : _lists(0), _size(0) {
			while (_array.Capacity() < 1024) {
				try {
					_array.ExpandArray();
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::HashTable() -> " + std::string(ex.what()));
				}
			}
		}

		~HashTable() {
			Erase();
			_array.Erase();
		}

		size_t Lists() const {
			return _lists;
		}

		size_t Size() const {
			return _size;
		}

		size_t Capacity() const {
			return _array.Capacity();
		}

		size_t ListSize(size_t index) const {
			if (_array[index]) {
				return _array[index]->Size();
			}
			throw std::runtime_error("HT::ListSize(): index (" + std::to_string(index) + ") was nullptr");
		}

		void Push(std::string key, T value) {
			Node* node = new Node(key, value);
			size_t index = Hash(key);
			
			if (!_array[index]) {
				try {
					_array[index] = new DLL::DoubLinList<Node*>();
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
				}
				_lists++;
			}

			Node* old_node = nullptr;
			try {
				//old_node = Find(node->key);
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
			}

			if (old_node) {
				old_node->value = value;
			}
			else {
				try {
					_array[index]->Push(node);
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
				}
				_size++;
			}

			if (_size > _array.Capacity() * FACTOR) {
				try {
					ReHash();
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
				}
			}
		}

		Node* Find(std::string key) const {
			size_t index = Hash(key);

			if (!_array[index]) {
				return nullptr;
			}

			if (index < _array.Capacity()) {
				Node* temp = new Node(key);
				Node* node = nullptr;

				try {
					node = _array[index]->Find(key, [](Node* a, Node* b) -> bool { return (a->key == b->key); })->data;
				}
				catch (const std::exception& ex) {
					delete temp;
					throw std::runtime_error("HT::Find() -> " + std::string(ex.what()));
				}

				delete temp;
				return node;
			}
			else {
				throw std::out_of_range("HT::Find(): index (" + std::to_string(index) + ") was greater or equal to array capacity (" + std::to_string(int(_array.Capacity())) + ")");
			}
		}

		void Pop(std::string key) {
			size_t index = Hash(key);

			if (!_array[index]) {
				throw std::runtime_error("HT::Pop(): index (" + std::to_string(index) + ") was nullptr");
			}

			if (index < _array.Capacity()) {
				Node* temp = new Node(key);

				try {
					_array[index]->Remove(temp, [](Node* a, Node* b) -> bool { return (a->key == b->key); });
				}
				catch (const std::exception& ex) {
					delete temp;
					throw std::runtime_error("HT::Pop() -> " + std::string(ex.what()));
				}

				if (!_array[index]->Size()) {
					delete _array[index];
					_array[index] = nullptr;
					_lists--;
				}

				_size--;
				delete temp;
			}
			else {
				throw std::out_of_range("HT::Pop(): index (" + std::to_string(index) + ") was greater or equal to array capacity (" + std::to_string(int(_array.Capacity())) + ")");
			}
		}

		void Erase() {
			try {
				for (int i = 0; i < _array.Capacity(); i++) {
					if (_array[i]) {
						_array[i]->Erase();
					}
				}
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::Erase() -> " + std::string(ex.what()));
			}

			_size = 0;
			_lists = 0;
		}

		std::string ToString(unsigned int limit = 0, std::string(*cmp_string)(T) = nullptr) const {
			if (limit <= 0 || limit > _lists) {
				limit = int(_array.Capacity());
			}

			std::string text = ">>> Hash Table <<<\n";
			text += "size: " + std::to_string(int(_size)) + "\n";
			text += "lists: " + std::to_string(int(_lists)) + "\n";
			text += "capacity: " + std::to_string(int(_array.Capacity())) + "\n";
			text += "list min: " + std::to_string(CalculateListElementMinCount()) + "\n";
			text += "list avg: " + std::to_string(CalculateListElementAvgCount()) + "\n";
			text += "list max: " + std::to_string(CalculateListElementMaxCount()) + "\n";
			text += "{\n";

			if (cmp_string) {
				int shown = 0;
				for (int i = 0; i < _array.Capacity(); i++) {
					if (_array[i]) {
						text += std::to_string(i) + ": ";
						for (int j = 0; j < _array[i]->Size(); j++) {
							text += (*_array[i])[j]->key + " -> " + cmp_string((*_array[i])[j]->value);
							text += "; ";
						}
						text += "\n";
						shown++;
					}
					if (shown >= limit) {
						break;
					}
				}
			}
			else if constexpr (std::is_arithmetic_v<T>) {
				int shown = 0;
				for (int i = 0; i < _array.Capacity(); i++) {
					if (_array[i]) {
						text += std::to_string(i) + ": ";
						for (int j = 0; j < _array[i]->Size(); j++) {
							text += (*_array[i])[j]->key + " -> " + std::to_string((*_array[i])[j]->value);
							text += "; ";
						}
						text += "\n";
						shown++;
					}
					if (shown >= limit) {
						break;
					}
				}
			}
			else {
				text = "T was not arithmetic and no cmp was provided\n";
			}

			if (limit < _array.Capacity()) {
				text += "[...]\n";
			}

			text += "}\n";

			return text;
		}
	};
}