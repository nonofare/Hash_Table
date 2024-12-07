#pragma once
#include <string>
#include "DLL.h"
#include "DA.h"

namespace HT {

	template <typename T>
	class HashTable {

		struct Node {
			std::string key;
			T value;

			Node(std::string in_key, T in_value) : key(in_key), value(in_value) {}
		};

		const double FACTOR = 0.75;
		DA::DynArr<DLL::DoubLinList<Node>> array;

		void Hash(std::string key, T value) {
			
		}

		void ReHash() {

		}

	public:
		HashTable() {

		}

		~HashTable() {

		}

		size_t Size() const {
			
		}

		size_t Capacity() const {
			
		}

		void Push(std::string key, T value) {
			
		}

		Node* Find(std::string key) const {

		}

		void Pop(std::string key) {

		}

		void Erase() {

		}

		std::string ToString(unsigned int limit = 0, std::string(*cmp_string)(T) = nullptr) const {

		}
	};
}