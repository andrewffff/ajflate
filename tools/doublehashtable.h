
// Keeps two hash tables of the given size, and will find potential matches from 
// either. There are two possible insertion strategies:
//
//   - CHAIN_ALTERNATE: just randomly alternate between the tables
//   - CHAIN_PROMOTION: use one table primarily, and "promote" entries that produce
//                      matches into the other

enum ChainStrategy {
	CHAIN_ALTERNATE = -10,
	CHAIN_PROMOTION = -11,
};

class DoubleHashTable : public Dictionary
{
private:
	IntArray _ht[2];

	Hasher* _hash;

	bool _allowInMatch;

	ChainStrategy _strategy;

	int _lastTableUsed;

public:
	DoubleHashTable(Hasher* hash, const char* name, ChainStrategy strategy, bool allowInMatch)
	: _hash(hash), _allowInMatch(allowInMatch), _strategy(strategy), _lastTableUsed(1)
	{
		_name = name;
	}

	~DoubleHashTable() {
		delete _hash;
	}

	vector<int> potentialMatches(const uchar* data, int ofs) const {
		int h0 = _ht[0][(*_hash)(data+ofs)];
		int h1 = _ht[1][(*_hash)(data+ofs)];

		vector<int> ret;
		if(h0 > h1) {
			if(h0 >= 0) ret.push_back(h0);
			if(h1 >= 0) ret.push_back(h1);
		} else if(h1 > h0) {
			if(h1 >= 0) ret.push_back(h1);
			if(h0 >= 0) ret.push_back(h0);
		} else {
			// should be inserted into one or the other..
			assert(h0 == -1 && h1 == -1);
		}

		return ret;
	}

	void insert(const uchar* data, int ofs, DictionaryEntryType t) {
		if(t == MATCH_WITHIN && !_allowInMatch) {
			return;
		}

		if(_strategy == CHAIN_PROMOTION) {
			_lastTableUsed = (t == MATCH_START) ? 0 : 1;
		} else {
			_lastTableUsed = (_lastTableUsed+1) & 1;
		}

		_ht[_lastTableUsed][(*_hash)(data + ofs)] = ofs;
	}
};

