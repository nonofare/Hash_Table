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
		DA::DynArr<DLL::DoubLinList<Node*>*>* _array;
		size_t _lists;
		size_t _elements;

		size_t GetHashIndex(std::string key) const {
			size_t index = 0;
			unsigned int q = key.length();

			for (int i = 0; i < q; i++) {
				index += key[i] * std::pow(31, q - (i + 1));
			}
			index = index % _array->Capacity();

			return index;
		}

		void ExpandAndReHash() {
			DA::DynArr<DLL::DoubLinList<Node*>*>* new_array;

			try {
				new_array = new DA::DynArr<DLL::DoubLinList<Node*>*>(_array->Factor() * _array->Capacity());
			}
			catch (const std::bad_alloc& ex) {
				throw std::runtime_error("HT::ExpandAndReHash() -> " + std::string(ex.what()));
			}

			DA::DynArr<DLL::DoubLinList<Node*>*>* old_array = _array;
			size_t old_lists = _lists;

			_array = new_array;
			_lists = 0;

			try {
				for (int i = 0; i < old_array->Capacity(); i++) {
					if ((*old_array)[i]) {
						for (int j = 0; j < (*old_array)[i]->Size(); j++) {
							Node* temp = (*(*old_array)[i])[j];
							PasteNode(temp);
						}
						delete (*old_array)[i];
					}
				}
			}
			catch (const std::bad_alloc& ex) {
				throw std::runtime_error("HT::ExpandAndReHash() -> " + std::string(ex.what()));
			}
			
			delete old_array;
		}

		void PasteNode(Node* node) {
			size_t index = GetHashIndex(node->key);

			if (!(*_array)[index]) {
				(*_array)[index] = new DLL::DoubLinList<Node*>();
				_lists++;
			}

			try {
				(*_array)[index]->Push(node);
			}
			catch (const std::exception& ex) {
				throw std::runtime_error("HT::PasteNode() -> " + std::string(ex.what()));
			}
		}

		double CalculateArrayLoad() const {
			if (_lists) {
				return 100 / (double(_array->Capacity()) / double(_lists));
			}
			return 0;
		}

		double CalculateListElementMinCount() const {
			double min = 0.0;

			for (int i = 0; i < _array->Capacity(); i++) {
				if ((*_array)[i]) {
					if (!min) {
						min = double((*_array)[i]->Size());
					}
					if ((*_array)[i]->Size() < min) {
						min = double((*_array)[i]->Size());
					}
				}
			}

			return min;
		}

		double CalculateListElementMaxCount() const {
			int max = 0.0;

			for (int i = 0; i < _array->Capacity(); i++) {
				if ((*_array)[i] && (*_array)[i]->Size() > max) {
					max = double((*_array)[i]->Size());
				}
			}

			return max;
		}

		double CalculateListElementAvgCount() const {
			if (_lists) {
				return double(_elements) / double(_lists);
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

		HashTable() : _lists(0), _elements(0) {
			try {
				_array = new DA::DynArr<DLL::DoubLinList<Node*>*>(1024);
			}
			catch (const std::bad_alloc& ex) {
				throw std::runtime_error("HT::HashTable() -> " + std::string(ex.what()));
			}
		}

		~HashTable() {
			Erase();
			delete _array;
		}

		size_t Lists() const {
			return _lists;
		}

		size_t Elements() const {
			return _elements;
		}

		size_t Capacity() const {
			return _array->Capacity();
		}

		size_t ListSize(size_t index) const {
			if ((*_array)[index]) {
				return (*_array)[index]->Size();
			}
			return 0;
		}

		void Push(std::string key, T value) {
			Node* node = new Node(key, value);
			size_t index = GetHashIndex(key);
			
			if (!(*_array)[index]) {
				(*_array)[index] = new DLL::DoubLinList<Node*>();
				_lists++;
			}

			try {
				if (Node* existing_node = Find(node->key)) {
					existing_node->value = value;
				}
				else {
					(*_array)[index]->Push(node);
					_elements++;
				}
			}
			catch (const std::bad_alloc& ex) {
				throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
			}
			
			if (_elements > (*_array).Capacity() * FACTOR) {
				try {
					ExpandAndReHash();
				}
				catch (const std::exception& ex) {
					throw std::runtime_error("HT::Push() -> " + std::string(ex.what()));
				}
			}
		}

		Node* Find(std::string key) const {
			size_t index = GetHashIndex(key);

			if (!(*_array)[index]) {
				return nullptr;
			}

			Node* temp = new Node(key);

			try {
				if (auto found = (*_array)[index]->Find(temp, [](Node* a, Node* b) -> bool { return a->key == b->key; })) {
					return found->data;
				}
			}
			catch (const std::exception& ex) {
				delete temp;
				throw std::runtime_error("HT::Find() -> " + std::string(ex.what()));
			}

			delete temp;
			return nullptr;
		}

		void Pop(std::string key) {
			size_t index = GetHashIndex(key);

			if (!(*_array)[index]) {
				return;
			}

			Node* temp = new Node(key);

			try {
				(*_array)[index]->Remove(temp, [](Node* a, Node* b) -> bool { return a->key == b->key; });
			}
			catch (const std::exception& ex) {
				delete temp;
				throw std::runtime_error("HT::Pop() -> " + std::string(ex.what()));
			}

			if ((*_array)[index]->Size() == 0) {
				delete (*_array)[index];
				(*_array)[index] = nullptr;
				_lists--;
			}

			_elements--;
			delete temp;
		}

		void Erase() {
			for (int i = 0; i < _array->Capacity(); i++) {
				if ((*_array)[i]) {
					delete (*_array)[i];
					(*_array)[i] = nullptr;
				}
			}

			_elements = 0;
			_lists = 0;
		}

		std::string ToString(unsigned int limit = 0, std::string(*out_to_string)(T) = nullptr) const {
			if (limit <= 0 || limit > _lists) {
				limit = int(_array->Capacity());
			}

			std::string text = ">>> Hash Table <<<\n";
			text += "> elements: " + std::to_string(int(_elements)) + "\n";
			text += "> all lists: " + std::to_string(int(_array->Capacity())) + "\n";
			text += "> non null lists: " + std::to_string(int(_lists)) + "\n";
			text += "> array load: " + std::to_string(CalculateArrayLoad()) + "%\n";
			text += "> min: " + std::to_string(CalculateListElementMinCount()) + "\n";
			text += "> avg: " + std::to_string(CalculateListElementAvgCount()) + "\n";
			text += "> max: " + std::to_string(CalculateListElementMaxCount()) + "\n";
			text += "{\n";

			if (out_to_string) {
				int shown = 0;
				for (int i = 0; i < _array->Capacity(); i++) {
					if ((*_array)[i]) {
						text += std::to_string(i) + ": ";
						for (int j = 0; j < (*_array)[i]->Size(); j++) {
							text += (*(*_array)[i])[j]->key + " -> " + out_to_string((*(*_array)[i])[j]->value);
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
				for (int i = 0; i < _array->Capacity(); i++) {
					if ((*_array)[i]) {
						text += std::to_string(i) + ": ";
						for (int j = 0; j < (*_array)[i]->Size(); j++) {
							text += (*(*_array)[i])[j]->key + " -> " + std::to_string((*(*_array)[i])[j]->value);
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

			if (limit < _array->Capacity()) {
				text += "[...]\n";
			}

			text += "}\n";

			return text;
		}
	};
}