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

// CLASS ASSETS ****************************

dll::ASSETS::ASSETS(assets _what, float _sx, float _sy) :PROTON(_sx, _sy)
{
	type = _what;

	switch (type)
	{
	case assets::civilian:
		new_dims(44.0f, 80.0f);
		max_frames = 16;
		frame_delay = 4;
		break;

	case assets::supply :
		new_dims(40.0f, 51.0f);
		if (_Rand(0, 4) == 2)set_path(start.x - 50.0f, ground);
		else set_path(end.x + 50.0f, ground);
		max_frames = 26;
		frame_delay = 3;
		break;
	}

	max_frame_delay = frame_delay;
}

bool dll::ASSETS::move(dirs dir, float gear)
{
	float my_speed = _speed + gear / 10.0f;

	if (type == assets::civilian)
	{
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
	}
	else
	{
		if (hor_dir)
		{
			if (move_ex < move_sx)
			{
				this->dir = dirs::left;
				start.x -= my_speed;
				set_edges();
				if (start.x <= move_ex)
				{
					set_path(end.x + 100.0f, ground);
					dir = dirs::right;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
			else
			{
				this->dir = dirs::right;
				start.x += my_speed;
				set_edges();
				if (end.x >= move_ex)
				{
					set_path(start.x - 100.0f, ground);
					dir = dirs::left;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
		}
		else if (ver_dir)
		{
			if (move_ey < move_sy)
			{
				start.y -= my_speed;
				set_edges();
				if (start.y <= move_ey)
				{
					set_path(end.x + 100.0f, ground);
					this->dir = dirs::right;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
			else
			{
				start.y += my_speed;
				set_edges();
				if (end.y >= move_ey)
				{
					set_path(start.x - 100.0f, ground);
					this->dir = dirs::left;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
		}
		else
		{
			if (move_ex < move_sx)
			{
				this->dir = dirs::left;
				start.x -= my_speed;
				start.y = start.x * slope + intercept;
				set_edges();
				if (start.x <= move_ex)
				{
					set_path(end.x + 100.0f, ground);
					dir = dirs::right;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
			else
			{
				this->dir = dirs::right;
				start.x += my_speed;
				start.y = start.x * slope + intercept;
				set_edges();
				if (end.x >= move_ex)
				{
					set_path(start.x - 100.0f, ground);
					dir = dirs::left;
				}
				if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
			}
		}
	}

	return true;
}

void dll::ASSETS::Release()
{
	delete this;
}

int dll::ASSETS::get_frame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = max_frame_delay;
		++frame;
		if (frame > max_frames)frame = 0;
	}
	return frame;
}

dll::ASSETS* dll::ASSETS::create(assets what, float sx, float sy)
{
	ASSETS* ret = nullptr;

	ret = new ASSETS(what, sx, sy);

	return ret;
}

////////////////////////////////////////////

// CLASS SHOTS ****************************

dll::SHOTS::SHOTS(float _first_x, float _first_y, float _end_x, float _end_y) :PROTON(_first_x, _first_y, 15.0f, 15.0f)
{
	set_path(_end_x, _end_y);
}

bool dll::SHOTS::move(float gear)
{
	float my_speed = _speed + gear / 10.0f;

	if (hor_dir)
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.x += my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}
	else if (ver_dir)
	{
		if (move_ey < move_sy)
		{
			start.y -= my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.y += my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}
	else
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			start.y = start.x * slope + intercept;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.x += my_speed;
			start.y = start.x * slope + intercept;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}

	return true;
}

void dll::SHOTS::Release()
{
	delete this;		
}

dll::SHOTS* dll::SHOTS::create(float first_x, float first_y, float end_x, float end_y)
{
	SHOTS* ret{ nullptr };

	ret = new SHOTS(first_x, first_y, end_x, end_y);
	
	return ret;
}

//////////////////////////////////////////

// CLASS SHOTS ****************************

dll::METEORS::METEORS(meteors _type, float _first_x, float _first_y, float _end_x, float _end_y) :PROTON(_first_x, _first_y)
{
	type = _type;

	switch (type)
	{
	case meteors::big:
		new_dims(100.0f, 120.0f);
		_speed = 0.8f;
		break;

	case meteors::mid:
		new_dims(90.0f, 80.0f);
		_speed = 1.1f;
		break;
	}

	set_path(_end_x, _end_y);
}

bool dll::METEORS::move(float gear)
{
	float my_speed = _speed + gear / 10.0f;

	if (hor_dir)
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.x += my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}
	else if (ver_dir)
	{
		if (move_ey < move_sy)
		{
			start.y -= my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.y += my_speed;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}
	else
	{
		if (move_ex < move_sx)
		{
			start.x -= my_speed;
			start.y = start.x * slope + intercept;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
		else
		{
			start.x += my_speed;
			start.y = start.x * slope + intercept;
			set_edges();
			if (end.x <= -scr_width || start.x >= 2.0f * scr_width || end.y <= 0 || end.y >= ground)return false;
		}
	}

	return true;
}

void dll::METEORS::Release()
{
	delete this;
}

dll::METEORS* dll::METEORS::create(meteors type, float first_x, float first_y, float end_x, float end_y)
{
	METEORS* ret{ nullptr };

	ret = new METEORS(type, first_x, first_y, end_x, end_y);

	return ret;
}

///////////////////////////////////////////

// CLASS GUN ******************************

dll::GUN::GUN(float _sx, float _sy) :PROTON(_sx, _sy, 85.0f, 94.0f) {};

bool dll::GUN::move(dirs dir, float gear)
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

int dll::GUN::attack()
{
	--delay;
	if (delay <= 0)
	{
		delay = 100;
		return damage;
	}
	return 0;
}

int dll::GUN::get_frame()
{
	--frame_delay;
	if (frame_delay <= 0)
	{
		frame_delay = 13;
		++frame;
		if (frame > 3)frame = 0;
	}
	return frame;
}

void dll::GUN::Release()
{
	delete this;
}

dll::GUN* dll::GUN::create(float sx, float sy)
{
	GUN* ret{ nullptr };

	ret = new GUN(sx, sy);
	
	return ret;
}

///////////////////////////////////////////






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