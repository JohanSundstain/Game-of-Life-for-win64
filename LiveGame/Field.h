#pragma once

#include <vector>
#include <ostream>

class Field
{
private:
	static int32_t count;
	std::vector<std::vector<bool>> field;
	int32_t width, height;
	// ws - whitespaces, cl - cell
	char pholder_ws, pholder_cl;
	Field(int32_t width, int32_t height);
public:
	~Field();
	Field();

	int32_t count_neighbors(int32_t x, int32_t y);
	int32_t get_width() const;
	int32_t get_height() const;
	void set_pholder_ws(const char c);
	void set_pholder_cl(const char c);

	static Field create_field(int32_t width, int32_t height);
	static Field* create_field_ptr(int32_t width, int32_t height);

	std::vector<bool>& operator[](int32_t row);
	//safe taking of an element
	bool operator()(int32_t x, int32_t y);

	std::string get_string_view();
	// output operator
	friend std::ostream& operator<<(std::ostream& stream, const Field& f);

	std::vector<std::pair<int32_t, int32_t>> get_filled_cells();

};