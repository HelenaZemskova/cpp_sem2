// g++ -std=c++23 -Wall -Wextra -pedantic 12_05.cpp -o 12_05

#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
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

	void test()
	{
		std::string string;

		std::print("Calculator::test : enter std::string string(s) : \n");

		while (std::getline(std::cin >> std::ws, string, '\n'))
		{
			if (Stream stream(string); !stream.empty())
			{
				std::print("Calculator::test : {} = {}\n", string, statement(stream));
			}
			else
			{
				break;
			}
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
	assert(std::abs(calculator.evaluate("5!") - 120.0) < 1e-9);
	assert(std::abs(calculator.evaluate("2 ^ 3 ^ 2") - 512.0) < 1e-9);
	assert(std::abs(calculator.evaluate("3!!") - 720.0) < 1e-9);
	assert(std::abs(calculator.evaluate("(2+3)!") - 120.0) < 1e-9);
	assert(std::abs(calculator.evaluate("[2+3]*4") - 20.0) < 1e-9);
	assert(std::abs(calculator.evaluate("{2+3}*4") - 20.0) < 1e-9);
	assert(std::abs(calculator.evaluate("(2^3)!%100") - 20.0) < 1e-9);

	std::print("Enter expression for manual test (empty line to skip): ");
	std::string input;
	if (std::getline(std::cin, input) && !input.empty())
	{
		std::print("{} = {}\n", input, calculator.evaluate(input));
	}

	std::print("All tests passed\n");
}