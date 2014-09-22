
/** An "infinitely long" array of integers, all of which default to -1 */
class IntArray {
private:
	vector<int> _v;

public:
	IntArray()
	: _v(1024, -1) // avoid infinite loop on first call to operator[]
	{
		return;
	}

	int& operator[](int n) {
		assert(n >= 0 && n < INT_MAX/2);
		if((unsigned)n >= _v.size()) {
			size_t s = _v.size();
			do {
				s *= 2;
			} while((unsigned)n >= s);
			_v.resize(s, -1);
		}

		return _v[n];
	}

	int operator[](int n) const {
		assert(n >= 0 && n < INT_MAX/2);
		if((unsigned)n >= _v.size()) {
			return -1;
		}
		return _v[n];
	}
};


