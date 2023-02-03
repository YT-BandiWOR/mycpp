#include <iostream>
using namespace std;


class String
{	
private:
	char* _buffer;
	size_t _allocated;
	size_t _size;
public:
	static const size_t adding_buffer_size = 16;

	explicit String() {
		this->_buffer = nullptr;
		this->_allocated = 0;
		this->_size = 0;
	}

	String(const String& other) {
		this->_size = other._size;
		this->resize(this->_size + this->adding_buffer_size);
		for (size_t i = 0; i < this->_size; i++)
			this->_buffer[i] = other._buffer[i];
		this->_buffer[this->_size] = '\0';
	}

	String(String&& other) noexcept {
		this->_size = other.size();
		this->resize(this->_size + this->adding_buffer_size);
		for (size_t i = 0; i < this->_size; i++)
			this->_buffer[i] = other._buffer[i];
		this->_buffer[this->_size] = '\0';
	}

	String(const char* cchar_str) {
		this->_size = this->strlen(cchar_str);
		this->resize(this->_size + this->adding_buffer_size);
		for (size_t i = 0; i < this->_size; i++)
			this->_buffer[i] = cchar_str[i];
		this->_buffer[this->_size] = '\0';
	}

	String(char* cchar_str) {
		this->_size = this->strlen(cchar_str);
		this->resize(this->_size + this->adding_buffer_size);
		for (size_t i = 0; i < this->_size; i++)
			this->_buffer[i] = cchar_str[i];
		this->_buffer[this->_size] = '\0';
	}

	String& operator=(const char* cchar_str) {
		auto chars_size = strlen(cchar_str);

		if (!enough_space_for(this->_size, chars_size, this->_allocated))
			resize(chars_size + this->adding_buffer_size);

		for (size_t i = 0; i < chars_size; i++)
			this->_buffer[i] = cchar_str[i];
		this->_size = chars_size;
		this->_buffer[this->_size] = '\0';

		return *this;
	}

	String operator+(const String& other) {
		String new_string;
		new_string.resize(this->_size + other._size + this->adding_buffer_size);
		new_string += this->_buffer;
		new_string += other._buffer;
		return new_string;
	}
	
	String& operator+=(const char* cchar_str) {
		auto chars_size = strlen(cchar_str);

		if (!enough_space_for(this->_size, chars_size, this->_allocated))
			resize(chars_size + this->adding_buffer_size);

		for (size_t i = this->_size; i < this->_size + chars_size; ++i)
			this->_buffer[i] = cchar_str[i - this->_size];
		this->_size += chars_size;
		this->_buffer[this->_size] = '\0';
		
		return *this;
	}

	String& operator+=(const char chr) {
		if (!enough_space_for(this->_size, sizeof(char), this->_allocated))
			resize(this->_allocated + this->adding_buffer_size + sizeof(char));

		this->_buffer[this->_size] = chr;
		this->_buffer[this->_size + 1] = '\0';

		this->_size++;
		return *this;
	}

	String& operator+=(const String& other) {
		if (!enough_space_for(this->_size, other._size, this->_allocated))
			resize(this->_allocated + other._size + this->adding_buffer_size);

		for (size_t i = this->_size; i < (this->_size + other._size); i++)
			this->_buffer[i] = other._buffer[i - this->_size];
		
		return *this;
	}

	String operator*(size_t _n) {
		size_t chars_size = _n * this->_size;
		String new_string;
		new_string.resize(chars_size + this->adding_buffer_size);

		for (size_t n = 0; n < _n; ++n)
			for (size_t i = 0; i < this->_size; ++i)
				new_string.at(n * this->_size + i) = this->_buffer[i];
		new_string.at(chars_size) = '\0';

		return new_string;
	}

	String lower() {
		String new_string(this->_buffer);
		for (size_t i = 0; i < this->_size; i++)
			if (new_string[i] >= 'A' && new_string[i] <= 'Z')
				new_string[i] += 32; // 97(a) - 65(A)
			else if (new_string[i] >= 'À' && new_string[i] <= 'ß')
				new_string[i] += 32; // -32(à) + 64(À)
		return new_string;
	}

	String upper() {
		String new_string(this->_buffer);
		for (size_t i = 0; i < this->_size; i++)
			if (new_string[i] >= 'a' && new_string[i] <= 'z')
				new_string[i] += -32; // 65(A) - 97(a)
			else if (new_string[i] >= 'à' && new_string[i] <= 'ÿ')
				new_string[i] += -32; // 64(À) - 32(à)
		return new_string;
	}

	String reverse() {
		String new_string(this->_buffer);
		for (size_t i = 0; i < this->_size / 2; i++)
			this->swap(&new_string.at(i), &new_string.at(this->_size - i - 1));
		return new_string;
	}

	String slice(size_t start, size_t stop = 0, int step = 1) {
		if (stop == 0)
			stop = this->_size;
		if (start > stop)
			this->swap(&start, &stop);

		String new_string;
		long long chars_size = ceil((double)(stop - start) / (double)abs(step));
		new_string.resize(chars_size + this->adding_buffer_size);

		if (step > 0)
			for (size_t i = start; i < stop; i += step)
				new_string += this->_buffer[i];
		else if (step < 0)
			for (long long i = stop - 1; i >= start; i += step)
				if (i >= 0)
					new_string += this->_buffer[i];
				else
					break;
		else
			new_string.clear();

		return new_string;
	}

	bool isDigit() {
		bool check = true;
		for (size_t i = 0; i < this->_size; i++)
			if (!(this->_buffer[i] >= '0' && this->_buffer[i] <= '9'))
				check = false;
		return check;
	}

	bool isAlpha() { // Return true if ALL chars is literal (empty string => false)
		bool check = this->_size > 0;

		for (size_t i = 0; i < this->_size; i++) {
			auto c = this->_buffer[i];
			if (!(
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= 'à' && c <= 'ÿ') ||
				(c >= 'À' && c <= 'ß')
				)) {
				check = false;
			}
		}

		return check;
	}

	String replace(String& from, String& to, long long max = -1) {
		if (this->empty()) return String();

		String buffer, new_string;
		buffer.resize(from.size());
		new_string.resize(this->adding_buffer_size);

		for (size_t i = 0; i < this->_size; i++)
		{
			auto c = this->_buffer[i];
			buffer += c;
			if (buffer == from) {
				new_string += to;
				buffer.clear();
			}
			else if (buffer.size() == from.size()) {
				new_string += buffer;
				buffer.clear();
			}
			else
				if (i + 1 == this->_size)
					new_string += buffer;
		}

		return new_string;
	}

	void clear() {
		this->_size = 0;

		if (this->_allocated > 0)
			this->_buffer[0] = '\0';
		else {
			this->resize(this->adding_buffer_size);
			this->_buffer[0] = '\0';
		}
	}

	~String()
	{
		if (this->_allocated) {
			delete[] this->_buffer;
		}
	}

	char* c_str() {
		return this->_buffer;
	}

	size_t size() {
		return this->_size;
	}

	size_t allocated() {
		return this->_allocated;
	}

	bool operator==(const String& other) {
		if (other._size != this->_size) return false;

		for (size_t i = 0; i < this->_size; i++)
			if (this->_buffer[i] != other._buffer[i])
				return false;

		return true;
	}

	char& operator[](size_t index) {
		return this->_buffer[index];
	}

	char& at(size_t index) {
		return this->_buffer[index];
	}

	static bool enough_space_for(size_t size, size_t bytes, size_t allocated) {
		return size + bytes <= allocated;
	}

	bool empty() {
		return this->_size == 0;
	}

	bool resize(size_t new_size) {
		if (new_size > this->_allocated) {
			this->_allocated = new_size;
			auto new_buffer = new char[this->_allocated + 1];
			if (this->_buffer)
				for (size_t i = 0; i < this->_allocated + 1; i++) new_buffer[i] = this->_buffer[i];
			delete[] this->_buffer;
			this->_buffer = new_buffer;
			return true;
		}
		else if (new_size < this->_allocated) {
			this->_allocated = new_size;
			this->_size = new_size;
			auto new_buffer = new char[this->_allocated + 1];
			for (size_t i = 0; i < this->_allocated; i++) new_buffer[i] = this->_buffer[i];
			new_buffer[this->_allocated] = '\0';
			delete[] this->_buffer;
			this->_buffer = new_buffer;
			return true;
		}
		return false;
	}

	static size_t strlen(const char* str) {
		size_t size = 0;
		while (str[size] != '\0') ++size;
		return size;
	}

	static size_t strlen(char* str) {
		size_t size = 0;
		while (str[size] != '\0') ++size;
		return size;
	}

	template <typename T>
	static void swap(T* left, T* right) {
		T temp = *left;
		*left = *right;
		*right = temp;
	}

	friend ostream& operator << (ostream& out_stream, const String& string) {
		out_stream << string._buffer;
		return out_stream;
	}

	size_t getsizeof() {
		return sizeof(*this) + this->_allocated + 1;
	}

	static size_t getsizeof(String& object) {
		return sizeof(object) + object.allocated() + 1;
	}
};

int main() 
{
	setlocale(0, "Rus");
	String str_1 = "Äåöë";
	String str_2;

	cout << str_1.isAlpha() << endl;
}