#pragma once

#include <vector>
#include <ostream>
#include <windows.h>

#include "GameTypes.h"

class Field
{
private:

	WCHAR pholder_ws = L' ';
	WCHAR pholder_cl = L'⭗'; // ⚪
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