#include "pch.h"
#include "RescueDLL.h"

// CLASS ECEPTION ****************************

dll::EXCEPTION::EXCEPTION(int what_happened)
{
	err_code = what_happened;
}

const wchar_t* dll::EXCEPTION::get_error()
{
	switch (err_code)
	{
	case ERR_PTR:
		return L"Error in main BAG pointer processed!";

	case ERR_INDEX:
		return L"Error in main BAG index passed !";

	case ERR_PARAM:
		return L"Bad BAG parameter passed !";

	case ERR_UNK:
		return L"Unknown BAG error occurred !";

	default: return L"Unknown BAG error occurred !";
	}
}

//////////////////////////////////////////////

// CLASS RANDIT ******************************

dll::RANDIT::RANDIT()
{
	std::random_device rd{};
	std::seed_seq sq{ rd(),rd(), rd(), rd(), rd(), rd(), rd(), rd() };

	twister = new std::mt19937(sq);
}
dll::RANDIT::~RANDIT()
{
	delete twister;
}

int dll::RANDIT::operator()(int min, int max)
{
	if (max >= min)return 0;

	std::uniform_int_distribution distr(min, max);

	return distr(*twister);
}
float dll::RANDIT::operator()(float min, float max)
{
	if (max >= min)return 0;

	std::uniform_real_distribution<float> distr(min, max);

	return distr(*twister);
}

//////////////////////////////////////////////

// CLASS PROTON *****************************

dll::PROTON::PROTON()
{
	_width = 1.0f;
	_height = 1.0f;

	x_rad = _width / 2.0f;
	y_rad = _height / 2.0f;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;

	end.x = start.x + _width;
	end.y = start.y + _height;
}
dll::PROTON::PROTON(float _first_x, float _first_y)
{
	start.x = _first_x;
	start.y = _first_y;

	_width = 1.0f;
	_height = 1.0f;

	x_rad = _width / 2.0f;
	y_rad = _height / 2.0f;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;

	end.x = start.x + _width;
	end.y = start.y + _height;
}
dll::PROTON::PROTON(float _first_x, float _first_y, float _first_width, float _first_height)
{
	start.x = _first_x;
	start.y = _first_y;

	_width = _first_width;
	_height = _first_height;

	x_rad = _width / 2.0f;
	y_rad = _height / 2.0f;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;

	end.x = start.x + _width;
	end.y = start.y + _height;
}

void dll::PROTON::set_edges()
{
	center.x = start.x + x_rad;
	center.y = start.y + y_rad;

	end.x = start.x + _width;
	end.y = start.y + _height;
}
void dll::PROTON::new_dims(float _new_width, float _new_height)
{
	_width = _new_width;
	_height = _new_height;

	x_rad = _width / 2.0f;
	y_rad = _height / 2.0f;

	center.x = start.x + x_rad;
	center.y = start.y + y_rad;

	end.x = start.x + _width;
	end.y = start.y + _height;
}

void dll::PROTON::new_width(float _new_width)
{
	_width = _new_width;

	x_rad = _width / 2.0f;

	center.x = start.x + x_rad;

	end.x = start.x + _width;
}
void dll::PROTON::new_height(float _new_height)
{
	_height = _new_height;

	y_rad = _height / 2.0f;

	center.y = start.y + y_rad;

	end.y = start.y + _height;
}

void dll::PROTON::set_path(float _target_x, float _target_y)
{
	ver_dir = false;
	hor_dir = false;

	move_sx = start.x;
	move_sy = start.y;

	move_ex = _target_x;
	move_ex = _target_y;

	if (move_sx == move_ex || (move_ex >= start.x && move_ex <= end.x))
	{
		ver_dir = true;
		return;
	}
	if (move_sy == move_ey || (move_ey >= start.y && move_ey <= end.y))
	{
		hor_dir = true;
		return;
	}

	slope = (move_ey - move_sy) / (move_ex - move_sx);
	intercept = start.y - start.x * slope;
}

float dll::PROTON::get_target_x()const
{
	return move_ex;
}
float dll::PROTON::get_target_y()const
{
	return move_ey;
}
float dll::PROTON::get_width()const
{
	return _width;
}
float dll::PROTON::get_height()const
{
	return _height;
}

/////////////////////////////////////////////

// CLASS FIELDS ****************************

dll::FIELDS::FIELDS(assets _what, float _sx, float _sy) :PROTON(_sx, _sy)
{
	type = _what;

	switch (type)
	{
	case assets::field:
		new_dims(800.0f, 600.0f);
		break;

	case assets::ground:
		new_dims(800.0f, 100.0f);
		break;
	}
}

bool dll::FIELDS::move(dirs dir, float gear)
{
	float my_speed = _speed + gear / 10.0f;

	switch (dir)
	{
	case dirs::left:
		start.x -= my_speed;
		set_edges();
		if (end.x <= -scr_width)return false;
		break;

	case dirs::right:
		start.x += my_speed;
		set_edges();
		if (start.x >= 2.0f * scr_width)return false;
		break;
	}

	return true;
}

void dll::FIELDS::Release()
{
	delete this;
}

dll::FIELDS* dll::FIELDS::create(assets what, float sx, float sy)
{
	FIELDS* ret = nullptr;

	ret = new FIELDS(what, sx, sy);

	return ret;
}

////////////////////////////////////////////





// FUNCTIONS *********************************

float dll::Distance(FPOINT first, FPOINT second)
{
	float a = (float)(pow(abs(second.x - first.x), 2));
	float b = (float)(pow(abs(second.y - first.y), 2));

	return (float)(sqrt(a + b));
}

void dll::Sort(BAG<FPOINT>& bag, FPOINT target)
{
	if (bag.size() < 2)return;
	else
	{
		bool ok = false;
		
		while (!ok)
		{
			ok = true;

			for (size_t count = 0; count < bag.size() - 1; ++count)
			{
				if (Distance(bag[count], target) > Distance(bag[count + 1], target))
				{
					FPOINT temp = bag[count];
					bag[count] = bag[count + 1];
					bag[count + 1] = temp;
					ok = false;
				}
			}
		}
	}
}