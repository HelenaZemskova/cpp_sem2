// g++ -std=c++23 -Wall -Wextra -pedantic 13_02.cpp -o 13_02

#include <cassert>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ios>
#include <istream>
#include <print>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

class Stream
{
public :

	using token_t = std::variant < char, double, std::string > ;

	Stream(std::string const & string) : m_stream(string + ';') {}

	auto empty()
	{
		return m_stream.peek() == ';';
	}

	auto get()
	{
		if (m_has_token)
		{
			m_has_token = false;

			return m_token;
		}

		auto x = '\0';

		m_stream >> x;

		switch (x)
		{
			case '+' : case '-' : case '*' : case '/' :
			case '%' : case '^' : case '!' :
			case '(' : case ')' :
			case '[' : case ']' :
			case '{' : case '}' :
			case ';' :
			{
				return token_t(x);
			}

			case '0' : case '1' : case '2' : case '3' : case '4' :

			case '5' : case '6' : case '7' : case '8' : case '9' :

			case '.' :
			{
				m_stream.unget();

				auto y = 0.0;

				m_stream >> y;

				return token_t(y);
			}

			default :
			{
				std::string string(1, x);

				while (m_stream.get(x) && (std::isalpha(x) || std::isdigit(x)))
				{
					string += x;
				}

				if (!std::isspace(x))
				{
					m_stream.unget();
				}

				return token_t(string);
			}
		}
	}

	void put(token_t const & token)
	{
		m_token = token;

		m_has_token = true;
	}

private :

	std::stringstream m_stream;

	token_t m_token;

	bool m_has_token = false;
};

static auto factorial(double x) -> double
{
	if (x <= 0.0) return 1.0;

	return x * factorial(x - 1.0);
}

class Calculator
{
public :

	auto evaluate(std::string const & string) -> double
	{
		Stream stream(string);

		return statement(stream);
	}

	void test(std::string const & path)
	{
		std::fstream stream(path, std::ios::in);

		std::string expression;

		while (std::getline(stream >> std::ws, expression))
		{
			auto expected = 0.0;

			stream >> expected;

			auto const result = evaluate(expression);

			assert(std::abs(result - expected) < 1e-9);

			std::print("Calculator::test : {} = {}\n", expression, result);
		}
	}

private :

	auto statement(Stream & stream) -> double
	{
		auto token = stream.get();

		if (std::holds_alternative < std::string > (token))
		{
			if (std::get < std::string > (token) == "set")
			{
				return declaration(stream);
			}
		}

		stream.put(token);

		return expression(stream);
	}

	auto declaration(Stream & stream) -> double
	{
		auto string = std::get < std::string > (stream.get());

		m_variables[string] = expression(stream);

		return m_variables[string];
	}

	auto expression(Stream & stream) const -> double
	{
		auto x = term(stream);

		auto token = stream.get();

		while (true)
		{
			switch (std::get < char > (token))
			{
				case '+' : { x += term(stream); break; }

				case '-' : { x -= term(stream); break; }

				default  :
				{
					stream.put(token);

					return x;
				}
			}

			token = stream.get();
		}
	}

	auto term(Stream & stream) const -> double
	{
		auto x = power(stream);

		auto token = stream.get();

		while (true)
		{
			switch (std::get < char > (token))
			{
				case '*' : { x *= power(stream); break; }

				case '/' : { x /= power(stream); break; }

				case '%' : { x = std::fmod(x, power(stream)); break; }

				default  :
				{
					stream.put(token);

					return x;
				}
			}

			token = stream.get();
		}
	}

	auto power(Stream & stream) const -> double
	{
		auto x = postfix(stream);

		auto token = stream.get();

		if (std::holds_alternative < char > (token) && std::get < char > (token) == '^')
		{
			return std::pow(x, power(stream));
		}

		stream.put(token);

		return x;
	}

	auto postfix(Stream & stream) const -> double
	{
		auto x = primary(stream);

		auto token = stream.get();

		while (std::holds_alternative < char > (token) && std::get < char > (token) == '!')
		{
			x = factorial(x);

			token = stream.get();
		}

		stream.put(token);

		return x;
	}

	auto primary(Stream & stream) const -> double
	{
		auto token = stream.get();

		if (std::holds_alternative < char > (token))
		{
			switch (std::get < char > (token))
			{
				case '(' : case '[' : case '{' :
				{
					auto x = expression(stream);

					stream.get();

					return x;
				}

				case '+' : { return primary(stream); }

				case '-' : { return -1 * primary(stream); }
			}
		}

		if (std::holds_alternative < double > (token))
		{
			return std::get < double > (token);
		}

		return m_variables.at(std::get < std::string > (token));
	}

	std::unordered_map < std::string, double > m_variables;
};

int main()
{
	Calculator calculator;

	assert(std::abs(calculator.evaluate("10 % 3") - 1.0) < 1e-9);
	assert(std::abs(calculator.evaluate("2 ^ 10") - 1024.0) < 1e-9);
	assert(std::abs(calculator.evaluate("5!") - 120.0)  < 1e-9);
	assert(std::abs(calculator.evaluate("2 ^ 3 ^ 2") - 512.0) < 1e-9);
	assert(std::abs(calculator.evaluate("3!!") - 720.0) < 1e-9);
	assert(std::abs(calculator.evaluate("(2+3)!") - 120.0) < 1e-9);
	assert(std::abs(calculator.evaluate("[2+3]*4") - 20.0) < 1e-9);
	assert(std::abs(calculator.evaluate("{2+3}*4") - 20.0) < 1e-9);
	assert(std::abs(calculator.evaluate("(2^3)!%100") - 20.0) < 1e-9);

	auto const path = std::string("13_02.data");

	{
		std::fstream stream(path, std::ios::out);

		stream << "10 % 3\n" << "1\n";
		stream << "2^10\n" << "1024\n";
		stream << "5!\n" << "120\n";
		stream << "2 ^ 3 ^ 2\n" << "512\n";
		stream << "3!!\n" << "720\n";
		stream << "(2+3)!\n" << "120\n";
		stream << "[2+3]*4\n" << "20\n";
		stream << "{2+3}*4\n" << "20\n";
		stream << "(2^3)!%100\n" << "20\n";
	}

	calculator.test(path);

	std::filesystem::remove(path);

	std::print("All tests passed\n");
}
