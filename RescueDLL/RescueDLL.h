#pragma once

#ifdef RESCUEDLL_EXPORTS
#define RESCUEDLL_API __declspec(dllexport)
#else
#define RESCUEDLL_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 800.0f };
constexpr float scr_height{ 650.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 550.0f };

constexpr int ERR_PTR{ 3001 };
constexpr int ERR_INDEX{ 3002 };
constexpr int ERR_PARAM{ 3003 };
constexpr int ERR_UNK{ 3004 };

struct RESCUEDLL_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};
struct RESCUEDLL_API FRECT
{
	float left{ 0 };
	float up{ 0 };
	float right{ 0 };
	float down{ 0 };
};

namespace dll
{
	class RESCUEDLL_API EXCEPTION
	{
	private:
		int err_code = 0;
		
	public:
		EXCEPTION(int what_happened);

		const wchar_t* get_error();
	};

	template<typename T>class BAG
	{
	private:
		size_t m_size{ 0 };
		T* m_ptr{ nullptr };
		size_t m_pos{ 0 };

	public:
		BAG() :m_size{ 1 }, m_ptr{ reinterpret_cast<T*>(calloc(1, sizeof(T))) } {};
		BAG(size_t lenght) :m_size{ lenght }, m_ptr{ reinterpret_cast<T*>(calloc(lenght, sizeof(T))) } {};
		BAG(BAG& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				m_size = other.m_size;
				m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));

				if (!m_ptr)throw EXCEPTION(ERR_PTR);
				else if (other.m_pos > 0)
				{
					for (size_t i = 0; i < other.m_pos; ++i)
					{
						m_ptr[i] = other.m_ptr[i];
						++m_pos;
					}
				}
			}
		}
		BAG(BAG&& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				m_ptr = other.m_ptr;
				m_size = other.m_size;
				m_pos = other.m_pos;

				other.m_ptr = nullptr;
			}
		}

		~BAG()
		{
			free(m_ptr);
		}

		bool empty() const
		{
			return(m_pos == 0);
		}
		size_t size() const
		{
			return m_pos;
		}
		size_t capacity() const
		{
			return m_size;
		}

		void clear()
		{
			free(m_ptr);
			m_size = 1;
			m_pos = 0;
			m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));
		}

		BAG& operator=(BAG& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else if (m_ptr != other.m_ptr)
			{
				free(m_ptr);

				m_size = other.m_size;
				m_pos = other.m_pos;

				m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));

				if (other.m_pos > 0)
					for (size_t i = 0; i < other.m_pos; ++i)m_ptr[i] = other.m_ptr[i];
			}

			return *this;
		}
		BAG& operator=(BAG&& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				free(m_ptr);

				m_size = other.m_size;
				m_pos = other.m_pos;
				m_ptr = other.m_ptr;

				other.m_ptr = nullptr;
			}

			return *this;
		}

		T& operator[](size_t index)
		{
			if (index < 0 || index >= m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);

			return m_ptr[index];
		}

		void push_back(T element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					m_ptr[m_pos] = element;
					++m_pos;
				}
				else
				{
					++m_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if(!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						m_ptr[m_pos] = element;
						++m_pos;
					}
				}
			}
		}
		void push_back(T* element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					m_ptr[m_pos] = *element;
					++m_pos;
				}
				else
				{
					++m_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						m_ptr[m_pos] = *element;
						++m_pos;
					}
				}
			}
		}

		void push_front(T element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					*m_ptr = element;
					++m_pos;
				}
				else
				{
					++m_size;

					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						*m_ptr = element;
						++m_pos;
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					*m_ptr = *element;
					++m_pos;
				}
				else
				{
					++m_size;

					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						*m_ptr = *element;
						++m_pos;
					}
				}
			}
		}

		void erase(size_t index)
		{
			if (index < 0 || index >= m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos == 0)clear();
				else
				{
					for (size_t count = index; count < m_pos - 1; ++count)m_pos[count] = m_pos[count + 1];
					--m_pos;
				}
			}
		}

		void insert(size_t index, T element)
		{
			if (index < 0 || index > m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);

			if (m_pos + 1 <= m_size)
			{
				for (size_t count = m_pos; count > index; --count)m_ptr[count] = m_ptr[count - 1];
				m_ptr[index] = element;
				++m_pos;
			}
			else
			{
				++m_size;
				m_ptr = realloc(m_ptr, m_size * sizeof(T));
				if (!m_ptr)throw EXCEPTION(ERR_PTR);
				else
				{
					for (size_t count = m_pos; count > index; --count)m_ptr[count] = m_ptr[count - 1];
					m_ptr[index] = element;
					++m_pos;
				}
			}
		}
	};

	class RESCUEDLL_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();
		~RANDIT();

		int operator()(int min, int max);
		float operator()(float min, float max);
	};









}