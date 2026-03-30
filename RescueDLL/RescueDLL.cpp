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