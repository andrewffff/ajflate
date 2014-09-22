
class LinkedListChain : public Dictionary {
private:
	// Further backward links to the next item in a hash chain
	// One entry for each byte seen in the input
	// No fancy windowing - _links[100000] corresponds to the
	// 100,101st byte in the input stream
	// links chains are terminated with -1
	IntArray _links;

	// Hash table. _hashtable[n] points to the most recent offset for
	// hash value n. _links[_hashtable[n]] will point to the next most
	// recent offset and so forth. If there are no matches at
	// all then _hashtable[n] == -1
	IntArray _hashtable;

protected:
	Hasher* _hash;

	// do we add entries to the hash table within a match?
	bool _allowInMatch;

	// maximum number of backward links that potentialMatches
	// will walk through
	int _maxMatches;

public:
	LinkedListChain(Hasher* hash, const char* name = 0, int maxMatches = INT_MAX, bool allowInMatch = true)
	: _hash(hash), _allowInMatch(allowInMatch), _maxMatches(maxMatches)
	{
		_name = name;
	}

	~LinkedListChain() {
		delete _hash;
	}

	vector<int> potentialMatches(const uchar* data, int ofs) const {
		vector<int> ret;
		int n = _maxMatches;

		int current = _hashtable[(*_hash)(data + ofs)];
		while(current >= 0 && n-- > 0) {
			ret.push_back(current);
			current = _links[current];
		}

		return ret;
	}

	void insert(const uchar* data, int ofs, DictionaryEntryType t) {
		if(t == MATCH_WITHIN && !_allowInMatch) {
			return;
		}

		int n = (*_hash)(data + ofs);

		// construct link back to previous entry (or uselessly
		// write -1 over -1)
		_links[ofs] = _hashtable[n];
		_hashtable[n] = ofs;
	}
};
	
