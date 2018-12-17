#include <variant>
#include <iostream>

int main()
{
    // Example with `try-catch` block
    {
        std::variant<int, float> intFloat;
        // Accessing savely the variant
        try
        {
            auto f = std::get<float>(intFloat);
            std::cout << "float!" << std::endl;
        }
        catch (std::bad_variant_access&)
        {
            std::cout << "Variant doesn't hold currently a float!" << std::endl;
        }
    }

    // std::get_if
    {
        std::variant<int, float> intFloat;
        if (const auto intPtr = std::get_if<0>(&intFloat))   // interestingly `get_if` takes a pointer; `get` a reference...
        {
            std::cout << "int!" << std::endl;
        }
    }

    // Visitor
    {
        std::variant<int, float> intFloat;
        intFloat = 42.11f;

        // generic lambda --> works ONLY if types share the same interface
        auto PrintVisitor = [](const auto& t){ std::cout << t << std::endl; };
        std::visit(PrintVisitor, intFloat);
    }

    // Different actions based on active type
    {
        std::variant<int, float> intFloat;
        
        struct MultiplyVisitor
        {
            float m_fFactor;

            MultiplyVisitor(float factor) : m_fFactor(factor) 
            {
            }

            void operator()(int &i) const
            {
                i *= static_cast<int>(m_fFactor);
            }

            void operator()(float &f) const
            {
                f *= m_fFactor;
            }
        };

        intFloat = 10.0f;
        std::visit(MultiplyVisitor(0.5f), intFloat);
    }
}