// g++ -std=c++23 -Wall -Wextra -pedantic 12_04.cpp -o 12_04

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <print>
#include <regex>
#include <string>
#include <vector>


struct email_t
{
    std::string address;
    std::string domain;
};


auto extract_emails(std::string const & text) -> std::vector < email_t >
{
    static std::regex const pattern(R"([a-zA-Z0-9.+_-]+@([a-zA-Z0-9.-]+\.[a-zA-Z]{2,}))");

    std::vector < email_t > result;

    std::ranges::for_each
    (
        std::sregex_iterator(std::cbegin(text), std::cend(text), pattern),

        std::sregex_iterator(),

        [&result](auto const & match){ result.push_back({ match[0].str(), match[1].str() }); }
    );

    return result;
}

int main()
{

    {
        auto text = std::string(R"(
Hello, please contact us at support@example.com or admin@company.org.
For billing, email billing@service.net.
)");

        auto emails = extract_emails(text);

        assert(emails.size() == 3);

        assert(emails[0].address == "support@example.com");
        assert(emails[0].domain == "example.com");

        assert(emails[1].address == "admin@company.org");
        assert(emails[1].domain == "company.org");

        assert(emails[2].address == "billing@service.net");
        assert(emails[2].domain == "service.net");

        for (auto const & [address, domain] : emails)
            std::print("{:30} domain : {}\n", address, domain);
    }

    {
        auto text = std::string(R"(
Users: alice@gmail.com, bob.smith@yahoo.com, charlie_99@hotmail.co.uk
No-email text here.
)");

        auto emails = extract_emails(text);

        assert(emails.size() == 3);

        assert(emails[0].address == "alice@gmail.com");
        assert(emails[0].domain == "gmail.com");

        assert(emails[1].address == "bob.smith@yahoo.com");
        assert(emails[1].domain == "yahoo.com");

        assert(emails[2].address == "charlie_99@hotmail.co.uk");
        assert(emails[2].domain == "hotmail.co.uk");

        std::print("\n");

        for (auto const & [address, domain] : emails)
            std::print("{:30} domain : {}\n", address, domain);
    }

    {
        auto text = std::string(R"(
Please send to qa-team+alerts@sub.example.co.uk, dev_ops@corp-mail.io!
Also cc: qa-team+alerts@sub.example.co.uk; done.
)");

        auto emails = extract_emails(text);

        assert(emails.size() == 3);

        assert(emails[0].address == "qa-team+alerts@sub.example.co.uk");
        assert(emails[0].domain == "sub.example.co.uk");

        assert(emails[1].address == "dev_ops@corp-mail.io");
        assert(emails[1].domain == "corp-mail.io");

        assert(emails[2].address == "qa-team+alerts@sub.example.co.uk");
        assert(emails[2].domain == "sub.example.co.uk");
    }

    {
        auto text = std::string(R"(
bad@@example.com
user@localhost
name@domain
@missing.local
)");

        auto emails = extract_emails(text);

        assert(emails.empty());
    }

    {
        auto text = std::string(R"(This text contains no email addresses at all.)");

        auto emails = extract_emails(text);

        assert(emails.empty());
    }

    std::print("Enter text for manual extraction (empty line to skip): ");
    std::string input;
    if (std::getline(std::cin, input) && !input.empty())
    {
        auto emails = extract_emails(input);
        if (emails.empty())
        {
            std::print("No emails found\n");
        }
        else
        {
            for (auto const & [address, domain] : emails)
                std::print("{:30} domain : {}\n", address, domain);
        }
    }

    std::print("All tests passed\n");
}
