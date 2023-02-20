#include <iostream>
using namespace std;


struct CharsMemory {
	char* buffer;
	size_t allocated;
	size_t links;
	size_t size;

	CharsMemory(size_t size = 0, size_t adding_allocated = 0) {
		this->links = 0;
		this->size = size;
		this->allocated = this->size + adding_allocated;

		if (allocated == 0) {
			this->buffer = nullptr;
		}
		else {
			this->buffer = new char[this->allocated + 1];
			this->buffer[this->size] = '\0';
		}
	}

	void leave_from_string() {
		this->links--;
		if (this->links == 0 && this->buffer != nullptr)
			delete[] buffer;
	}
};


class String
{	
private:
	CharsMemory* chars;
	static const size_t adding_buffer_size = 16;

public:
	explicit String(bool create_memory = true) {
		this->chars = (create_memory) ? (new CharsMemory(0, 1)) : (nullptr);
	}

	String(const String& other) {
		this->chars = other.chars;
	}

	String(String&& other) noexcept {
		this->chars = other.chars;
	}

	String(const char cchar) {
		this->chars = new CharsMemory(1, this->adding_buffer_size);
		this->chars->buffer[0] = cchar;
	}

	String(const char* cchar_str) {
		this->chars = new CharsMemory(this->strlen(cchar_str), this->adding_buffer_size);
		for (size_t i = 0; i < this->chars->size; i++) this->chars->buffer[i] = cchar_str[i];
	}

	String(char* cchar_str) {
		this->chars = new CharsMemory(this->strlen(cchar_str), this->adding_buffer_size);
		for (size_t i = 0; i < this->chars->size; i++) this->chars->buffer[i] = cchar_str[i];
	}

	explicit String(CharsMemory* chars_memory) {
		this->chars = chars_memory;
	}

	~String()
	{
		this->leave_from_chars_memory();
	}

	String& operator=(const char* cchar_str) {
		this->leave_from_chars_memory();
		this->chars = new CharsMemory(this->strlen(cchar_str), this->adding_buffer_size);
		for (size_t i = 0; i < this->chars->size; i++) this->chars->buffer[i] = cchar_str[i];
		return *this;
	}

	String operator+(const String& other) {
		String new_string(false); // false - don't create CharsMemory

		auto chars_memory = new CharsMemory(this->chars->size + other.chars->size, this->adding_buffer_size);
		new_string.set_new_chars_memory(chars_memory);

		for (size_t i = 0; i < this->chars->size; i++)
			chars_memory->buffer[i] = this->chars->buffer[i];

		for (size_t i = 0; i < other.chars->size; i++)
			chars_memory->buffer[i + this->chars->size] = other.chars->buffer[i];

		return new_string;
	}

	String& operator+=(const String& other) {
		if (this->chars->links <= 1 && this->chars->allocated >= this->chars->size + other.chars->size) { // если строка CharBuffer только у этого класса, и нам хватит места для операции конкатенации
			this->chars->buffer[this->chars->size + other.chars->size] = '\0';
			for (size_t i = 0; i < other.chars->size; i++)
				this->chars->buffer[i + this->chars->size] = other.chars->buffer[i];
			this->chars->size += other.chars->size;
		}
		else {
			auto new_chars = new CharsMemory(this->chars->size + other.chars->size, this->adding_buffer_size);

			for (size_t i = 0; i < this->chars->size; i++)
				new_chars->buffer[i] = this->chars->buffer[i];
			for (size_t i = 0; i < other.chars->size; i++)
				new_chars->buffer[i + this->chars->size] = other.chars->buffer[i];

			set_new_chars_memory(new_chars);
		}
		
		return *this;
	}

	String& operator+=(const char c) {
		if (this->chars->links <= 1 && this->chars->allocated >= this->chars->size + 1) {
			this->chars->buffer[this->chars->size++] = c;
			this->chars->buffer[this->chars->size] = '\0';
		}
		else {
			auto new_chars = new CharsMemory(this->chars->size + 1, this->adding_buffer_size);

			for (size_t i = 0; i < this->chars->size; i++) new_chars->buffer[i] = this->chars->buffer[i];
			this->chars->buffer[this->chars->size + 1] = c;

			set_new_chars_memory(new_chars);
		}

		return *this;
	}

	String operator*(size_t _n) {
		String new_string(false); // false - don't create CharsMemory

		auto chars_memory = new CharsMemory(this->chars->size * _n, this->adding_buffer_size);
		new_string.set_new_chars_memory(chars_memory);

		for (size_t n = 0; n < _n; n++)
			for (size_t i = 0; i < this->chars->size; i++)
				chars_memory->buffer[n*i+i] = this->chars->buffer[i];

		return new_string;
	}

	String& operator*=(size_t _n) {
		if (_n <= 1) {
			if (_n == 0)
				this->clear();
			return *this;
		}

		if (this->chars->links <= 1 && this->chars->allocated >= this->chars->size * _n) { // если строка CharBuffer только у этого класса, и нам хватит места для операции конкатенации
			this->chars->buffer[this->chars->size * _n] = '\0';
			for (size_t n = 1; n < _n; n++)
				for (size_t i = 0; i < this->chars->size; i++)
					this->chars->buffer[n * i + i] = this->chars->buffer[i];

			this->chars->size *= _n;
		}
		else {
			auto new_chars = new CharsMemory(this->chars->size *= _n, this->adding_buffer_size);
			for (size_t n = 0; n < _n; n++)
				for (size_t i = 0; i < this->chars->size; i++)
					new_chars->buffer[n * i + i] = this->chars->buffer[i];

			set_new_chars_memory(new_chars);
		}

		return *this;
	}

	String lower() {
		String new_string(false);
		auto new_chars = new CharsMemory(this->chars->size, this->adding_buffer_size);
		new_string.set_new_chars_memory(new_chars);

		for (size_t i = 0; i < this->chars->size; i++)
			if ((new_chars->buffer[i] >= 'A' && new_chars->buffer[i] <= 'Z') ||
				(new_chars->buffer[i] >= 'А' && new_chars->buffer[i] <= 'Я'))
				new_chars->buffer[i] += 32; // 97(a) - 65(A)

		return new_string;
	}

	String upper() {
		String new_string(false);
		auto new_chars = new CharsMemory(this->chars->size, this->adding_buffer_size);
		new_string.set_new_chars_memory(new_chars);

		for (size_t i = 0; i < this->chars->size; i++)
			if ((new_chars->buffer[i] >= 'A' && new_chars->buffer[i] <= 'Z') ||
				(new_chars->buffer[i] >= 'А' && new_chars->buffer[i] <= 'Я'))
				new_chars->buffer[i] -= 32; // 97(a) - 65(A)

		return new_string;
	}

	String reverse() {
		String new_string(false);
		auto new_chars = new CharsMemory(this->chars->size, this->adding_buffer_size);
		new_string.set_new_chars_memory(new_chars);
		
		for (size_t i = 0; i < this->chars->size / 2; i++)
			this->swap(new_chars->buffer[i], new_chars->buffer[this->chars->size - i - 1]);
		
		return new_string;
	}

	String slice(size_t start, size_t stop = 0, int step = 1) {
		if (stop == 0)
			stop = this->chars->size;
		if (start > stop)
			this->swap(start, stop);

		String new_string;
		auto new_chars = new CharsMemory(ceil((double)(stop - start) / (double)abs(step)), this->adding_buffer_size);
		new_string.set_new_chars_memory(new_chars);

		size_t buffer_index = 0;

		if (step > 0)
			for (size_t i = start; i < stop; i += step)
				new_chars->buffer[buffer_index++] = this->chars->buffer[i];

		else if (step < 0)
			for (long long i = stop - 1; i >= start; i += step)
				if (i >= 0)
					new_chars->buffer[buffer_index++] = this->chars->buffer[i];
				else
					break;
		else
			new_string.clear();

		return new_string;
	}

	bool isDigit() { // Return true if ALL chars is decimals
		bool check = true;
		for (size_t i = 0; i < this->chars->size; i++)
			if (!(this->chars->buffer[i] >= '0' && this->chars->buffer[i] <= '9'))
				check = false;
		return check;
	}

	bool isAlpha() { // Return true if ALL chars is literal (empty string => false)
		bool check = this->chars->size > 0;

		for (size_t i = 0; i < this->chars->size; i++) {
			auto c = this->chars->buffer[i];
			if (!(
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= 'а' && c <= 'я') ||
				(c >= 'А' && c <= 'Я')
				)) {
				check = false;
			}
		}

		return check;
	}

	String replace(const String& from, const String& to, long long limit = -1) {
		if (this->empty() || (from.chars == nullptr) || (from.chars->size == 0)) return String();
		if (this->chars->size < from.chars->size) return *this;

		String buffer(new CharsMemory(0, from.chars->size));
		String new_string;

		size_t points_counter = 0;

		for (size_t i = 0; (i < this->chars->size) && (limit != 0); i++)
		{
			auto c = this->chars->buffer[i];

			if (from.chars->buffer[points_counter] == c) {
				if (from.chars->size == buffer.size()) {
					new_string += buffer;
					buffer.clear();
					points_counter = 0;
					limit--;
				}
				else {
					buffer += c;
					points_counter++;
				}
			}
			else {
				new_string += buffer;
				new_string += c;
				points_counter = 0;
				buffer.clear();
			}
		}

		return new_string;
	}

	void clear() {
		if (this->chars == nullptr || this->chars->links == 1)
			set_new_chars_memory(new CharsMemory(0, this->adding_buffer_size));
		else
			this->chars->buffer[0] = '\0';
	}

	void leave_from_chars_memory() {
		if (this->chars != nullptr) {
			this->chars->leave_from_string();
			if (this->chars->links == 0) delete this->chars;
		}
	}

	void set_new_chars_memory(CharsMemory* chars_memory) {
		leave_from_chars_memory();
		this->chars = chars_memory;
	}

	char* c_str() {
		return this->chars->buffer;
	}

	CharsMemory* getChars() {
		return this->chars;
	}

	size_t size() {
		return this->chars->size;
	}

	size_t allocated() {
		return this->chars->allocated;
	}

	bool operator==(const String& other) {
		if (other.chars == this->chars) return true;
		if (other.chars->size != this->chars->size) return false;

		for (size_t i = 0; i < this->chars->size; i++)
			if (this->chars->buffer[i] != other.chars->buffer[i])
				return false;
		
		return true;
	}

	char& operator[](size_t index) {
		return this->chars->buffer[index];
	}

	char& at(size_t index) {
		return this->chars->buffer[index];
	}

	static bool enough_space_for(size_t size, size_t bytes, size_t allocated) {
		return size + bytes <= allocated;
	}

	bool empty() {
		return (this->chars == nullptr) || (this->chars->size == 0);
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
	static void swap_ptr(T* left, T* right) {
		T temp = *left;
		*left = *right;
		*right = temp;
	}

	template <typename T>
	static void swap(T left, T right) {
		T temp = left;
		left = right;
		right = temp;
	}

	friend ostream& operator << (ostream& out_stream, const String& string) {
		return out_stream << string.chars->buffer;
	}

	size_t getsizeof() {
		if (this->chars)
			return (sizeof(*this)) + this->chars->allocated + 1;
		else
			return sizeof(*this);
	}

	static size_t getsizeof(const String& object) {
		if (object.chars)
			return (sizeof(object)) + object.chars->allocated + 1;
		else
			return sizeof(object);
	}
};

int main() 
{
	setlocale(0, "Rus");
	
	String str("ор");
	
	cout << str.replace("р", "к") << endl;
}