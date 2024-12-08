#pragma once
#include <string>
#include <cmath>
#include "DLL.h"
#include "DA.h"

namespace HT {

	template <typename T>
	class HashTable {

		struct Node;

		const double FACTOR = 0.75;
		DA::DynArr<DLL::DoubLinList<Node*>> _array;
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
			// Add logic
			_array.Expand();
		}

		int CalculateListElementMinCount() const {
			int min = 0;

			for (int i = 0; i < _array.Capacity(); i++) {
				if (_array[i].Size() < min && _array[i].Size() != 0) {
					min = _array[i].Size();
				}
			}

			return min;
		}

		int CalculateListElementMaxCount() const {
			int max = 0;

			for (int i = 0; i < _array.Capacity(); i++) {
				if (_array[i].Size() > max && _array[i].Size() != 0) {
					max = _array[i].Size();
				}
			}

			return max;
		}

		int CalculateListElementAvgCount() const {
			int count = 0;

			if (_array.Size()) {
				return _size / _array.Size();
			}

			return 0;
		}

	public:
		struct Node {
			std::string key;
			T value;

			Node() = default;
			Node(std::string in_key) : key(in_key) {}
			Node(std::string in_key, T in_value) : key(in_key), value(in_value) {}
		};

		HashTable() : _size(0) {
			while (_array.Capacity() < 1024) {
				try {
					_array.Expand();
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::HashTable() -> " + std::string(ex.what()));
				}
			}
		}

		~HashTable() {
			Erase();
		}

		size_t Size() const {
			return _size;
		}

		size_t ArraySize() const {
			return _array.Size();
		}

		size_t ArrayCapacity() const {
			return _array.Capacity();
		}

		size_t ListSize(size_t index) const {
			return _array[index].Size();
		}

		void Push(std::string key, T value) {
			Node* node = new Node(key, value);
			Node* old_node = nullptr;
			size_t index = Hash(key);

			try {
				old_node = Find(key);
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
			}

			if (old_node) {
				old_node->value = value;
			}
			else {
				try {
					_array[index].Push(node);
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

			if (index < _array.Capacity()) {
				Node* temp = new Node(key);
				Node* node = nullptr;

				try {
					node = _array[index].Find(temp, [](Node* a, Node* b) { return (a->key == b->key); })->data;
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

			if (index < _array.Capacity()) {
				Node* temp = new Node(key);

				try {
					_array[index].Remove(temp, [](Node* a, Node* b) { return (a->key == b->key); });
				}
				catch (const std::exception& ex) {
					delete temp;
					throw std::runtime_error("HT::Pop() -> " + std::string(ex.what()));
				}

				delete temp;
			}
			else {
				throw std::out_of_range("HT::Pop(): index (" + std::to_string(index) + ") was greater or equal to array capacity (" + std::to_string(int(_array.Capacity())) + ")");
			}
		}

		void Erase() {
			try {
				for (int i = 0; i < _array.Capacity(); i++) {
					_array[i].Erase();
				}
				_array.Erase();
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::Erase() -> " + std::string(ex.what()));
			}

			_size = 0;
		}

		std::string ToString(unsigned int limit = 0, std::string(*cmp_string)(T) = nullptr) const {
			if (limit <= 0 || limit > _array.Capacity()) {
				limit = _array.Capacity();
			}

			std::string text = ">>> Hash Table <<<\n";
			text += "size: " + std::to_string(int(_size)) + "\n";
			text += "array size: " + std::to_string(int(_array.Size())) + "\n";
			text += "array capacity: " + std::to_string(int(_array.Capacity())) + "\n";
			text += "list min: " + std::to_string(CalculateListElementMinCount()) + "\n";
			text += "list avg: " + std::to_string(CalculateListElementAvgCount()) + "\n";
			text += "list max: " + std::to_string(CalculateListElementMaxCount()) + "\n";
			text += "{\n";

			if (cmp_string) {
				for (int i = 0; i < _array.Capacity(); i++) {
					if (_array[i].Size() > 0) {
						text += i + ": ";
						for (int j = 0; j < _array[i].Size(); j++) {
							text += _array[i][j]->key + " -> ";
							if (_array[i][j]->value) {
								text += cmp_string(_array[i][j]->value);
							}
							else {
								text += "-> null";
							}
							text += "; ";
						}
					}
					text += "\n";
				}
			}
			else if constexpr (std::is_arithmetic_v<T>) {
				for (int i = 0; i < _array.Capacity(); i++) {
					if (_array[i].Size() > 0) {
						text += i + ": ";
						for (int j = 0; j < _array[i].Size(); j++) {
							text += _array[i][j]->key + " -> ";
							if (_array[i][j]->value) {
								text += std::to_string(_array[i][j]->value);
							}
							else {
								text += "-> null";
							}
							text += "; ";
						}
					}
					text += "\n";
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